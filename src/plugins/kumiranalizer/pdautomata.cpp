#include "pdautomata.h"

#include "dataformats/ast.h"
#include "interfaces/lexemtype.h"
#include "errormessages/errormessages.h"

#include <deque>
#include <algorithm>

#define isTerminal(x) QRegExp(QString::fromUtf8("!?[a-zа-я][a-zа-я_,]*")).exactMatch(x)
#define isNeterminal(x) QRegExp(QString::fromUtf8("[А-ЯA-Z][А-ЯA-Z1-9_*]*")).exactMatch(x)
#define isNagruzka(x) ( x.startsWith("{") && x.endsWith("}") )

static const int MAXIMUM_ERRORS_EDGES = 1000;

using namespace Shared;

typedef AST::Data AST_Data;
typedef AST::Algorithm AST_Algorhitm;
typedef AST::Statement AST_Statement;

namespace KumirAnalizer {

//static QFile logger("/home/victor/decision.txt");

AST::ModulePtr moduleOfAlgorhitm(AST::DataPtr data, AST::AlgorithmPtr alg) {
    foreach (AST::ModulePtr mod, data->modules) {
        if (mod->impl.algorhitms.contains(alg)) {
            return mod;
        }
    }
    return AST::ModulePtr();
}

void prepareRules(const QStringList &files, QString &out);
QString terminalByCode(unsigned int code);
bool isCorrectTerminal(QString terminal);
QStringList makeAllTerminals(const QString & representation);

PDAutomata::~PDAutomata()
{
    const QStringList keys = matrix_.keys();
    foreach ( const QString & key, keys ) {
        Rules rules = matrix_[key];
        for (int i=0; i<rules.size(); i++) {
            RuleRightPart right = rules[i];
            if (right.script)
                delete right.script;
        }
    }
}

PDAutomata::PDAutomata(QObject *parent) :
    QObject(parent)
{

    QString rulesPath = qApp->property("sharePath").toString()+"/kumiranalizer/"; // TODO set me
    loadRules(rulesPath);
}

void PDAutomata::init(bool teacherMode, const QList<Statement*> & statements, AST::DataPtr ast, AST::AlgorithmPtr algorhitm)
{
    teacherMode_ = teacherMode;
    static Statement * begin = new Statement(LexemType(0xFFFFFFFF));
    source_.clear();
    source_ << begin;
//    logger.open(QIODevice::WriteOnly|QIODevice::Text);
    foreach (Statement * st, statements) {
        if (st->type!=LxTypeComment)
            source_ << st;
    }
    ast_ = ast;
    algorhitm_ = algorhitm;
    if (!algorhitm) {
        QList<AST::ModulePtr>::iterator it = ast_->modules.begin();
        AST::ModuleType moduleToRemove = teacherMode? AST::ModTypeHidden : AST::ModTypeUser;
        while (it!=ast_->modules.end()) {
            if ( (*it)->header.type == moduleToRemove ) {
                it = ast_->modules.erase(it);
            }
            else {
                it++;
            }
        }
        foreach (Statement * st, statements) {
            st->statement.clear();
        }
    }
    else {
        algorhitm->impl.pre.clear();
        algorhitm->impl.post.clear();
        algorhitm->impl.body.clear();
        algorhitm->impl.locals.clear();
        algorhitm->header.arguments.clear();
    }

    errorsCount_ = 0;
    for (int i=0; i<statements.size(); i++) {
        statements[i]->indentRank = QPoint(0,0);
    }
    currentPosition_ = 0;
    stack_.clear();
    clearDataHistory();
    PDStackElem start;
    start.nonTerminal = "START";
    start.iterateStart = 0;
    start.priority = 0;
    stack_.push(start);
    scripts_ = QVector< ScriptListPtr > (source_.size()+2, NULL);
    acceptedRules_ = QVector< QString > (source_.size()+2, QString());
    fixedScripts_ = QVector< ScriptListPtr > (source_.size()+2, NULL);
    nextPointers_ = QVector<int>(source_.size());
    for (int i=0; i<nextPointers_.size(); i++) {
        nextPointers_[i] = i+1;
    }
    allowSkipParts_ = source_.size() >= MAXIMUM_ERRORS_EDGES;
}


void PDAutomata::matchScript(const QString &text, ScriptListPtr & scripts)
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

QStringList makeAllTerminals(const QString & representation)
{
    QStringList result;
    if (!representation.startsWith("!")) {
        result << representation.split(",");
    }
    else {
        const QStringList notTerminals = representation.mid(1).split(",");
        QStringList allTerminals;
//        allTerminals << QString::fromUtf8("begin");
//        allTerminals << QString::fromUtf8("end");
        allTerminals << QString::fromUtf8("простое_предложение");
        allTerminals << QString::fromUtf8("алг");
        allTerminals << QString::fromUtf8("нач");
        allTerminals << QString::fromUtf8("кон");
        allTerminals << QString::fromUtf8("нц");
        allTerminals << QString::fromUtf8("кц");
        allTerminals << QString::fromUtf8("иначе");
        allTerminals << QString::fromUtf8("если");
        allTerminals << QString::fromUtf8("то");
        allTerminals << QString::fromUtf8("все");
        allTerminals << QString::fromUtf8("выбор");
        allTerminals << QString::fromUtf8("при");
        allTerminals << QString::fromUtf8("ограничение_алгоритма");
        allTerminals << QString::fromUtf8("исп");
        allTerminals << QString::fromUtf8("использовать");
        allTerminals << QString::fromUtf8("кон_исп");
        allTerminals << QString::fromUtf8("строка_документации");
        result = allTerminals;
        foreach (const QString notTerminal, notTerminals)
            result.removeAll(notTerminal);
    }
    for (int i=0; i<result.size(); i++) {
        result[i] = result[i].trimmed();
    }
    return result;
}

void PDAutomata::addEpsilonRule(const QString &leftPart, const qreal prior, const QString & script)
{
    static const LexemType allLines[] = {
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
        LxPriAssign,
        LxPriAssert,
        LxPriImport,
        LxPriExit,
        LxPriPause,
        LxPriExit,

        LexemType(0xFFFFFFFF)
    };

    int j = 0;
    do {
        QString key;
        RuleRightPart rule;
        if ( allLines[j] != 0 ) {
            unsigned int code;
            code = allLines[j];
            key = terminalByCode(code) + "/" + leftPart;
        }
        else
            key = "end/" + leftPart;
        Q_ASSERT ( ! key.startsWith("/") );
        rule.nonTerminals = QStringList();
        rule.isEpsilon = true;
        rule.priority = prior;
        matchScript(script.mid(1, script.length()-2), rule.script);
        if ( matrix_.contains(key) ) {
            // Добавляем \epsilon-правило только если нет других правил
            // c тем же приоритетом
            bool allowToAdd = true;
            Rules rulesList = matrix_[key];
            foreach ( RuleRightPart rule, rulesList ) {
                if ( rule.priority == prior ) {
                    allowToAdd = false;
                    break;
                }
            }
            if ( allowToAdd )
                matrix_[key].append(rule);
            else
                delete rule.script;
        }
        else {
            Rules newRulesList;
            newRulesList.append(rule);
            matrix_[key] = newRulesList;
        }
        j++;
    } while ( allLines[j] != 0xFFFFFFFF );
}

void PDAutomata::loadRules(const QString &rulesRoot)
{
    const QDir rulesDir(rulesRoot);
    QStringList rulesFileList = rulesDir.entryList(QStringList() << "[A-Z]*.rules");
    QString normalizedRules;

    for (int i=0; i<rulesFileList.count(); i++) {
        QString absName = rulesDir.absoluteFilePath(rulesFileList[i]);
        rulesFileList[i] = absName;
    }
    prepareRules(rulesFileList, normalizedRules);

//    QFile dbg("/home/victor/work/git/kumir2/rules.txt");
//    dbg.open(QIODevice::WriteOnly);
//    QTextStream ts(&dbg);
//    ts.setCodec("UTF-8");
//    ts << normalizedRules;
//    dbg.close();

    QStringList lines = normalizedRules.split("\n");

    QString leftPart;
    QString rightPart;
    QString script;
    QString terminal;

    int lineNo = 0;
    maxPriorityValue_ = 0;
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
        qreal prior = prior_s.toDouble(&ok);
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
                if (!( terminal[j]=='!' || terminal[j]=='_' || ( terminal[j].isLetter() && terminal[j].isLower() ) )) {
                    isCorrect = false;
                    break;
                }
            }
            if ( !isCorrect )
                return ; // ошибка: первый ксимвол в правой части -- нетерминал
            rightPart = rightPart.mid(space_p+1).trimmed();
        }
        maxPriorityValue_ = qMax(maxPriorityValue_, prior);

        // Добавляем в таблицу правил
        if ( terminal.isEmpty() ) {
            addEpsilonRule(leftPart, prior, script);
        }
        else {
            // Случай обычного правила
            const QStringList possibleTerminals = makeAllTerminals(terminal);
            foreach (const QString & aTerminal, possibleTerminals) {
                Q_ASSERT( isCorrectTerminal(aTerminal) );
                QString key;
                key = aTerminal + "/" + leftPart;
                RuleRightPart rule;
                rule.nonTerminals = rightPart.isEmpty()? QStringList() : rightPart.split(" ");
                rule.isEpsilon = false;
                rule.priority = prior;
                matchScript(script.mid(1, script.length()-2), rule.script);
                if ( matrix_.contains(key) ) {
                    matrix_[key].append(rule);
                    // Удаляем \epsilon-правило, если оно там есть
                    // и имеет тот же приоритет
                    for ( int j=matrix_[key].count()-1; j>=0; j-- ) {
                        if ( matrix_[key][j].isEpsilon && matrix_[key][j].priority==prior ) {
                            delete matrix_[key][j].script;
                            matrix_[key].removeAt(j);
                        }
                    }
                }
                else {
                    Rules newRulesList;
                    newRulesList.append(rule);
                    matrix_[key] = newRulesList;
                }
            }
        }
    }
//    qDebug() << "fff";
    foreach ( QString key, matrix_.keys() ) {
        Rules rulesList = matrix_[key];
        qSort(rulesList);
        matrix_[key] = rulesList;
    }
//    qDebug() << "End load rules";
}

int PDAutomata::process()
{
    if ( stack_.isEmpty() ) {
        if ( currentPosition_ == source_.count() )
            return 0;
        else
            return 1;
    }
    do {
        if (errorsCount_>MAXIMUM_ERRORS_EDGES) {
            setTooManyErrors();
            return 0;
        }
        PDStackElem currentStackElem = stack_.pop();
        QString currentTerminal;
        QString key;
        if ( currentPosition_ > source_.count() )
            break;
        if ( currentPosition_ < source_.count() && currentPosition_ >= 0 ) {
            const Statement * st = source_[currentPosition_];
            currentTerminal = terminalByCode(st->type);
        }
        else {
            currentTerminal = "end";
        }
        key = currentTerminal + "/" + currentStackElem.nonTerminal;
//        logger.write(QString::fromAscii("Processing %1 -> %2 (%3): \n")
//                     .arg(currentStackElem.nonTerminal)
//                     .arg(currentTerminal)
//                     .arg(d->currentPosition)
//                     .toUtf8()
//                     );
//        logger.flush();
        if ( matrix_.contains(key) ) {
            Rules rulesList = matrix_[key];
//            logger.write(QString::fromAscii("rules count = %1\n")
//                         .arg(rulesList.size()).toUtf8()
//                         );
//            logger.flush();
            if ( rulesList.count() == 1 ) {
                // Линейный случай, когда паре {ТЕРМИНАЛ,НЕТЕРМИНАЛ}
                // соответствует только одно правило
                RuleRightPart rule = rulesList[0];
//                logger.write(QString::fromAscii("1:\t[%1] %2\n")
//                             .arg(rule.priority)
//                             .arg(rule.isEpsilon? "0" : rule.nonTerminals.join(" "))
//                             .toUtf8()
//                             );
//                logger.flush();
                if ( !rule.isEpsilon && currentPosition_>=0 ) {
                    scripts_[currentPosition_] = rule.script;
                    acceptedRules_[currentPosition_] = QString::fromAscii("[%1] %2 -> %3 %4")
                            .arg(rule.priority)
                            .arg(currentStackElem.nonTerminal)
                            .arg(currentTerminal)
                            .arg(rule.nonTerminals.join(" "));
                }
                // Если левая часть правила итеративная (*), то помещаем её обратно в стек
                if ( !rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") ) {
                    PDStackElem backElem = currentStackElem;
                    backElem.priority = rule.priority;
                    stack_.push(backElem);
                }
                PDStackElem cse;
                for ( int j=rule.nonTerminals.count()-1; j>=0; j-- ) {
                    cse.nonTerminal = rule.nonTerminals[j];
                    cse.iterateStart = currentPosition_;
                    cse.priority = rule.priority;
                    stack_.push(cse);
                }
                // Если отрабатываем правило вида: ИТЕРАТИВНЫЙ_НЕТЕРМИНАЛ* -> 0,
                // то устанавливаем соответствующий next-указатель
                if ( rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") && currentTerminal!="end")
                    finalizeIterativeRule(currentStackElem);
                if ( !rule.isEpsilon && !key.startsWith("end/") )
                    nextStep();
            }
            else {
                // Разветвление. Сохраняем состояние и пытаемся применять
                // правила до тех пор, пока одно из них не будет успешным
                uint success = 0;

                // 				int errorsBefore = m_errorWayCounter;
                for ( int i=0; i<rulesList.count(); i++ )  {
                    if (i>0)
                        errorsCount_ ++;
                    saveData();
                    RuleRightPart rule = rulesList[i];
//                    logger.write(QString::fromAscii("%1:\t[%2] %3\n")
//                                 .arg(i)
//                                 .arg(rule.priority)
//                                 .arg(rule.isEpsilon? "0" : rule.nonTerminals.join(" "))
//                                 .toUtf8()
//                                 );
//                    logger.flush();

                    if ( !rule.isEpsilon && currentPosition_>=0 ) {
                        scripts_[currentPosition_] = rule.script;
                        acceptedRules_[currentPosition_] = QString::fromAscii("[%1] %2 -> %3 %4")
                                .arg(rule.priority)
                                .arg(currentStackElem.nonTerminal)
                                .arg(currentTerminal)
                                .arg(rule.nonTerminals.join(" "));
                    }
                    // Если левая часть правила итеративная (*), то помещаем её обратно в стек
                    if ( !rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") ) {
                        PDStackElem backElem = currentStackElem;
                        backElem.priority = rule.priority;
                        stack_.push(backElem);
                    }
                    PDStackElem cse;
                    for ( int j=rule.nonTerminals.count()-1; j>=0; j-- ) {
                        cse.nonTerminal = rule.nonTerminals[j];
                        cse.iterateStart = currentPosition_;
                        cse.priority = rule.priority;
                        stack_.push(cse);
                    }
                    // Если отрабатываем правило вида: ИТЕРАТИВНЫЙ_НЕТЕРМИНАЛ* -> 0,
                    // то устанавливаем соответствующий next-указатель
                    if ( rule.isEpsilon && currentStackElem.nonTerminal.endsWith("*") && currentTerminal!="end")
                        finalizeIterativeRule(currentStackElem);
                    if ( !rule.isEpsilon && !key.startsWith("end/") )
                        nextStep();
                    success = process();
                    if ( success == 0 ) {
                        popHistory();
                        break;
                    }
                    if ( success == 1 ) {
                        restoreData();
                    }
                }
                if ( success != 0 ) {
                    break;
                }
            }
        } // end if (matrix has key)
        else {
            return 1;
        }
    } while ( !stack_.isEmpty() );
    bool hasEnd = currentPosition_ == source_.count();

    return ( stack_.isEmpty() && hasEnd ) ? 0 : 1;
}

static bool isEmptyRulesLine(const QString & s) {
    for (int i=0; i<s.length(); i++) {
        if (s[i].isSpace())
            continue;
        else if (s[i]=='#')
            // found a comment symbol
            // --- this line is like empty
            return true;
        else
            // found non comment and non space symbol in line
            // --- it is NOT empty line this case
            return false;
    }
    // nothing instead of spaces in this line
    // -- line is empty this case
    return true;
}

inline int lineLevel(const QString & line) {
    return line.count("{") - line.count("}");
}

static void cleanupRuleLine(QString & line) {
    int commentPos = line.indexOf('#');
    if (commentPos!=-1) {
        line = line.left(commentPos);
    }
    line = line.trimmed();
}

static void mergeRuleLines(std::list<QString> & lines)
{
    typedef std::list<QString>::iterator It;
    It current = lines.begin();
    It nextToCurrent;
    It statement;
    for ( ; current!=lines.end(); ) {
        QString & currentLine = *current;
        cleanupRuleLine(currentLine);
        int currentLevel = lineLevel(currentLine);
        if (currentLevel!=0 || currentLine.endsWith("\\")) {
            nextToCurrent = current;
            ++nextToCurrent;
            statement = nextToCurrent;
            for ( ; statement!=lines.end(); ++statement) {
                QString & statementLine = *statement;
                cleanupRuleLine(statementLine);
                currentLevel += lineLevel(statementLine);
                if (currentLine.endsWith("\\"))
                    currentLine = currentLine.remove(currentLine.length()-1, 1);
                currentLine += " " + statementLine;
                if (currentLevel==0 && !statementLine.endsWith("\\")) {
                    statement ++;
                    break;
                }
            }
            current = lines.erase(nextToCurrent, statement);
        }
        else {
            current ++;
        }
    }
}

static std::list<QString> readRulesFile(const QString & fileName)
{
    std::list<QString> result;
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        return result;
    }
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    result = ts.readAll().split("\n").toStdList();
    f.close();
    result.remove_if(isEmptyRulesLine);
    mergeRuleLines(result);
    return result;
}

static std::deque<QString> readAllRulesFiles(const QStringList & files)
{
    std::deque<QString> result;
    foreach (const QString & fileName, files) {
        std::list<QString> fileLines = readRulesFile(fileName);
        result.insert(result.end(), fileLines.begin(), fileLines.end());
    }
    return result;
}

struct RulesLine {
    QString leftPart;
    QString rightPart;
    QString data;
    qreal priority;
};

static RulesLine parseRulesLine(const QString & line) {
    RulesLine result;
    static QRegExp rxRule(
                "\\[(\\d+\\.?\\d*)\\]" /* floating-point value inside [...] */
                "\\s*"                 /* possible spaces */
                "(\\S+)"               /* non-empty left part */
                "\\s*"                 /* possible spaces */
                "->"                   /* -> symbol */
                "([^:]+)"              /* non-empty right part */
                ":?(.+)?"              /* possible : followed by script */
                );
    if (rxRule.indexIn(line)!=-1) {
        const QStringList caps = rxRule.capturedTexts();
        bool ok;
        result.priority = caps[1].toDouble(&ok);
        if (!ok)
            throw QString::fromAscii("Wrong priority at line: %1").arg(line);
        result.leftPart = caps[2].simplified();
        result.rightPart = caps[3].simplified();
        if (caps.size()>4)
            result.data = caps[4].mid(1).simplified();
    }
    else {
        throw QString::fromAscii("Wrong rule line: %1").arg(line);
    }
    return result;
}

bool hasEpsilonRule(const std::list<RulesLine> & rules,
                    const QString & left, const qreal priority) {
    typedef std::list<RulesLine>::const_iterator It;
    for (It it = rules.begin(); it!=rules.end(); ++it) {
        const RulesLine & rule = *it;
        if (rule.priority==priority && rule.rightPart=="0" && rule.leftPart==left)
            return true;
    }
    return false;
}

void insertEpsilonRules(std::list<RulesLine> & rules) {
    typedef std::list<RulesLine>::iterator It;
    for ( It it = rules.begin(); it!=rules.end(); ) {
        RulesLine & rule = *it;
        bool iterative = rule.leftPart.endsWith("*");
        bool mustInsertEpsilon =
                iterative &&
                !hasEpsilonRule(rules, rule.leftPart, rule.priority);
        if (mustInsertEpsilon) {
            RulesLine epsilonRule;
            epsilonRule.leftPart = rule.leftPart;
            epsilonRule.rightPart = "0";
            epsilonRule.priority = rule.priority;
            it = rules.insert(it, epsilonRule);
        }
        else {
            it ++;
        }
    }
}

void prepareRules(const QStringList &files, QString &out)
{
    int iskNet = 0;


    std::deque<QString> ruleLines = readAllRulesFiles(files);

    std::list<RulesLine> rules;
    try {
        for (auto it=ruleLines.begin(); it!=ruleLines.end(); ++it) {
            rules.push_back(parseRulesLine(*it));
        }
    }
    catch (const QString & error) {
        qDebug() << "ERROR PARSING RULES: " << error;
    }

    insertEpsilonRules(rules);

    QStringList pravila1;

    for (auto it=rules.begin(); it!=rules.end(); ++it) {
        QString subochLine = QString::fromAscii("%1 -> %2 : %3 [%4]")
                .arg(it->leftPart)
                .arg(it->rightPart)
                .arg(it->data)
                .arg(it->priority);
        pravila1.push_back(subochLine);
    }

    // The code below is written by N.M.Suboch in 2008 and undocumented :-(
    // So it is kept "as is".

    pravila1.sort();

    QStringList nagruzki1;
    QList<qreal> priorities;
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
        priorities << p.mid(1,p.length()-2).toDouble();
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
        if (nagruzki1[i].trimmed().isEmpty())
        {
            for(int j=0; j<pravila[i].size(); ++j)
            {
                if (isTerminal(pravila[i][j]))
                    nagruzki1[i] += "{}";
            }
            nagruzki1[i] = nagruzki1[i].simplified();
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
        QList<qreal> addPriors;
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
            QList<qreal> addPriors;
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
        qreal priority = priorities[c];
        for(int j=c+1; j<pravila.size(); )
        {
            QString pravilo1( pravila[j].join(" ") );
            QString nagruzka1( nagruzki[j].join(" ") );
            qreal priority1 = priorities[j];
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

void prepareRules_By_N_M_Suboch(const QStringList &files, QString &out, const QList<int> &priors)
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
        str = str.trimmed();
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


QString terminalByCode(unsigned int code)
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

bool isCorrectTerminal(QString terminal)
{
    if (terminal.startsWith('!'))
        terminal.remove(0, 1);
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

void PDAutomata::finalizeIterativeRule(const PDStackElem & stackElem)
{
    int iterationStartPos = stackElem.iterateStart;
    for ( int i=iterationStartPos+1; i<currentPosition_; i++ ) {
        if ( fixedScripts_[i]==NULL )
            fixedScripts_[i] = scripts_[i];
    }
    if ( allowSkipParts_ || stackElem.priority==0 )
        nextPointers_[iterationStartPos] = currentPosition_;
}

void PDAutomata::saveData()
{
    history_stack_.push(stack_);
    history_currentPosition_.push(currentPosition_);
    history_scripts_.push(scripts_);
    history_nextPointers_.push(nextPointers_);
}

void PDAutomata::restoreData()
{
    currentPosition_ = history_currentPosition_.pop();
    stack_ = history_stack_.pop();
    scripts_ = history_scripts_.pop();
    nextPointers_ = history_nextPointers_.pop();
}

void PDAutomata::popHistory()
{
    history_currentPosition_.pop();
    history_stack_.pop();
    history_scripts_.pop();
    history_nextPointers_.pop();
}

void PDAutomata::clearDataHistory()
{
    history_currentPosition_.clear();
    history_stack_.clear();
    history_scripts_.clear();
    history_nextPointers_.clear();
}

void PDAutomata::nextStep()
{
    currentPosition_ = currentPosition_<nextPointers_.size()
            ? nextPointers_[currentPosition_]
            : source_.size();
}

void PDAutomata::postProcess()
{
//    logger.close();
    currentContext_.clear();
    currentPosition_ = 0;
    if (!algorhitm_) {
        currentModule_ = AST::ModulePtr(new AST::Module());
        if (teacherMode_) {
            currentModule_->header.type = AST::ModTypeHidden;
            currentModule_->header.name = "@";
        }
        currentAlgorhitm_.clear();
        currentContext_.push(&(currentModule_->impl.initializerBody));
    }
    else {
        AST::ModulePtr mod;
        for (int i=0; i<ast_->modules.size(); i++) {
            for (int j=0; j<ast_->modules[i]->impl.algorhitms.size(); j++) {
                if (algorhitm_==ast_->modules[i]->impl.algorhitms[j]) {
                    mod = ast_->modules[i];
                    break;
                }
            }
            if (mod)
                break;
        }
        currentModule_ = mod;
//        d->currentContext.push(&(d->algorhitm->impl.body));
        currentAlgorhitm_ = algorhitm_;
    }

    for (int i=0; i<scripts_.size(); i++) {
//        qDebug() << i << ": " << d->acceptedRules[i];
        ScriptListPtr scripts = scripts_[i];
        if (!scripts) {
            scripts = fixedScripts_[i];
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
                        m.invoke(this, Qt::DirectConnection,
                                 Q_ARG(int, arguments[0].toInt())
                                 );
                    }
                    else {
                        m.invoke(this, Qt::DirectConnection,
                                 Q_ARG(QString, arguments[0].toString())
                                 );
                    }
                    break;
                case 2:
                    if (QString::fromAscii(m.parameterTypes()[0])=="int") {
                        m.invoke(this, Qt::DirectConnection,
                                 Q_ARG(int, arguments[0].toInt()),
                                 Q_ARG(int, arguments[1].toInt())
                                 );
                    }
                    else {
                        m.invoke(this, Qt::DirectConnection,
                                 Q_ARG(QString, arguments[0].toString()),
                                 Q_ARG(int, arguments[1].toInt())
                                 );
                    }
                    break;
                default:
                    m.invoke(this, Qt::DirectConnection);
                }
            }
        }
        currentPosition_ ++;
    }
    if (currentModule_ && !algorhitm_) {
        ast_->modules << currentModule_;
    }
//    if (!algorhitm_ && currentAlgorhitm_) {
//        bool foundAlgorhitm = false;
//        for (int i=0; i<ast_->modules.size(); i++) {
//            AST::ModulePtr module = ast_->modules[i];
//            foundAlgorhitm = module->impl.algorhitms.contains(currentAlgorhitm_);
//            if (foundAlgorhitm)
//                break;
//        }
//        if (currentModule_ && !foundAlgorhitm) {
//            foundAlgorhitm = currentModule_->impl.algorhitms.contains(currentAlgorhitm_);
//        }
//        if (!foundAlgorhitm)
//            delete currentAlgorhitm_;
//    }
    source_.pop_front();
//    if (!ast_->modules.contains(currentModule_)) {
//        delete currentModule_;
//        currentModule_ = 0;
//    }
    currentAlgorhitm_.clear();

}

void PDAutomata::setCurrentIndentRank(int start, int end)
{   
    source_[currentPosition_]->indentRank = QPoint(start, end);
}

void PDAutomata::suggest(const QString &text, int moveCursorBackLinesCount)
{
    QString txt = text;
    txt.replace("\\n","\n");
    Statement * st = nullptr;
    if (currentPosition_<source_.size())
        st = source_.at(currentPosition_);
    else {
        for (int i=0; i<source_.size(); i++) {
            if (source_[i]->mod == currentModule_ && source_[i]->type==LxPriModule) {
                st = source_.at(i);
                break;
            }
        }
    }
    if (st) {
        st->suggestedClosingBracket.first = txt;
        if (moveCursorBackLinesCount>=0)
            st->suggestedClosingBracket.second = quint32(moveCursorBackLinesCount);
        else
            st->suggestedClosingBracket.second = 0;
    }
}

void PDAutomata::setCurrentError(const QString &value)
{
    for (int i=0; i<source_[currentPosition_]->data.size(); i++) {
        if (source_[currentPosition_]->data[i]->type!=LxTypeComment) {
            source_[currentPosition_]->data[i]->error = value;
            source_[currentPosition_]->data[i]->errorStage = AST::Lexem::PDAutomata;
        }
    }
}

void PDAutomata::setCurrentErrorRaisePosition(Lexem::ErrorRaisePosition pos)
{
    for (int i=0; i<source_[currentPosition_]->data.size(); i++) {
        if (source_[currentPosition_]->data[i]->type!=LxTypeComment) {
            source_[currentPosition_]->data[i]->errorRaisePosition = pos;
        }
    }
}

void PDAutomata::setOutOfAlgError()
{
    QString value;
    if (source_[currentPosition_]->type & LxNameClass) {
        value = _("Variable declaration out of algorhitm");
    }
    else {
        value = _("Instruction out of algorhitm");
    }
    for (int i=0; i<source_[currentPosition_]->data.size(); i++) {
        source_[currentPosition_]->data[i]->error = value;
        source_[currentPosition_]->data[i]->errorStage = AST::Lexem::PDAutomata;
    }

    bool doNotEvaluate = false;

    if (currentModule_)
        doNotEvaluate = currentModule_->impl.algorhitms.size()>0;

    bool hasAnyAlgorithmAfter = false;
    for (int i=0; i<source_.size(); i++) {
        if (source_[i]->type==LxPriAlgHeader) {
            hasAnyAlgorithmAfter = true;
            break;
        }
    }

    if (!hasAnyAlgorithmAfter)
        doNotEvaluate = false;

    appendSimpleLine();
    if (!currentContext_.isEmpty() && currentContext_.top())
        currentContext_.top()->last()->skipErrorEvaluation = doNotEvaluate;
}

void PDAutomata::processAlgWithNoBegin()
{
    setCurrentIndentRank(0, +1);
    processCorrectAlgHeader();
    processCorrectAlgBegin();
    Statement * errorStatement = source_.at(currentPosition_);
    for (int i=currentPosition_+1; i<source_.size(); i++) {
        // Try to find a statement after which there shound be 'begin'
        Statement * st = source_.at(i);
        if (st->type==Shared::LxTypeDoc ||
                st->type==Shared::LxPriPre ||
                st->type==Shared::LxPriPost)
            errorStatement = st;
        else if (st->type!=Shared::LxTypeComment) {
            break;
        }
    }
    QString errorText;
    if (errorStatement->type==Shared::LxTypeDoc)
        errorText = _("No 'begin' after last document line");
    else if (errorStatement->type==Shared::LxPriAlgHeader)
        errorText = _("No 'begin' after 'alg'");
    else if (errorStatement->type==Shared::LxPriPre)
        errorText = _("No 'begin' after 'pre'");
    else if (errorStatement->type==Shared::LxPriPost)
        errorText = _("No 'begin' after 'post'");
    errorStatement->setError(errorText, Lexem::PDAutomata, Lexem::Header);
    if (currentAlgorhitm_) {
        foreach (Lexem * lx, currentAlgorhitm_->impl.headerLexems) {
            lx->error = errorText;
            lx->errorRaisePosition = Lexem::Header;
            lx->errorStage = Lexem::PDAutomata;
        }
    }
}

void PDAutomata::processCorrectAlgHeader()
{
    AST::AlgorithmPtr alg = algorhitm_? algorhitm_ : AST::AlgorithmPtr(new AST::Algorithm);
    if (teacherMode_)
        alg->header.specialType = AST::AlgorhitmTypeTeacher;
    alg->impl.headerLexems = source_[currentPosition_]->data;
    currentAlgorhitm_ = alg;
    if (algorhitm_) {
        source_.at(currentPosition_)->mod = moduleOfAlgorhitm(ast_, algorhitm_);
        currentModule_ = source_.at(currentPosition_)->mod;
        currentAlgorhitm_ = algorhitm_;
    }
    else {
        if (currentModule_==0) {
            AST::ModulePtr lastModule = ast_->modules.last();
            if (lastModule->header.type == AST::ModTypeUser) {
                lastModule->impl.algorhitms << alg;
                source_.at(currentPosition_)->mod = lastModule;
            }
            setCurrentError(_("Algorithm out of module"));
        }
        else {
            currentModule_->impl.algorhitms << alg;
            source_.at(currentPosition_)->mod = currentModule_;
        }
    }
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
}

void PDAutomata::processCorrectAlgBegin()
{
    setCurrentIndentRank(  0, +1);
    if (currentAlgorhitm_) {
        currentAlgorhitm_->impl.beginLexems = source_.at(currentPosition_)->data;
        source_.at(currentPosition_)->mod = currentModule_;
        source_.at(currentPosition_)->alg = currentAlgorhitm_;
        currentContext_.push(&(currentAlgorhitm_->impl.body));
    }
}

void PDAutomata::appendSimpleLine()
{
    AST::StatementPtr statement = AST::StatementPtr(new AST::Statement);
    statement->skipErrorEvaluation = false;
    statement->lexems = source_.at(currentPosition_)->data;
    switch ( source_.at(currentPosition_)->type ) {
    case LxPriAssign:
        statement->type = AST::StAssign;
        break;
    case LxPriAssert:
        statement->type = AST::StAssert;
        break;
    case LxNameClass: {
            // Check for we are in algorhitm top or module top
            if (currentContext_.size()>1) {
                QList<AST::StatementPtr> * body = currentContext_[currentContext_.size()-2];
                if (!body->isEmpty()) {
                    AST::StatementPtr st = body->last();
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
    if (teacherMode_) {
        if (currentContext_.size()<=1) {
            // Can't do anything out of algorhitms
            foreach (Lexem * lx, statement->lexems) {
                lx->error = _("Hidden part must contain only algorithm");
                lx->errorStage = AST::Lexem::PDAutomata;
            }

            statement->type = AST::StError;
        }
    }
    if ( source_.at(currentPosition_)->data[0]->error.size()>0 ) {
        statement->type = AST::StError;
        statement->error = source_.at(currentPosition_)->data[0]->error;
    }
    if (!currentContext_.isEmpty() && currentContext_.top()) {
        if (statement->type==AST::StError) {
            if (currentContext_.top()==&(currentModule_->impl.initializerBody)) {
                if (currentModule_->impl.algorhitms.size()>0) {
                    statement->skipErrorEvaluation = true;
                }
            }
        }
        currentContext_.top()->append(statement);
    }
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = statement;
}

AST::StatementPtr PDAutomata::createSimpleAstStatement(Statement * st)
{
    AST::StatementPtr statement = AST::StatementPtr(new AST::Statement);
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

void PDAutomata::processCorrectAlgEnd()
{
    int indentStart = 0;
    bool beginFound = false;
    for (int i=currentPosition_-1; i>=0; i--) {
        if (source_[i]->type==LxPriAlgBegin) {
            if (currentContext_.size()>0 && currentContext_.top()->contains(source_[i]->statement)) {

            }
            else {
                beginFound = true;
                indentStart = - source_[i]->indentRank.y();
                break;
            }
        }
    }

    if (!beginFound) {
        for (int i=currentPosition_-1; i>=0; i--) {
            if (source_[i]->type==LxPriAlgHeader) {
                if (currentContext_.size()>0 && currentContext_.top()->contains(source_[i]->statement)) {

                }
                else {
                    indentStart = - source_[i]->indentRank.y();
                    break;
                }
            }
        }
    }

    setCurrentIndentRank(indentStart,  0);
    if (currentAlgorhitm_) {
        currentAlgorhitm_->impl.endLexems = source_.at(currentPosition_)->data;
    }
    currentContext_.pop();
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    currentAlgorhitm_.clear();
}

void PDAutomata::addDummyAlgHeader()
{
    // nothind to do
}

void PDAutomata::setModuleBeginError(const QString & value)
{
    for (int i=0; i<source_.size(); i++) {
        if (source_[i]->mod == currentModule_ && source_[i]->type==LxPriModule) {
            for (int a=0; a<source_[i]->data.size(); a++) {
                source_[i]->data[a]->error = value;
                source_[i]->data[a]->errorStage = AST::Lexem::PDAutomata;
            }
            source_[i]->indentRank = QPoint(0, 0);
        }
    }
}

void PDAutomata::processCorrectEndOfLoop()
{
    setCurrentIndentRank(-1,  0);
    currentContext_.pop();
    Q_ASSERT(currentContext_.size()>0);
    QList<AST::StatementPtr> * topContext = currentContext_.top();
    AST::StatementPtr beginOfLoop;
    for (int i=topContext->size()-1; i>=0; i--) {
        AST::StatementPtr st = topContext->at(i);
        if (st->type==AST::StLoop) {
            beginOfLoop = st;
            break;
        }
    }
    if (beginOfLoop) {
        beginOfLoop->loop.endLexems = source_.at(currentPosition_)->data;
        source_[currentPosition_]->statement = beginOfLoop;
    }
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
}

void PDAutomata::processAlgEndInsteadOfLoopEnd()
{
    setCurrentIndentRank(-1, 0);
    setCurrentError(_("'end' instead of 'endloop'"));
//    currentContext.pop();
    Q_ASSERT(currentContext_.size()>0);
//    Q_ASSERT(currentContext.top()->last()->type==AST::StLoop);
    if (currentContext_.top()->last()->type==AST::StLoop) {
        currentContext_.top()->last()->loop.endLexems = source_.at(currentPosition_)->data;
    }
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = currentContext_.top()->last();
    if (currentContext_.top()->last()->type==AST::StLoop) {
        currentContext_.pop();
    }
}

void PDAutomata::processModEndInsteadOfAlgEnd()
{
    int a = currentPosition_-1;
    int modDeclPos = -1;
    int beginPos = -1;
    int algPos = -1;
    while (a>=0) {
        if (source_[a]->type==LxPriEndModule && !source_[a]->hasError()) {
            break;
        }
        else if (source_[a]->type==LxPriAlgBegin) {
            beginPos = a;
        }
        else if (source_[a]->type==LxPriAlgHeader) {
            algPos = a;
        }
        else if (source_[a]->type==LxPriAlgEnd) {
            algPos = beginPos = -1;
            break;
        }
        else if (source_[a]->type==LxPriEndModule && !source_[a]->hasError()) {
            modDeclPos = -1;
            break;
        }
        else if (source_[a]->type==LxPriModule && !source_[a]->hasError()) {
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
            for (int i=0; i<source_[pos]->data.size(); i++) {
                if (source_[pos]->data[i]->type!=LxTypeComment) {
                    source_[pos]->data[i]->error = err;
                    source_[pos]->data[i]->errorStage = AST::Lexem::PDAutomata;
                }
            }
            if (currentContext_.size()==1) { // Module initializer
                AST::StatementPtr st = AST::StatementPtr(new AST::Statement);
                st->type = AST::StError;
                st->lexems = source_[pos]->data;
                st->error = err;
                AST::ModulePtr mod = ast_->modules.last();
                mod->impl.initializerBody.prepend(st);
            }
            setCurrentIndentRank(-2, 0);
        }
    }
}

void PDAutomata::processCorrectCase()
{
    setCurrentIndentRank(-1, +1);
    currentContext_.pop();
    if (currentContext_.size()==0 || currentContext_.top()->size()==0)
        return;
    Q_ASSERT(currentContext_.size()>0);
    Q_ASSERT(currentContext_.top()->last()->type==AST::StSwitchCaseElse);
    AST::ConditionSpec cond;
    cond.lexems = source_.at(currentPosition_)->data;
    cond.condition.clear();
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = currentContext_.top()->last();
    source_.at(currentPosition_)->conditionalIndex = currentContext_.top()->last()->conditionals.size();
    currentContext_.top()->last()->conditionals << cond;
    currentContext_.push(&(currentContext_.top()->last()->conditionals.last().body));
}

void PDAutomata::processCorrectIf()
{
    setCurrentIndentRank(0, +2);
    AST::StatementPtr st = AST::StatementPtr(new AST::Statement);
    st->skipErrorEvaluation = false;
    st->type = AST::StIfThenElse;
    st->lexems = source_.at(currentPosition_)->data;
    currentContext_.top()->append(st);
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = currentContext_.top()->last();
}

void PDAutomata::processCorrectThenIfNotExists()
{
    bool foundThen
            = currentAlgorhitm_
            && currentAlgorhitm_->impl.body.size()>0
            && currentAlgorhitm_->impl.body.last()->type==AST::StIfThenElse
            && currentAlgorhitm_->impl.body.last()->conditionals.size()>0;
    if (!foundThen)
        processCorrectThen();
}

void PDAutomata::processCorrectThenIfNotExists2()
{
    bool foundThenContext
            = currentContext_.size()>1
            && currentContext_.at(currentContext_.size()-2)->size()>0
            && currentContext_.at(currentContext_.size()-2)->last()->type == AST::StIfThenElse
            && currentContext_.at(currentContext_.size()-2)->last()->conditionals.size()>0;
    if (!foundThenContext)
        processCorrectThen();
}


void PDAutomata::processCorrectThen()
{
    setCurrentIndentRank(-1, +1);
    Q_ASSERT(currentContext_.size()>0);
    while (currentContext_.top()->size()>0 && currentContext_.top()->last()->type==AST::StError) {
        currentContext_.top()->pop_back();
    }
    while (currentContext_.top()->size()==0) {
        currentContext_.pop_back();
    }
    Q_ASSERT(currentContext_.top()->last()->type==AST::StIfThenElse);
    AST::ConditionSpec cond;
    cond.parent = currentContext_.top()->last();

    cond.lexems = source_.at(currentPosition_)->data;
    cond.condition.clear();
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = currentContext_.top()->last();
    source_.at(currentPosition_)->conditionalIndex = currentContext_.top()->last()->conditionals.size();
    if (currentContext_.top()->last()->conditionals.size()==0)
        currentContext_.top()->last()->conditionals << cond;
    currentContext_.push(&(currentContext_.top()->last()->conditionals.last().body));
}

void PDAutomata::processCorrectFi()
{
    bool doPop = true;

    if (currentAlgorhitm_ && currentContext_.last()==&(currentAlgorhitm_->impl.body))
        doPop = false;

    if (currentModule_ && currentContext_.last()==&(currentModule_->impl.initializerBody))
        doPop = false;

    if (doPop)
        currentContext_.pop();

    if (currentContext_.size()>0 && currentContext_.last()->size()>0 && (currentContext_.top()->last()->type==AST::StIfThenElse
                                            || currentContext_.top()->last()->type==AST::StSwitchCaseElse))
    {
        if (currentContext_.top()->last()->conditionals.size()>0) {
            setCurrentIndentRank(-2, 0);
        }
        else {
            setCurrentIndentRank(-2, 0);
//            setCurrentError(_("Extra 'fi'"));
        }
        source_.at(currentPosition_)->mod = currentModule_;
        source_.at(currentPosition_)->alg = currentAlgorhitm_;
        source_.at(currentPosition_)->statement = currentContext_.top()->last();
        source_.at(currentPosition_)->statement->endBlockLexems = source_.at(currentPosition_)->data;
    }
}


void PDAutomata::processCorrectElse()
{
    setCurrentIndentRank(-1, +1);
//    bool shouldPopContext = true;
//    bool thenFound = false;
//    for (int i=currentPosition-1; i>=0; i--) {
//        if (source[i]->type==LxPriThen) {
//            thenFound = true;
//        }
//        else if (source[i]->type==LxPriIf) {
//            shouldPopContext = thenFound;
//            break;
//        }
//    }
//    if (shouldPopContext)
    currentContext_.pop();
    Q_ASSERT(currentContext_.size()>0);
    if (currentContext_.top()->size()>0 && (currentContext_.top()->last()->type==AST::StIfThenElse ||
            currentContext_.top()->last()->type==AST::StSwitchCaseElse)
            )
    {
        AST::ConditionSpec cond;
        cond.lexems = source_.at(currentPosition_)->data;
        cond.condition.clear();
        source_.at(currentPosition_)->mod = currentModule_;
        source_.at(currentPosition_)->alg = currentAlgorhitm_;
        source_.at(currentPosition_)->statement = currentContext_.top()->last();
        source_.at(currentPosition_)->conditionalIndex = currentContext_.top()->last()->conditionals.size();
        currentContext_.top()->last()->conditionals << cond;
        currentContext_.push(&(currentContext_.top()->last()->conditionals.last().body));
    }
    else {
        // Error: no if..then
        AST::StatementPtr ste = AST::StatementPtr(new AST::Statement);
        ste->skipErrorEvaluation = false;
        ste->type = AST::StError;
        ste->error = _("No then before else");
        ste->lexems = source_.at(currentPosition_)->data;
        foreach (Lexem * lx, source_.at(currentPosition_)->data) {
            lx->error = ste->error;
        }
        currentContext_.top()->append(ste);
        AST::StatementPtr st = AST::StatementPtr(new AST::Statement);
        st->skipErrorEvaluation = false;
        st->type = AST::StIfThenElse;
        st->lexems = source_.at(currentPosition_)->data;
        source_.at(currentPosition_)->mod = currentModule_;
        source_.at(currentPosition_)->alg = currentAlgorhitm_;
        source_.at(currentPosition_)->statement = currentContext_.top()->last();
        currentContext_.top()->append(st);

        AST::ConditionSpec cond;
        cond.lexems = source_.at(currentPosition_)->data;
        cond.condition.clear();
        source_.at(currentPosition_)->mod = currentModule_;
        source_.at(currentPosition_)->alg = currentAlgorhitm_;
        source_.at(currentPosition_)->statement = currentContext_.top()->last();
        source_.at(currentPosition_)->conditionalIndex = currentContext_.top()->last()->conditionals.size();
        currentContext_.top()->last()->conditionals << cond;
        currentContext_.push(&(currentContext_.top()->last()->conditionals.last().body));

    }
}

void PDAutomata::processCorrectSwitch()
{
    setCurrentIndentRank(0, +2);
    AST::StatementPtr st = AST::StatementPtr(new AST::Statement);
    st->skipErrorEvaluation = false;
    st->type = AST::StSwitchCaseElse;
    st->lexems = source_.at(currentPosition_)->data;
    currentContext_.top()->append(st);
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = currentContext_.top()->last();

    // Push dummy case statement into context for
    // correct processing of first 'case' statement

    currentContext_.push(0);
}

void PDAutomata::processCorrectBeginOfLoop()
{
    setCurrentIndentRank(0, +1);
    AST::StatementPtr st = AST::StatementPtr(new AST::Statement);
    st->skipErrorEvaluation = false;
    st->type = AST::StLoop;
    st->lexems = source_.at(currentPosition_)->data;
    currentContext_.top()->append(st);
    currentContext_.push(&(currentContext_.top()->last()->loop.body));
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = st;

}

void PDAutomata::processCorrectDocLine()
{
    // TODO implement me
}

void PDAutomata::processCorrectRestrictionLine()
{
    AST::StatementPtr st = AST::StatementPtr(new AST::Statement);
    st->skipErrorEvaluation = false;
    st->type = AST::StAssert;
    st->lexems = source_.at(currentPosition_)->data;
    source_.at(currentPosition_)->mod = currentModule_;
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->statement = st;
    if (currentAlgorhitm_) {
        if ( source_.at(currentPosition_)->type==LxPriPre ) {
            if ( source_.at(currentPosition_)->data.size()>1 )
                currentAlgorhitm_->impl.pre.append(st);
        }
        else {
            if ( source_.at(currentPosition_)->data.size()>1 )
                currentAlgorhitm_->impl.post.append(st);
        }
    }
}

void PDAutomata::processCorrectModuleBegin()
{
    setCurrentIndentRank(0, +1);
    if (currentModule_) {
        ast_->modules << currentModule_;
    }
    currentModule_ = AST::ModulePtr(new AST::Module);
    currentAlgorhitm_.clear();
    currentModule_->header.type = teacherMode_? AST::ModTypeHidden : AST::ModTypeUser;
    source_.at(currentPosition_)->mod = currentModule_;
    currentContext_.push(&(currentModule_->impl.initializerBody));
}

void PDAutomata::processCorrectModuleEnd()
{
    setCurrentIndentRank(-1, 0);
    source_.at(currentPosition_)->mod = currentModule_;
    if (currentModule_ && !algorhitm_) {
        ast_->modules << currentModule_;
    }
    currentModule_.clear();
}

void PDAutomata::processCorrectLoad()
{
    source_[currentPosition_]->mod = currentModule_;
    source_[currentPosition_]->alg.clear();
}

void PDAutomata::setGarbageAlgError()
{
    static const QList<LexemType> OutgoingOperationalBrackets =
            QList<LexemType>() << LxPriImport << LxPriAlgHeader << LxPriModule << LxPriEndModule;

    QString error;
    if (OutgoingOperationalBrackets.contains(source_[currentPosition_]->type)) {
        error = _("'%1' in algorithm", source_[currentPosition_]->data.first()->data);
    }
    else {     
        error = _("Garbage between alg..begin");
    }
    setCurrentError(error);
    setCurrentErrorRaisePosition(Lexem::Header);
    if (currentAlgorhitm_) {
        int lineNo = -1;
        if (source_[currentPosition_]->data.size()>0) {
            lineNo = source_[currentPosition_]->data[0]->lineNo;
        }
        currentAlgorhitm_->impl.headerRuntimeError = error;
        currentAlgorhitm_->impl.headerRuntimeErrorLine = lineNo;
    }
    appendSimpleLine();
}

void PDAutomata::setGarbageIfThenError()
{
    bool hasThen = false;
    for (int i=currentPosition_+1; i<source_.size(); i++) {
        if (source_[i]->type==LxPriThen) {
            hasThen = true;
            break;
        }
        if (source_[i]->type==LxPriFi) {
            break;
        }
    }
    const QString error = hasThen? _("Garbage between if..then")
                                 : _("No 'then' after 'if'");
    AST::StatementPtr ifStatement;
    if (currentContext_.size()>0
            && currentContext_.top()->size()>0
            && currentContext_.top()->at(0)->type==AST::StIfThenElse
            )
    {
        ifStatement = currentContext_.top()->at(0);
        currentContext_.top()->at(0)->headerError = error;
        int lineNo = -1;
        if (source_[currentPosition_]->data.size()>0) {
            lineNo = source_[currentPosition_]->data[0]->lineNo;
        }
        currentContext_.top()->at(0)->headerErrorLine = lineNo;
    }
    if (ifStatement) {
        for (int i=0; i<source_.size(); i++) {
            Statement * st = source_.at(i);
            if (st->statement==ifStatement) {
                st->setError(error, Lexem::PDAutomata, Lexem::Header);
                break;
            }
        }
    }
    setCurrentError(error);
//    appendSimpleLine();
    processCorrectThen();
    appendSimpleLine();
}

void PDAutomata::setGarbageSwitchCaseError()
{
    bool pushBackZero = false;
    if (currentContext_.top()==0) {
        currentContext_.pop();
        pushBackZero = true;
    }
    if (currentContext_.top()->last()->conditionals.isEmpty()) {
        AST::ConditionSpec dummyCond;
        dummyCond.condition = AST::ExpressionPtr(new AST::Expression);
        dummyCond.condition->kind = AST::ExprConst;
        dummyCond.condition->baseType.kind = AST::TypeBoolean;
        dummyCond.condition->constant = QVariant(1);
        currentContext_.top()->last()->conditionals << dummyCond;
    }

    AST::StatementPtr switchStatement;
    for (int i=currentContext_.size()-1; i>=0; i--) {
        if (currentContext_[i]->size()>0 && currentContext_[i]->last()->type==AST::StSwitchCaseElse) {
            switchStatement = currentContext_[i]->last();
            break;
        }
    }


    currentContext_.push(&(currentContext_.top()->last()->conditionals.first().body));

    const QString error = _("Garbage between switch..case");

    for (int i=0; i<source_.size(); i++) {
        Statement * st = source_.at(i);
        if (st->statement==switchStatement) {
            st->setError(error, Lexem::PDAutomata, Lexem::Header);
            switchStatement->headerErrorLine = st->data.at(0)->lineNo;
            switchStatement->headerError = error;
            break;
        }
    }

    setCurrentError(error);
    appendSimpleLine();
    currentContext_.pop();
    if (pushBackZero) {
        currentContext_.push(0);
    }
}

void PDAutomata::setTooManyErrors()
{
    if (currentPosition_<source_.size())
        source_[currentPosition_]->indentRank = QPoint(-100, 0);
    for (int i=currentPosition_; i<source_.size()-1; i++) {
        source_[i]->setError(_("Too many errors"), Lexem::PDAutomata, Lexem::AsIs);
    }
}

void PDAutomata::setCorrespondingIfBroken()
{
    AST::StatementPtr st;
    int position = currentPosition_;
    int deep = 0;
    for ( ; position >= 0; position --) {
        Statement * test = source_[position];
        if (test->type == LxPriAlgBegin
                || test->type == LxPriAlgHeader
                || test->type == LxPriModule
                )
        {
            break;  // Reached start of more complex block
        }
        else if (!test->hasError()) {
            if (test->type == LxPriFi) {
                deep ++;
            }
            else if (test->type == LxPriIf || test->type == LxPriSwitch) {
                deep --;
                if (deep < 0) {
                    st = findASTStatementBySourceStatement(test);
                    break;
                }
            }
        }

    }
    if (st) {
        st->type = AST::StError;
        st->error = _("Broken if statement");
        for (int i=0; i<source_.size(); i++) {
            if ( source_[i]->statement==st ) {
                for (int a=0; a<source_[i]->data.size(); a++) {
                    source_[i]->data[a]->error = _("Broken if statement");
                    source_[i]->data[a]->errorStage = AST::Lexem::PDAutomata;
                }

                break;
            }
        }
    }
}

AST::StatementPtr PDAutomata::findASTStatementBySourceStatement(const Statement *st) const
{
    AST::StatementPtr result;
    for (int i=0; i<currentContext_.size(); i++) {
        QList<AST::StatementPtr> * contextItem = currentContext_[i];
        for (int j=0; j<contextItem->size(); j++) {
            AST::StatementPtr astStatement = contextItem->at(j);
            if (astStatement->lexems.size() == st->data.size()) {
                bool allMatch = st->data.size() > 0;
                for (int k=0; k<st->data.size(); k++) {
                    const Lexem * a = st->data[k];
                    const Lexem * b = astStatement->lexems[k];
                    allMatch = allMatch && a==b;
                }
                if (allMatch) {
                    result = astStatement;
                    break;
                }
            }
            if (result) break;
        }
        if (result) break;
    }
    return result;
}

//void PDAutomata::setCorrespondingIfBroken()
//{
//    AST::Statement * st = 0;
//    for (int i=currentContext_[currentContext_.size()-1]->size()-1; i>=0; i--) {
//        if (currentContext_[currentContext_.size()-1]->at(i)->type==AST::StIfThenElse
//                ||
//                currentContext_[currentContext_.size()-1]->at(i)->type==AST::StSwitchCaseElse
//                ) {
//            st = currentContext_[currentContext_.size()-1]->at(i);
//            break;
//        }
//    }
//    if (!st) {
//        // Try to find at top-level
//        for (int i=currentContext_.size()-1; i>=0; i--) {
//            if (currentContext_[i]->size()>0 &&
//                    ( currentContext_[i]->last()->type==AST::StIfThenElse ||
//                      currentContext_[i]->last()->type==AST::StSwitchCaseElse )
//                    ) {
//                st = currentContext_[i]->last();
//                break;
//            }
//        }
//    }
//    if (st) {
//        st->type = AST::StError;
//        st->error = _("Broken if statement");
//        for (int i=0; i<source_.size(); i++) {
//            if ( source_[i]->statement==st ) {
//                for (int a=0; a<source_[i]->data.size(); a++) {
//                    source_[i]->data[a]->error = _("Broken if statement");
//                    source_[i]->data[a]->errorStage = AST::Lexem::PDAutomata;
//                }

//                break;
//            }
//        }
//    }
//}


void PDAutomata::setExtraOpenKeywordError(const QString &kw)
{
    bool appendLine = true;
    if (kw==QString::fromUtf8("если")) {
        setCurrentIndentRank(0,0);
        setCurrentError(_("Extra 'if'"));
    }
    else if (kw==QString::fromUtf8("то")) {
        setCurrentIndentRank(-1,-1);
        QString err = _("Extra 'then'");
        int a = currentPosition_ + 1;
        bool fiFound = false;
        while (a<source_.size()) {
            if (source_[a]->type==LxPriFi && !source_[a]->hasError()) {
                fiFound = true;
                break;
            }
            else if (source_[a]->type==LxPriEndModule || source_[a]->type==LxPriAlgEnd)
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
        setCurrentIndentRank(0,0);
        setCurrentError(_("Extra 'begin'"));
        if (currentAlgorhitm_) {
            AST::StatementPtr st = AST::StatementPtr(new AST::Statement);
            st->type = AST::StError;
            st->lexems = source_[currentPosition_]->data;
            st->error = _("Extra 'begin'");
            currentAlgorhitm_->impl.body.append(st);
        }
    }
    else if (kw==QString::fromUtf8("иначе")) {
        setCurrentIndentRank(-1,-1);
        QString err = _("No 'end' after 'else'");
//        int a = currentPosition + 1;
        bool elseFoundForward = false;
        bool elseFoundBackward = false;
        for (int i=currentPosition_+1; i<source_.size(); i++) {
            if (source_[i]->type==LxPriFi || source_[i]->type==LxPriAlgEnd)
                break;
            if (source_[i]->type==LxPriElse) {
                elseFoundForward =true;
                break;
            }
        }
        for (int i=currentPosition_-1; i>=0; i--) {
            if (source_[i]->type==LxPriElse || source_[i]->type==LxPriAlgBegin)
                break;
            if (source_[i]->type==LxPriElse) {
                elseFoundBackward =true;
                break;
            }
        }
        if (elseFoundForward || elseFoundBackward) {
            err = _("Extra 'else'");
        }
        else {
            // Append dummy 'else' block
            if (currentContext_.size()>1
                    && currentContext_[currentContext_.size()-2]->size()>0
                    && (
                        currentContext_[currentContext_.size()-2]->last()->type==AST::StSwitchCaseElse
                        ||
                        currentContext_[currentContext_.size()-2]->last()->type==AST::StIfThenElse
                       )
                    )
            {
                appendLine = false;
                AST::ConditionSpec cond;
                cond.lexems = source_[currentPosition_]->data;
                cond.condition.clear();
                AST::StatementPtr errStatement = AST::StatementPtr(new AST::Statement());
                errStatement->type = AST::StError;
                errStatement->skipErrorEvaluation = false;
                errStatement->error = err;
                errStatement->lexems = source_[currentPosition_]->data;
                cond.body.append(errStatement);
                currentContext_[currentContext_.size()-2]->last()->conditionals.push_back(cond);
                source_.at(currentPosition_)->statement = errStatement;
            }
        }
        setCurrentError(err);
    }
    else if (kw==QString::fromUtf8("исп")) {
        setCurrentIndentRank(0,0);
        setCurrentError(_("Extra 'module'"));
    }
    source_.at(currentPosition_)->alg = currentAlgorhitm_;
    source_.at(currentPosition_)->mod = currentModule_;
    if (appendLine) {
        appendSimpleLine();
        source_.at(currentPosition_)->statement = currentContext_.top()->last();
    }
}

void PDAutomata::setExtraCloseKeywordError(const QString &kw)
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
        int a = currentPosition_-1;
        while (a>=0) {
            if (source_[a]->hasError() &&
                    (source_[a]->type==LxPriIf || source_[a]->type==LxPriSwitch)
                    ) {
                err = _("No 'end' after 'else'");
                break;
            }
            else if (source_[a]->type==LxPriFi)
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
    if (currentAlgorhitm_) {
        appendSimpleLine();
    }
}

} // namespace KumirAnalizer
