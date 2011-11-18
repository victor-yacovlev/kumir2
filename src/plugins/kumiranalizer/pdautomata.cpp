#include "pdautomata.h"
#include "pdautomata_p.h"
#include "dataformats/ast.h"
#include "interfaces/lexemtype.h"
#include "errormessages/errormessages.h"

#define isTerminal(x) QRegExp(QString::fromUtf8("[a-z|а-я][a-z|а-я|_]*")).exactMatch(x)
#define isNeterminal(x) QRegExp(QString::fromUtf8("[А-Я|A-Z][А-Я|A-Z|1-9|_*]*")).exactMatch(x)
#define isNagruzka(x) ( x.startsWith("{") && x.endsWith("}") )

#define MAXIMUM_ERRORS_COUNT 20

namespace KumirAnalizer {

AST::Module * moduleOfAlgorhitm(AST::Data * data, AST::Algorhitm * alg) {
    foreach (AST::Module * mod, data->modules) {
        if (mod->impl.algorhitms.contains(alg)) {
            return mod;
        }
    }
    return 0;
}

void prepareRules(const QStringList &files, QString &out, const QList<int> &priors);
QString terminalByCode(int code);
bool isCorrectTerminal(const QString &terminal);

PDAutomata::PDAutomata(QObject *parent) :
    QObject(parent)
{
    d = new PDAutomataPrivate();
    d->q = this;
    QString rulesPath = qApp->property("sharePath").toString()+"/kumiranalizer/"; // TODO set me
    d->loadRules(rulesPath);
}

void PDAutomata::init(bool teacherMode, const QList<Statement*> & statements, AST::Data *ast, AST::Algorhitm * algorhitm)
{
    d->teacherMode = teacherMode;
    static Statement * begin = new Statement(LexemType(0xFFFFFFFF));
    d->source.clear();
    d->source << begin;
    foreach (Statement * st, statements) {
        if (st->type!=LxTypeComment)
            d->source << st;
    }
    d->ast = ast;
    d->algorhitm = algorhitm;
    if (!algorhitm) {
        QList<AST::Module*>::iterator it = d->ast->modules.begin();
        AST::ModuleType moduleToRemove = teacherMode? AST::ModTypeHidden : AST::ModTypeUser;
        while (it!=d->ast->modules.end()) {
            if ( (*it)->header.type == moduleToRemove ) {
                it = d->ast->modules.erase(it);
            }
            else {
                it++;
            }
        }
    }
    else {
        algorhitm->impl.pre.clear();
        algorhitm->impl.post.clear();
        algorhitm->impl.body.clear();
        algorhitm->impl.locals.clear();
        algorhitm->header.arguments.clear();
    }

    d->errorsCount = 0;
    for (int i=0; i<statements.size(); i++) {
        statements[i]->indentRank = QPoint(0,0);
    }
    d->currentPosition = 0;
    d->stack.clear();
    d->clearDataHistory();
    PDStackElem start;
    start.nonTerminal = "START";
    start.iterateStart = 0;
    start.priority = 0;
    d->stack.push(start);
    d->scripts = QVector< ScriptListPtr > (d->source.size()+2, NULL);
    d->fixedScripts = QVector< ScriptListPtr > (d->source.size()+2, NULL);
    d->nextPointers = QVector<int>(d->source.size());
    for (int i=0; i<d->nextPointers.size(); i++) {
        d->nextPointers[i] = i+1;
    }
    d->allowSkipParts = d->source.size() >= MAXIMUM_ERRORS_COUNT;
}

void PDAutomataPrivate::matchScript(const QString &text, ScriptListPtr & scripts)
{
    scripts = new QList<Script>;
    if (text.isEmpty()) {
        return;
    }
    QStringList statements = text.split(";");
    foreach (QString s, statements) {
        if (s.trimmed().isEmpty())
            continue;
        Script script;
        s = s.simplified();
        script.source = s;
        int op_br = s.indexOf("(");
        int cl_br = s.lastIndexOf(")");
        Q_ASSERT(op_br!=-1);
        Q_ASSERT(cl_br!=-1);
        const QString name = s.left(op_br).trimmed();
        const QString argline = s.mid(op_br+1, cl_br-op_br-1);
        QStringList args;
        if (!argline.isEmpty())
            args = argline.split(",");
        bool found = false;
        for (int i=0; i<metaObject()->methodCount(); i++) {
            QMetaMethod m = metaObject()->method(i);
            QString m_signature = m.signature();
            int spacePos = m_signature.indexOf(" ");
            int brPos = m_signature.indexOf("(");
            QString m_name = m_signature.mid(spacePos+1, brPos);
            if (name==m_name) {
                found = true;
                script.method = m;
                Q_ASSERT(args.size()==m.parameterTypes().size());
                break;
            }
        }
        Q_ASSERT(found);
        for (int i=0; i<args.size(); i++) {
            QString arg = args[i];
            if (arg.startsWith("\"")) {
                arg.remove("\"");
                script.arguments << QVariant(arg);
            }
            else {
                script.arguments << QVariant(arg.toInt());
            }
        }
        scripts->append(script);
    }
}

void PDAutomataPrivate::loadRules(const QString &rulesRoot)
{
    QDir rulesDir(rulesRoot);
    QStringList rulesFileList = rulesDir.entryList(QStringList() << "*.rules");
    QString normalizedRules;
    QList<int> priorities;
    for (int i=0; i<rulesFileList.count(); i++) {
        QString absName = rulesDir.absoluteFilePath(rulesFileList[i]);
        rulesFileList[i] = absName;
        priorities << i;
    }
    prepareRules(rulesFileList, normalizedRules, priorities);

    QStringList lines = normalizedRules.split("\n");

    QString leftPart;
    QString rightPart;
    QString script;
    QString terminal;

    int lineNo = 0;
    maxPriorityValue = 0;
    foreach ( QString line, lines ) {
        lineNo ++;
        // Обработка комментария
        if ( line.trimmed().startsWith("#") || line.trimmed().isEmpty() )
            continue;

        // Выделение приоритета
        QString prior_s;

        int bracket_p = line.lastIndexOf("[");
        if ( bracket_p == -1 ) {
            return; // ошибка: нет приоритета
        }
        else {
            prior_s = line.mid(bracket_p);
            line = line.left(bracket_p).trimmed();
        }

        prior_s = prior_s.remove("[").remove("]").trimmed();
        bool ok;
        int prior = prior_s.toInt(&ok);
        if ( ! ok )
            return; // ошибка: приоритет -- не целое число
        // Выделение левой части
        int arrow_p = line.indexOf("->");
        if ( arrow_p == -1 )
            return; // ошибка: неправильный формат правила
        leftPart = line.left(arrow_p).trimmed();
        if ( leftPart.isEmpty() ) {
            return; // ошибка: пустая левая часть
        }
        // Выделение правой части и скрипта
        rightPart = line.mid(arrow_p+2).trimmed();
        int script_p = rightPart.indexOf(":");
        if ( script_p != -1 ) {
            script = rightPart.mid(script_p+1).trimmed();
            rightPart = rightPart.left(script_p).trimmed();
        }
        else {
            script = ""; // очистка, т.к. могло что-то остаться от пред. строки
        }
        // Выделение терминала в правой части
        int space_p = rightPart.indexOf(' ');
        if ( space_p == -1 ) {
            if ( rightPart.startsWith("0") ) {
                // Правило \epsilon
                terminal = "";
                rightPart = "";
            }
            else {
                bool isCorrect = true;
                for ( int j=0; j<terminal.length(); j++ ) {
                    if (!( terminal[j]=='_' || ( terminal[j].isLetter() && terminal[j].isLower() ) )) {
                        isCorrect = false;
                        break;
                    }
                }
                if ( isCorrect ) {
                    terminal = rightPart;
                    rightPart = "";
                }
                else
                    return ; // ошибка: всего один ксимвол в правой части (не \epsilon и не терминал)
            }
        }
        else {
            terminal = rightPart.left(space_p);
            bool isCorrect = true;
            for ( int j=0; j<terminal.length(); j++ ) {
                if (!( terminal[j]=='_' || ( terminal[j].isLetter() && terminal[j].isLower() ) )) {
                    isCorrect = false;
                    break;
                }
            }
            if ( !isCorrect )
                return ; // ошибка: первый ксимвол в правой части -- нетерминал
            rightPart = rightPart.mid(space_p+1).trimmed();
        }
        maxPriorityValue = qMax(maxPriorityValue, prior);

        // Добавляем в таблицу правил
        if ( terminal.isEmpty() ) {
            LexemType allLines[] = {
                LxTypeEmpty,
                LxTypeDoc,
                LxNameClass,
                LxPriModule,
                LxPriEndModule,
                LxPriAlgHeader,
                LxPriAlgBegin,
                LxPriAlgEnd,
                LxPriPre,
                LxPriPost,
                LxPriIf,
                LxPriThen,
                LxPriElse,
                LxPriFi,
                LxPriSwitch,
                LxPriCase,
                LxPriLoop,
                LxPriEndLoop,
                LxPriInput,
                LxPriOutput,
                LxPriFinput,
                LxPriFoutput,
                LxPriAssign,
                LxPriAssert,
                LxPriImport,
                LxPriExit,
                LxPriPause,
                LxPriExit,
                LxPriAssignFile,
                LexemType(0xFFFFFFFF)
            };

            int j = 0;
            do {
                QString key;
                RuleRightPart rule;
                if ( allLines[j] != 0 )
                    key = terminalByCode(allLines[j]) + "/" + leftPart;
                else
                    key = "end/" + leftPart;
                Q_ASSERT ( ! key.startsWith("/") );
                rule.nonTerminals = QStringList();
                rule.isEpsilon = true;
                rule.priority = prior;
                matchScript(script.mid(1, script.length()-2), rule.script);
                if ( matrix.contains(key) ) {
                    // Добавляем \epsilon-правило только если нет других правил
                    // c тем же приоритетом
                    bool allowToAdd = true;
                    Rules rulesList = matrix[key];
                    foreach ( RuleRightPart rule, rulesList ) {
                        if ( rule.priority == prior ) {
                            allowToAdd = false;
                            break;
                        }
                    }
                    if ( allowToAdd )
                        matrix[key].append(rule);
                }
                else {
                    Rules newRulesList;
                    newRulesList.append(rule);
                    matrix[key] = newRulesList;
                }
                j++;
            } while ( allLines[j] != 0xFFFFFFFF );
        }
        else {
            // Случай обычного правила
            QString key;
            Q_ASSERT( isCorrectTerminal(terminal) );
            key = terminal + "/" + leftPart;
            RuleRightPart rule;
            rule.nonTerminals = rightPart.isEmpty()? QStringList() : rightPart.split(" ");
            rule.isEpsilon = false;
            rule.priority = prior;
            matchScript(script.mid(1, script.length()-2), rule.script);
            if ( matrix.contains(key) ) {
                matrix[key].append(rule);
                // Удаляем \epsilon-правило, если оно там есть
                // и имеет тот же приоритет
                for ( int j=matrix[key].count()-1; j>=0; j-- ) {
                    if ( matrix[key][j].isEpsilon && matrix[key][j].priority==prior )
                        matrix[key].removeAt(j);
                }
            }
            else {
                Rules newRulesList;
                newRulesList.append(rule);
                matrix[key] = newRulesList;
            }
        }
    }
//    qDebug() << "fff";
    foreach ( QString key, matrix.keys() ) {
        Rules rulesList = matrix[key];
        qSort(rulesList);
        matrix[key] = rulesList;
    }
//    qDebug() << "End load rules";
}

int PDAutomata::process()
{
    if ( d->stack.isEmpty() ) {
        if ( d->currentPosition == d->source.count() )
            return 0;
        else
            return 1;
    }
    do {
        if (d->errorsCount>MAXIMUM_ERRORS_COUNT) {
            d->setTooManyErrors();
            return 0;
        }
        PDStackElem currentStackElem = d->stack.pop();
        QString currentTerminal;
        QString key;
        if ( d->currentPosition > d->source.count() )
            break;
        if ( d->currentPosition < d->source.count() && d->currentPosition >= 0 ) {
            const Statement * st = d->source[d->currentPosition];
            currentTerminal = terminalByCode(st->type);
        }
        else {
            currentTerminal = "end";
        }
        key = currentTerminal + "/" + currentStackElem.nonTerminal;
        if ( d->matrix.contains(key) ) {
            Rules rulesList = d->matrix[key];

            if ( rulesList.count() == 1 ) {
                // Линейный случай, когда паре {ТЕРМИНАЛ,НЕТЕРМИНАЛ}
                // соответствует только одно правило
                RuleRightPart rule = rulesList[0];

                if ( !rule.isEpsilon && d->currentPosition>=0 )
                    d->scripts[d->currentPosition] = rule.script;
                // Если левая часть правила итеративная (*), то помещаем её обратно в стек
                if ( !rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") ) {
                    PDStackElem backElem = currentStackElem;
                    backElem.priority = rule.priority;
                    d->stack.push(backElem);
                }
                PDStackElem cse;
                for ( int j=rule.nonTerminals.count()-1; j>=0; j-- ) {
                    cse.nonTerminal = rule.nonTerminals[j];
                    cse.iterateStart = d->currentPosition;
                    cse.priority = rule.priority;
                    d->stack.push(cse);
                }
                // Если отрабатываем правило вида: ИТЕРАТИВНЫЙ_НЕТЕРМИНАЛ* -> 0,
                // то устанавливаем соответствующий next-указатель
                if ( rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") && currentTerminal!="end")
                    d->finalizeIterativeRule(currentStackElem);
                if ( !rule.isEpsilon && !key.startsWith("end/") )
                    d->nextStep();
            }
            else {
                // Разветвление. Сохраняем состояние и пытаемся применять
                // правила до тех пор, пока одно из них не будет успешным
                uint success = 0;

                // 				int errorsBefore = m_errorWayCounter;
                for ( int i=0; i<rulesList.count(); i++ )  {
                    d->saveData();
                    RuleRightPart rule = rulesList[i];

                    if ( !rule.isEpsilon && d->currentPosition>=0 )
                        d->scripts[d->currentPosition] = rule.script;
                    // Если левая часть правила итеративная (*), то помещаем её обратно в стек
                    if ( !rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") ) {
                        PDStackElem backElem = currentStackElem;
                        backElem.priority = rule.priority;
                        d->stack.push(backElem);
                    }
                    PDStackElem cse;
                    for ( int j=rule.nonTerminals.count()-1; j>=0; j-- ) {
                        cse.nonTerminal = rule.nonTerminals[j];
                        cse.iterateStart = d->currentPosition;
                        cse.priority = rule.priority;
                        d->stack.push(cse);
                    }
                    // Если отрабатываем правило вида: ИТЕРАТИВНЫЙ_НЕТЕРМИНАЛ* -> 0,
                    // то устанавливаем соответствующий next-указатель
                    if ( rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") && currentTerminal!="end")
                        d->finalizeIterativeRule(currentStackElem);
                    if ( !rule.isEpsilon && !key.startsWith("end/") )
                        d->nextStep();
                    success = process();
                    if ( success == 0 ) {
                        d->popHistory();
                        break;
                    }
                    if ( success == 1 ) {
                        d->restoreData();
                    }
                }
                if ( success != 0 ) {
                    break;
                }
            }
        }
        else {
            return 1;
        }
    } while ( !d->stack.isEmpty() );
    bool hasEnd = (d->currentPosition) == (d->source.count());

    return ( d->stack.isEmpty() && hasEnd ) ? 0 : 1;
}



void prepareRules(const QStringList &files, QString &out, const QList<int> &priors)
{
    int iskNet = 0;

    if (files.isEmpty())
    {
        qDebug()<<QString::fromUtf8("Нет .rules-файлов");
        return;
    }

    QBuffer filIn1;
    filIn1.open(QIODevice::WriteOnly);

    QTextStream sIn1(&filIn1);
    sIn1.setCodec("UTF-8");


    int fileNum = -1;
    foreach ( QString ifn, files ) {

        ++fileNum;
        QFile filIn(ifn);

        filIn.open(QIODevice::ReadOnly);

        QTextStream sIn(&filIn);
        sIn.setCodec("UTF-8");


        for( ; !sIn.atEnd() ; )
        {
            QString str(sIn.readLine().trimmed());
            int openFig = 0;
            int i = 0;
            do
            {
                for( ; i<str.length(); ++i)
                {
                    if (str.at(i)=='{') ++openFig;
                    if (str.at(i)=='}') --openFig;
                }
                if (openFig==0)
                {
                    if (!str.startsWith("#") && !str.isEmpty())
                    {
                        sIn1 << str << " [" << priors[fileNum] << "]\n";
                    }
                    break;
                }
                else
                {
                    if (!sIn.atEnd())
                    {
                        str += sIn.readLine();
                    }
                    else
                    {
                        sIn1 << "Syntax error in {}:\n";
                        sIn1 << str << endl;
                        return;
                    }
                }
            } while (true);
        }
        filIn.close();
    }

    filIn1.close();

    QBuffer filIn;
    filIn.setData(filIn1.data());
    filIn.open(QIODevice::ReadOnly);
    QTextStream sIn(&filIn);
    sIn.setCodec("UTF-8");


    QStringList pravila1;
    for( ; !sIn.atEnd() ; )
    {
        QString str(sIn.readLine().trimmed());
        int i = str.indexOf('#');
        if (i!=-1)
        {
            str.truncate(i);
        }
        str.trimmed();
        if (!str.isEmpty())
        {
            pravila1 << str;
        }
    }

    pravila1.sort();


    QStringList nagruzki1;
    QList<int> priorities;
    for(int i=0; i<pravila1.size(); ++i)
    {
        QString str = pravila1[i];
        int d = str.indexOf(':');
        int d1 = str.lastIndexOf(':');
        if (d!=d1)
        {
            qDebug()<<QString::fromUtf8("Два двоеточия в строке: ")<<str<<endl;
            return;
        }
        int e = str.lastIndexOf('[');
        if (e==-1)
        {
            qDebug()<<QString::fromUtf8("Не задан приоритет строки: ")<<str<<endl;
            return;
        }
        if (d!=-1)
        {
            nagruzki1 << str.mid(d+1, e-d-1);
            pravila1[i].truncate(d);
        }
        else
        {
            nagruzki1 << QString();
            pravila1[i].truncate(e);
        }
        QString p(str.mid(e).trimmed());
        if (!p.startsWith("[") && !p.endsWith("]"))
        {
            qDebug()<<QString::fromUtf8("Ошибка задания приоритета в строке: ")<<str<<endl;
            return;
        }
        priorities << p.mid(1,p.length()-2).toInt();
    }

    QList<QStringList> pravila;
    foreach(QString s, pravila1)
    {
        pravila << s.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (!isNeterminal(pravila.last()[0]) || pravila.last()[1]!="->" || pravila.last().count()<3)
        {
            qDebug()<<QString::fromUtf8("Ошибка в левой части: ")<<pravila.last().join(" ")<<endl;
            return;
        }
    }

    for(int i=0; i<nagruzki1.size(); ++i)
    {
        if (nagruzki1[i].isEmpty())
        {
            for(int j=0; j<pravila[i].size(); ++j)
            {
                if (isTerminal(pravila[i][j]))
                    nagruzki1[i] += "{}";
            }
        }
    }

    QList<QStringList> nagruzki;
    int ind = 0;
    foreach(QString s, nagruzki1)
    {
        QStringList rez;
        int countNagr = 0;
        int i, j;
        for(i=0; i<s.length()-1; )
        {
            j = i+1;
            int openFig = 0;
            if (s.at(i)=='{')
            {
                openFig = 1;
                for( ; j<s.length(); ++j)
                {
                    if (s.at(j)=='{') ++openFig;
                    if (s.at(j)=='}') --openFig;
                    if (openFig==0) break;
                }
                rez << s.mid(i,j-i+1);
                ++countNagr;
            }
            i=j;
        }
        int countTerm = 0;
        foreach(QString str, pravila[ind])
        {
            if (isTerminal(str))
                ++countTerm;
        }
        if (countTerm != countNagr)
        {
            qDebug() << QString::fromUtf8("Не совпадает число терминалов и нагрузок: ") << pravila[ind].join(" ") << " : " << s;
            return;
        }
        nagruzki << rez;
        ++ind;
    }

    int c;


    QStringList sorts;

    for(bool flag = true ; flag ;)
    {
        QString s;
        for(int i=0; i<pravila.size(); ++i)
        {
            s += pravila[i][0];
            s += " ";
            s += pravila[i][2];
            s += " ; ";
        }
        if (sorts.indexOf(s)!=-1)
        {
            qDebug() << QString::fromUtf8("Данная грамматика является леворекурсивной\n");
            return;
        }
        else
        {
            sorts << s;
        }
        flag = false;
        for(int i=1; i<pravila.size(); ++i)
        {
            for(int j=0; j<i; ++j)
            {
                if (isNeterminal(pravila[i][2]) && pravila[i][2]==pravila[j][0])
                {
                    QString s1(pravila[j][0]);
                    QString s2(pravila[i][0]);

                    int k1 = 0, k2 = 0;
                    for(int k = pravila.size()-1; k>=0; --k)
                    {
                        if (pravila[k][0]==s1)
                            k1 = k;
                        if (pravila[k][0]==s2)
                            k2 = k;
                    }
                    for(int shift = 0 ; pravila[k2+shift][0]==s2 ; ++shift)
                    {
                        pravila.move(k2+shift,k1+shift);
                        nagruzki.move(k2+shift,k1+shift);
                        priorities.move(k2+shift,k1+shift);
                    }
                    flag = true;
                }
            }
        }
    }



    for(int i=pravila.size()-1; i>=0; --i)
    {
        QList<QStringList> add;
        QList<QStringList> addNagr;
        QList<int> addPriors;
        for(int j=i+1; j<pravila.size(); ++j)
        {
            if (pravila[j][0]==pravila[i][2] && (isTerminal(pravila[j][2]) || isNeterminal(pravila[j][2])))
            {
                QStringList sl;
                sl << pravila[i][0] << "->" << pravila[j].mid(2, pravila[j].size()-2) << pravila[i].mid(3, pravila[i].size()-3);
                add << sl;
                sl.clear();
                sl << nagruzki[j] << nagruzki[i];
                addNagr << sl;
                int a = priorities[i];
                int b = priorities[j];
                addPriors << (a>b ? a:b);
            }
        }
        for(int j=0; j<add.size(); ++j)
        {
            pravila.insert(i+j+1, add[j]);
            nagruzki.insert(i+j+1, addNagr[j]);
            priorities.insert(i+j+1, addPriors[j]);
        }
        if (add.size()>0)
        {
            pravila.removeAt(i);
            nagruzki.removeAt(i);
            priorities.removeAt(i);
        }
    }


    for(int i=pravila.size()-1; i>=0; --i)
    {
        QList<int> nirps;
        for(int j=2; j<pravila[i].size(); ++j)
            if (isTerminal(pravila[i][j]))
                nirps << j;
        if (nirps.size()>1)
        {
            nirps << pravila[i].size();
            QList<QStringList> add;
            QList<QStringList> addNagr;
            QList<int> addPriors;
            bool flag = false;
            for(int j=0; j<nirps.size()-1; ++j)
            {
                QStringList sl;
                QStringList slNagr;
                if (!flag)
                {
                    sl << pravila[i][0];
                    flag = true;
                }
                else
                    sl << QString::number(iskNet);
                sl << "->" << pravila[i].mid(nirps[j], nirps[j+1]-nirps[j]);
                slNagr << nagruzki[i][j];
                if (j<nirps.size()-2)
                {
                    ++iskNet;
                    sl << QString::number(iskNet);
                }
                add << sl;
                addNagr << slNagr;
                addPriors << priorities[i];
            }
            for(int j=0; j<add.size(); ++j)
            {
                pravila.insert(i+j+1, add[j]);
                nagruzki.insert(i+j+1, addNagr[j]);
                priorities.insert(i+j+1, addPriors[j]);
            }
            if (add.size()>0)
            {
                pravila.removeAt(i);
                nagruzki.removeAt(i);
                priorities.removeAt(i);
            }
        }
    }


    for(c=0; c<pravila.size(); ++c)
    {
        QStringList sl(pravila[c]);
        QString pravilo( sl.join(" ") );
        QString nagruzka( nagruzki[c].join(" ") );
        int priority = priorities[c];
        for(int j=c+1; j<pravila.size(); )
        {
            QString pravilo1( pravila[j].join(" ") );
            QString nagruzka1( nagruzki[j].join(" ") );
            int priority1 = priorities[j];
            if (pravilo==pravilo1)
            {
                if (nagruzka==nagruzka1 && priority==priority1)
                {
                    pravila.removeAt(j);
                    nagruzki.removeAt(j);
                    priorities.removeAt(j);
                }
                else
                {
//                    qDebug() << "Dva pravila s raznoj nagruzkoj ili raznymi prioritetami:";
//                    qDebug() << pravilo << " : " << nagruzka << " [" << priority << "]";
//                    qDebug() << pravilo1 << " : " << nagruzka1 << " [" << priority1 << "]";
                    ++j;
                }
            }
            else
            {
                ++j;
            }
        }
    }

    c = 0;
    foreach(QStringList sl, pravila)
    {
        foreach(QString s, sl)
        {
            out += s + " ";
        }
        if (!nagruzki[c].isEmpty())
        {
            out += ":";
        }
        foreach(QString s, nagruzki[c])
        {
            out += " " + s;
        }
        out += QString(" [") + QString::number(priorities[c]) + "]\n";
        ++c;
    }
    out += "\n";
}

QString terminalByCode(int code)
{
    switch ( code ) {
    case LxNameClass:
        return QString::fromUtf8("простое_предложение");
    case LxPriAlgHeader:
        return QString::fromUtf8("алг");
    case LxPriAlgBegin:
        return QString::fromUtf8("нач");
    case LxPriAlgEnd:
        return QString::fromUtf8("кон");
    case LxPriLoop:
        return QString::fromUtf8("нц");
    case LxPriEndLoop:
        return QString::fromUtf8("кц");
    case LxPriElse:
        return QString::fromUtf8("иначе");
    case LxPriExit:
        return QString::fromUtf8("простое_предложение");
    case LxPriIf:
        return QString::fromUtf8("если");
    case LxPriThen:
        return QString::fromUtf8("то");
    case LxPriFi:
        return QString::fromUtf8("все");
    case LxPriSwitch:
        return QString::fromUtf8("выбор");
    case LxPriCase:
        return QString::fromUtf8("при");
    case LxPriAssign:
        return QString::fromUtf8("простое_предложение");
    case LxPriAssignFile:
        return QString::fromUtf8("простое_предложение");
    case LxPriOutput:
        return QString::fromUtf8("простое_предложение");
    case LxPriAssert:
        return QString::fromUtf8("простое_предложение");
    case LxPriPre:
        return QString::fromUtf8("ограничение_алгоритма");
    case LxPriPost:
        return QString::fromUtf8("ограничение_алгоритма");
    case LxPriInput:
        return QString::fromUtf8("простое_предложение");
    case LxPriModule:
        return QString::fromUtf8("исп");
    case LxPriEndModule:
        return QString::fromUtf8("кон_исп");
    case LxPriImport:
        return QString::fromUtf8("использовать");
    case LxPriFinput:
        return QString::fromUtf8("простое_предложение");
    case LxPriFoutput:
        return QString::fromUtf8("простое_предложение");
    case LxPriPause:
        return QString::fromUtf8("простое_предложение");
    case LxPriHalt:
        return QString::fromUtf8("простое_предложение");
    case LxTypeDoc:
        return QString::fromUtf8("строка_документации");
    case LexemType(0xFFFFFFFF):
        return "begin";
    default:
        return QString(" ");
    }
}

bool isCorrectTerminal(const QString &terminal)
{
    if (
            terminal == QString::fromUtf8("простое_предложение")
            ||
            terminal == QString::fromUtf8("алг")
            ||
            terminal == QString::fromUtf8("нач")
            ||
            terminal == QString::fromUtf8("кон")
            ||
            terminal == QString::fromUtf8("нц")
            ||
            terminal == QString::fromUtf8("кц")
            ||
            terminal == QString::fromUtf8("иначе")
            ||
            terminal == QString::fromUtf8("если")
            ||
            terminal == QString::fromUtf8("то")
            ||
            terminal == QString::fromUtf8("все")
            ||
            terminal == QString::fromUtf8("выбор")
            ||
            terminal == QString::fromUtf8("при")
            ||
            terminal == QString::fromUtf8("ограничение_алгоритма")
            ||
            terminal == QString::fromUtf8("исп")
            ||
            terminal == QString::fromUtf8("использовать")
            ||
            terminal == QString::fromUtf8("кон_исп")
            ||
            terminal == QString::fromUtf8("строка_документации")
            ||
            terminal == "end"
            ||
            terminal == "begin"
            ) return true;
    else
        return false;
}

void PDAutomataPrivate::finalizeIterativeRule(const PDStackElem & stackElem)
{
    int iterationStartPos = stackElem.iterateStart;
    for ( int i=iterationStartPos+1; i<currentPosition; i++ ) {
        if ( fixedScripts[i]==NULL )
            fixedScripts[i] = scripts[i];
    }
    if ( allowSkipParts || stackElem.priority==0 )
        nextPointers[iterationStartPos] = currentPosition;
}

void PDAutomataPrivate::saveData()
{
    history_stack.push(stack);
    history_currentPosition.push(currentPosition);
    history_scripts.push(scripts);
}

void PDAutomataPrivate::restoreData()
{
    currentPosition = history_currentPosition.pop();
    stack = history_stack.pop();
    scripts = history_scripts.pop();
}

void PDAutomataPrivate::popHistory()
{
    history_currentPosition.pop();
    history_stack.pop();
    history_scripts.pop();
}

void PDAutomataPrivate::clearDataHistory()
{
    history_currentPosition.clear();
    history_stack.clear();
    history_scripts.clear();
}

void PDAutomataPrivate::nextStep()
{
    currentPosition = currentPosition<nextPointers.size()
            ? nextPointers[currentPosition]
            : source.size();
}

void PDAutomata::postProcess()
{
    d->currentContext.clear();
    d->currentPosition = 0;
    if (!d->algorhitm) {
        d->currentModule = new AST::Module();
        if (d->teacherMode) {
            d->currentModule->header.type = AST::ModTypeHidden;
            d->currentModule->header.name = "@";
        }
        d->currentAlgorhitm = 0;
        d->currentContext.push(&(d->currentModule->impl.initializerBody));
    }
    else {
        AST::Module * mod = 0;
        for (int i=0; i<d->ast->modules.size(); i++) {
            for (int j=0; j<d->ast->modules[i]->impl.algorhitms.size(); j++) {
                if (d->algorhitm==d->ast->modules[i]->impl.algorhitms[j]) {
                    mod = d->ast->modules[i];
                    break;
                }
            }
            if (mod)
                break;
        }
        d->currentModule = mod;
//        d->currentContext.push(&(d->algorhitm->impl.body));
        d->currentAlgorhitm = d->algorhitm;
    }

    for (int i=0; i<d->scripts.size(); i++) {
        ScriptListPtr scripts = d->scripts[i];
        if (!scripts) {
            scripts = d->fixedScripts[i];
        }
        if (scripts) {
            for (int j=0; j<scripts->size(); j++) {
                QMetaMethod m = scripts->at(j).method;
                const QString sign = m.signature();
                Q_UNUSED(sign); // Used only for debug
                const QList<QVariant> & arguments = scripts->at(j).arguments;
                switch (arguments.size()) {
                case 1:
                    if (QString::fromAscii(m.parameterTypes()[0])=="int") {
                        m.invoke(d, Qt::DirectConnection,
                                 Q_ARG(int, arguments[0].toInt())
                                 );
                    }
                    else {
                        m.invoke(d, Qt::DirectConnection,
                                 Q_ARG(QString, arguments[0].toString())
                                 );
                    }
                    break;
                case 2:
                    m.invoke(d, Qt::DirectConnection,
                             Q_ARG(int, arguments[0].toInt()),
                             Q_ARG(int, arguments[0].toInt())
                             );
                    break;
                default:
                    m.invoke(d, Qt::DirectConnection);
                }
            }
        }
        d->currentPosition ++;
    }
    if (d->currentModule && !d->algorhitm) {
        d->ast->modules << d->currentModule;
    }

    d->source.pop_front();

}

void PDAutomataPrivate::setCurrentIndentRank(int start, int end)
{
    source[currentPosition]->indentRank = QPoint(start, end);
}

void PDAutomataPrivate::setCurrentError(const QString &value)
{
    for (int i=0; i<source[currentPosition]->data.size(); i++) {
        source[currentPosition]->data[i]->error = value;
        source[currentPosition]->data[i]->errorStage = AST::Lexem::PDAutomata;
    }
}

void PDAutomataPrivate::setOutOfAlgError()
{
    QString value;
    if (source[currentPosition]->type & LxNameClass) {
        value = _("Variable declaration out of algorhitm");
    }
    else {
        value = _("Instruction out of algorhitm");
    }
    for (int i=0; i<source[currentPosition]->data.size(); i++) {
        source[currentPosition]->data[i]->error = value;
        source[currentPosition]->data[i]->errorStage = AST::Lexem::PDAutomata;
    }

    bool doNotEvaluate = false;

    if (currentModule)
        doNotEvaluate = currentModule->impl.algorhitms.size()>0;

    appendSimpleLine();
    if (!currentContext.isEmpty() && currentContext.top())
        currentContext.top()->last()->skipErrorEvaluation = doNotEvaluate;
}

void PDAutomataPrivate::processCorrectAlgHeader()
{
    AST::Algorhitm * alg = algorhitm? algorhitm : new AST::Algorhitm;
    if (teacherMode)
        alg->header.specialType = AST::AlgorhitmTypeTeacher;
    alg->impl.headerLexems = source[currentPosition]->data;
    currentAlgorhitm = alg;
    if (algorhitm) {
        source.at(currentPosition)->mod = moduleOfAlgorhitm(ast, algorhitm);
        currentModule = source.at(currentPosition)->mod;
    }
    else {
        if (currentModule==0) {

        }
        else {
            currentModule->impl.algorhitms << alg;
            source.at(currentPosition)->mod = currentModule;
        }
    }
    source.at(currentPosition)->alg = alg;
}

void PDAutomataPrivate::processCorrectAlgBegin()
{
    setCurrentIndentRank(  0, +1);
    if (currentAlgorhitm) {
        currentAlgorhitm->impl.beginLexems = source.at(currentPosition)->data;
        source.at(currentPosition)->mod = currentModule;
        source.at(currentPosition)->alg = currentAlgorhitm;
        currentContext.push(&(currentAlgorhitm->impl.body));
    }
}

void PDAutomataPrivate::appendSimpleLine()
{
    AST::Statement * statement = new AST::Statement;
    statement->skipErrorEvaluation = false;
    statement->lexems = source.at(currentPosition)->data;
    switch ( source.at(currentPosition)->type ) {
    case LxPriAssign:
        statement->type = AST::StAssign;
        break;
    case LxPriAssert:
        statement->type = AST::StAssert;
        break;
    case LxNameClass: {
            // Check for we are in algorhitm top or module top
            if (currentContext.size()>1) {
                QList<AST::Statement*> * body = currentContext[currentContext.size()-2];
                if (!body->isEmpty()) {
                    AST::Statement * st = body->last();
                    if (st->type==AST::StLoop
                            || st->type==AST::StIfThenElse
                            || st->type==AST::StSwitchCaseElse)
                    {
                        foreach (Lexem *lx, statement->lexems) {
                            lx->error = _("Can't declare variables at this level");
                            lx->errorStage = AST::Lexem::PDAutomata;
                        }
                    }
                }
            }
            statement->type = AST::StVarInitialize;
            break;
    }
    case LxPriInput:
        statement->type = AST::StInput;
        break;
    case LxPriOutput:
        statement->type = AST::StOutput;
        break;
    case LxPriFinput:
        statement->type = AST::StFileInput;
        break;
    case LxPriFoutput:
        statement->type = AST::StFileOutput;
        break;
    case LxPriAssignFile:
        statement->type = AST::StAssignFileStream;
        break;
    case LxPriExit:
        statement->type = AST::StBreak;
        break;
    case LxPriPause:
        statement->type = AST::StPause;
        break;
    case LxPriHalt:
        statement->type = AST::StHalt;
        break;
    default:
        statement->type = AST::StError;
        break;
    }
    if (teacherMode) {
        if (currentContext.size()<=1) {
            // Can't do anything out of algorhitms
            foreach (Lexem * lx, statement->lexems) {
                lx->error = _("Hidden part must contain only algorithm");
                lx->errorStage = AST::Lexem::PDAutomata;
            }

            statement->type = AST::StError;
        }
    }
    if ( source.at(currentPosition)->data[0]->error.size()>0 ) {
        statement->type = AST::StError;
        statement->error = source.at(currentPosition)->data[0]->error;
    }
    if (!currentContext.isEmpty() && currentContext.top()) {
        if (statement->type==AST::StError) {
            if (currentContext.top()==&(currentModule->impl.initializerBody)) {
                if (currentModule->impl.algorhitms.size()>0) {
                    statement->skipErrorEvaluation = true;
                }
            }
        }
        currentContext.top()->append(statement);
    }
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = statement;
}

AST::Statement * PDAutomata::createSimpleAstStatement(Statement * st)
{
    AST::Statement * statement = new AST::Statement;
    statement->skipErrorEvaluation = false;
    switch ( st->type ) {
    case LxPriAssign:
        statement->type = AST::StAssign;
        break;
    case LxPriAssert:
        statement->type = AST::StAssert;
        break;
    case LxPriInput:
        statement->type = AST::StInput;
        break;
    case LxPriOutput:
        statement->type = AST::StOutput;
        break;
    case LxPriFinput:
        statement->type = AST::StFileInput;
        break;
    case LxPriFoutput:
        statement->type = AST::StFileOutput;
        break;
    case LxPriExit:
        statement->type = AST::StBreak;
        break;
    default:
        statement->type = AST::StError;
        break;
    }
    if (st->hasError()) {
        statement->type = AST::StError;
        statement->error = st->data[0]->error;
        st->statement = statement;
    }
    return statement;
}

void PDAutomataPrivate::processCorrectAlgEnd()
{
    setCurrentIndentRank(-1,  0);
    if (currentAlgorhitm) {
        currentAlgorhitm->impl.endLexems = source.at(currentPosition)->data;
    }
    currentContext.pop();
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    currentAlgorhitm = 0;
}

void PDAutomataPrivate::addDummyAlgHeader()
{
    // nothind to do
}

void PDAutomataPrivate::setModuleBeginError(const QString & value)
{
    for (int i=0; i<source.size(); i++) {
        if (source[i]->mod == currentModule && source[i]->type==LxPriModule) {
            for (int a=0; a<source[i]->data.size(); a++) {
                source[i]->data[a]->error = value;
                source[i]->data[a]->errorStage = AST::Lexem::PDAutomata;
            }
            source[i]->indentRank = QPoint(0, 0);
        }
    }
}

void PDAutomataPrivate::processCorrectEndOfLoop()
{
    setCurrentIndentRank(-1,  0);
    currentContext.pop();
    Q_ASSERT(currentContext.size()>0);
    Q_ASSERT(currentContext.top()->last()->type==AST::StLoop);
    currentContext.top()->last()->loop.endLexems = source.at(currentPosition)->data;
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = currentContext.top()->last();
}

void PDAutomataPrivate::processAlgEndInsteadOfLoopEnd()
{
    setCurrentIndentRank(-1, 0);
    setCurrentError(_("'end' instead of 'endloop'"));
    currentContext.pop();
    Q_ASSERT(currentContext.size()>0);
    Q_ASSERT(currentContext.top()->last()->type==AST::StLoop);
    currentContext.top()->last()->loop.endLexems = source.at(currentPosition)->data;
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = currentContext.top()->last();
}

void PDAutomataPrivate::processModEndInsteadOfAlgEnd()
{
    int a = currentPosition-1;
    int modDeclPos = -1;
    int beginPos = -1;
    int algPos = -1;
    while (a>=0) {
        if (source[a]->type==LxPriEndModule && !source[a]->hasError()) {
            break;
        }
        else if (source[a]->type==LxPriAlgBegin) {
            beginPos = a;
        }
        else if (source[a]->type==LxPriAlgHeader) {
            algPos = a;
        }
        else if (source[a]->type==LxPriAlgEnd) {
            algPos = beginPos = -1;
            break;
        }
        else if (source[a]->type==LxPriEndModule && !source[a]->hasError()) {
            modDeclPos = -1;
            break;
        }
        else if (source[a]->type==LxPriModule && !source[a]->hasError()) {
            modDeclPos = a;
            break;
        }
        a--;
    }
    processCorrectAlgEnd();
    processCorrectModuleEnd();
    if (modDeclPos==-1) {
        setCurrentError(_("'end_module' instead of 'end'"));
        setCurrentIndentRank(-1, 0);
    }
    else {
        int pos = qMax(algPos, beginPos);
        if (pos!=-1) {
            const QString err = pos==beginPos
                    ? _("Extra 'begin'")
                    : _("Algorhitm not implemented");
            for (int i=0; i<source[pos]->data.size(); i++) {
                source[pos]->data[i]->error = err;
                source[pos]->data[i]->errorStage = AST::Lexem::PDAutomata;
            }
            setCurrentIndentRank(-2, 0);
        }
    }
}

void PDAutomataPrivate::processCorrectCase()
{
    setCurrentIndentRank(-1, +1);
    currentContext.pop();
    Q_ASSERT(currentContext.size()>0);
    Q_ASSERT(currentContext.top()->last()->type==AST::StSwitchCaseElse);
    AST::ConditionSpec cond;
    cond.lexems = source.at(currentPosition)->data;
    cond.condition = 0;
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = currentContext.top()->last();
    source.at(currentPosition)->conditionalIndex = currentContext.top()->last()->conditionals.size();
    currentContext.top()->last()->conditionals << cond;
    currentContext.push(&(currentContext.top()->last()->conditionals.last().body));
}

void PDAutomataPrivate::processCorrectIf()
{
    setCurrentIndentRank(0, +2);
    AST::Statement * st = new AST::Statement;
    st->skipErrorEvaluation = false;
    st->type = AST::StIfThenElse;
    st->lexems = source.at(currentPosition)->data;
    currentContext.top()->append(st);
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = currentContext.top()->last();
}

void PDAutomataPrivate::processCorrectThen()
{
    setCurrentIndentRank(-1, +1);
    Q_ASSERT(currentContext.size()>0);
    while (currentContext.top()->size()>0 && currentContext.top()->last()->type==AST::StError) {
        currentContext.top()->pop_back();
    }
    while (currentContext.top()->size()==0) {
        currentContext.pop_back();
    }
    Q_ASSERT(currentContext.top()->last()->type==AST::StIfThenElse);
    AST::ConditionSpec cond;
    cond.lexems = source.at(currentPosition)->data;
    cond.condition = 0;
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = currentContext.top()->last();
    source.at(currentPosition)->conditionalIndex = currentContext.top()->last()->conditionals.size();
    currentContext.top()->last()->conditionals << cond;
    currentContext.push(&(currentContext.top()->last()->conditionals.last().body));
}

void PDAutomataPrivate::processCorrectFi()
{
    currentContext.pop();

    if (currentContext.size()>0 && currentContext.last()->size()>0 && (currentContext.top()->last()->type==AST::StIfThenElse
                                            || currentContext.top()->last()->type==AST::StSwitchCaseElse))
    {
        if (currentContext.top()->last()->conditionals.size()>0) {
            setCurrentIndentRank(-2, 0);
        }
        else {
            setCurrentIndentRank(-2, 0);
//            setCurrentError(_("Extra 'fi'"));
        }
        source.at(currentPosition)->mod = currentModule;
        source.at(currentPosition)->alg = currentAlgorhitm;
        source.at(currentPosition)->statement = currentContext.top()->last();
    }
}

void PDAutomataPrivate::processCorrectElse()
{
    setCurrentIndentRank(-1, +1);
    currentContext.pop();
    Q_ASSERT(currentContext.size()>0);
    if (currentContext.top()->size()>0 && (currentContext.top()->last()->type==AST::StIfThenElse ||
            currentContext.top()->last()->type==AST::StSwitchCaseElse)
            )
    {
        AST::ConditionSpec cond;
        cond.lexems = source.at(currentPosition)->data;
        cond.condition = 0;
        source.at(currentPosition)->mod = currentModule;
        source.at(currentPosition)->alg = currentAlgorhitm;
        source.at(currentPosition)->statement = currentContext.top()->last();
        source.at(currentPosition)->conditionalIndex = currentContext.top()->last()->conditionals.size();
        currentContext.top()->last()->conditionals << cond;
        currentContext.push(&(currentContext.top()->last()->conditionals.last().body));
    }
    else {
        // Error: no if..then
        AST::Statement * ste = new AST::Statement();
        ste->skipErrorEvaluation = false;
        ste->type = AST::StError;
        ste->error = _("No then before else");
        ste->lexems = source.at(currentPosition)->data;
        foreach (Lexem * lx, source.at(currentPosition)->data) {
            lx->error = ste->error;
        }
        currentContext.top()->append(ste);
        AST::Statement * st = new AST::Statement;
        st->skipErrorEvaluation = false;
        st->type = AST::StIfThenElse;
        st->lexems = source.at(currentPosition)->data;
        source.at(currentPosition)->mod = currentModule;
        source.at(currentPosition)->alg = currentAlgorhitm;
        source.at(currentPosition)->statement = currentContext.top()->last();
        currentContext.top()->append(st);

        AST::ConditionSpec cond;
        cond.lexems = source.at(currentPosition)->data;
        cond.condition = 0;
        source.at(currentPosition)->mod = currentModule;
        source.at(currentPosition)->alg = currentAlgorhitm;
        source.at(currentPosition)->statement = currentContext.top()->last();
        source.at(currentPosition)->conditionalIndex = currentContext.top()->last()->conditionals.size();
        currentContext.top()->last()->conditionals << cond;
        currentContext.push(&(currentContext.top()->last()->conditionals.last().body));

    }
}

void PDAutomataPrivate::processCorrectSwitch()
{
    setCurrentIndentRank(0, +2);
    AST::Statement * st = new AST::Statement;
    st->skipErrorEvaluation = false;
    st->type = AST::StSwitchCaseElse;
    st->lexems = source.at(currentPosition)->data;
    currentContext.top()->append(st);
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = currentContext.top()->last();

    // Push dummy case statement into context for
    // correct processing of first 'case' statement

    currentContext.push(0);
}

void PDAutomataPrivate::processCorrectBeginOfLoop()
{
    setCurrentIndentRank(0, +1);
    AST::Statement * st = new AST::Statement;
    st->skipErrorEvaluation = false;
    st->type = AST::StLoop;
    st->lexems = source.at(currentPosition)->data;
    currentContext.top()->append(st);
    currentContext.push(&(currentContext.top()->last()->loop.body));
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = st;

}

void PDAutomataPrivate::processCorrectDocLine()
{
    // TODO implement me
}

void PDAutomataPrivate::processCorrectRestrictionLine()
{
    AST::Statement * st = new AST::Statement;
    st->skipErrorEvaluation = false;
    st->type = AST::StAssert;
    st->lexems = source.at(currentPosition)->data;
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->statement = st;
    if (currentAlgorhitm) {
        if ( source.at(currentPosition)->type==LxPriPre ) {
            if ( source.at(currentPosition)->data.size()>1 )
                currentAlgorhitm->impl.pre.append(st);
        }
        else {
            if ( source.at(currentPosition)->data.size()>1 )
                currentAlgorhitm->impl.post.append(st);
        }
    }
}

void PDAutomataPrivate::processCorrectModuleBegin()
{
    setCurrentIndentRank(0, +1);
    if (currentModule) {
        ast->modules << currentModule;
    }
    currentModule = new AST::Module;
    currentModule->header.type = teacherMode? AST::ModTypeHidden : AST::ModTypeUser;
    source.at(currentPosition)->mod = currentModule;
    currentContext.push(&(currentModule->impl.initializerBody));
}

void PDAutomataPrivate::processCorrectModuleEnd()
{
    setCurrentIndentRank(-1, 0);
    source.at(currentPosition)->mod = currentModule;
    if (currentModule && !algorhitm) {
        ast->modules << currentModule;
    }
    currentModule = 0;
}

void PDAutomataPrivate::processCorrectLoad()
{
    source[currentPosition]->mod = currentModule;
    source[currentPosition]->alg = 0;
}

void PDAutomataPrivate::setGarbageAlgError()
{
    static const QList<LexemType> OutgoingOperationalBrackets =
            QList<LexemType>() << LxPriImport << LxPriAlgHeader << LxPriModule << LxPriEndModule;
    if (OutgoingOperationalBrackets.contains(source[currentPosition]->type))
        setCurrentError(_("'%1' in algorithm", source[currentPosition]->data.first()->data));
    else
        setCurrentError(_("Garbage between alg..begin"));
    appendSimpleLine();
}

void PDAutomataPrivate::setGarbageIfThenError()
{
    setCurrentError(_("Garbage between if..then"));
//    appendSimpleLine();
    processCorrectThen();
}

void PDAutomataPrivate::setGarbageSwitchCaseError()
{
    setCurrentError(_("Garbage between switch..case"));
    appendSimpleLine();
}

void PDAutomataPrivate::setTooManyErrors()
{

}


void PDAutomataPrivate::setCorrespondingIfBroken()
{
    AST::Statement * st = 0;
    for (int i=currentContext[currentContext.size()-1]->size()-1; i>=0; i--) {
        if (currentContext[currentContext.size()-1]->at(i)->type==AST::StIfThenElse) {
            st = currentContext[currentContext.size()-1]->at(i);
            break;
        }
    }
    if (st) {
        st->type = AST::StError;
        st->error = _("Broken if statement");
        for (int i=0; i<source.size(); i++) {
            if ( source[i]->statement==st ) {
                for (int a=0; a<source[i]->data.size(); a++) {
                    source[i]->data[a]->error = _("Broken if statement");
                    source[i]->data[a]->errorStage = AST::Lexem::PDAutomata;
                }

                break;
            }
        }
    }
}

void PDAutomataPrivate::setExtraOpenKeywordError(const QString &kw)
{
    if (kw==QString::fromUtf8("если")) {
        setCurrentIndentRank(0,0);
        setCurrentError(_("Extra 'if'"));
    }
    else if (kw==QString::fromUtf8("то")) {
        setCurrentIndentRank(-1,-1);
        QString err = _("Extra 'then'");
        int a = currentPosition + 1;
        bool fiFound = false;
        while (a<source.size()) {
            if (source[a]->type==LxPriFi && !source[a]->hasError()) {
                fiFound = true;
                break;
            }
            else if (source[a]->type==LxPriEndModule || source[a]->type==LxPriAlgEnd)
                break;
            a++;
        }
        if (!fiFound)
            err = _("No 'end' after 'then'");
        setCurrentError(err);
    }
    else if (kw==QString::fromUtf8("выбор")) {
        setCurrentError(_("Extra 'switch'"));
    }
    else if (kw==QString::fromUtf8("при")) {
        setCurrentError(_("Extra 'case'"));
    }
    else if (kw==QString::fromUtf8("нц")) {
        setCurrentIndentRank(0,0);
        setCurrentError(_("Extra 'loop'"));
    }
    else if (kw==QString::fromUtf8("нач")) {
        setCurrentError(_("Extra 'begin'"));
    }
    else if (kw==QString::fromUtf8("иначе")) {
        setCurrentIndentRank(-1,-1);
        QString err = _("Extra 'else'");
        int a = currentPosition + 1;
        bool fiFound = false;
        while (a<source.size()) {
            if (source[a]->type==LxPriFi && !source[a]->hasError()) {
                fiFound = true;
                break;
            }
            else if (source[a]->type==LxPriEndModule || source[a]->type==LxPriAlgEnd)
                break;
            a++;
        }
        if (!fiFound)
            err = _("No 'end' after 'else'");
        setCurrentError(err);
    }
    else if (kw==QString::fromUtf8("исп")) {
        setCurrentIndentRank(0,0);
        setCurrentError(_("Extra 'module'"));
    }
    appendSimpleLine();
    source.at(currentPosition)->alg = currentAlgorhitm;
    source.at(currentPosition)->mod = currentModule;
    source.at(currentPosition)->statement = currentContext.top()->last();
}

void PDAutomataPrivate::setExtraCloseKeywordError(const QString &kw)
{
    if (kw==QString::fromUtf8("все")) {
        setCurrentError(_("Extra 'fi'"));
    }
    else if (kw==QString::fromUtf8("кц")) {
        setCurrentError(_("Extra 'endloop'"));
    }
    else if (kw==QString::fromUtf8("кон")) {
        setCurrentError(_("Extra 'end'"));
    }
    else if (kw==QString::fromUtf8("иначе")) {
        QString err = _("Extra 'else'");
        int a = currentPosition-1;
        while (a>=0) {
            if (source[a]->hasError() &&
                    (source[a]->type==LxPriIf || source[a]->type==LxPriSwitch)
                    ) {
                err = _("No 'end' after 'else'");
                break;
            }
            else if (source[a]->type==LxPriFi)
                break;
            a--;
        }
        setCurrentError(err);
    }
    else if (kw==QString::fromUtf8("ограничение_алгоритма")) {
        setCurrentError(_("Extra statement"));
    }
    else if (kw==QString::fromUtf8("строка_документации")) {
        setCurrentError(_("Extra docstring"));
        appendSimpleLine();
    }
    else if (kw==QString::fromUtf8("выбор")) {
        setCurrentError(_("Extra 'switch'"));
    }
    else if (kw==QString::fromUtf8("при")) {
        setCurrentError(_("Extra 'case'"));
    }
    else if (kw==QString::fromUtf8("если")) {
        setCurrentError(_("Extra 'if'"));
    }
    else if (kw==QString::fromUtf8("то")) {
        setCurrentError(_("Extra 'then'"));
    }
    else {
        setCurrentError(_("Program structure error"));
    }
}

} // namespace KumirAnalizer
