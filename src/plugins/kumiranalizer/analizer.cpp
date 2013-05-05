#include "analizer.h"
#include "analizer_p.h"
#include "interfaces/error.h"
#include "interfaces/lexemtype.h"
#include "lexer.h"
#include "pdautomata.h"
#include "syntaxanalizer.h"
#include "errormessages/errormessages.h"
#include "kumiranalizerplugin.h"
#include "stdlibmodules.h"

using namespace Shared;

namespace KumirAnalizer {

QLocale::Language AnalizerPrivate::nativeLanguage = QLocale::Russian;

void Analizer::setSourceLanguage(const QLocale::Language &language)
{
    Lexer::setLanguage(language);
    AnalizerPrivate::nativeLanguage = language;
}

void Analizer::setModuleAlwaysAvailable(const QString &moduleName)
{
    if (moduleName==QString::fromAscii("Files"))
        AnalizerPrivate::AlwaysAvailableModulesName.append(QString::fromUtf8("Файлы"));
    if (moduleName==QString::fromAscii("Strings"))
        AnalizerPrivate::AlwaysAvailableModulesName.append(QString::fromUtf8("Строки"));
}

Analizer::Analizer(KumirAnalizerPlugin * plugin) :
    QObject(plugin)
{
    d = new AnalizerPrivate(plugin, this);
}

QString Analizer::sourceText() const
{
    return d->sourceText.join("\n")+"\n";
}

void Analizer::setSourceDirName(const QString &dirName)
{
    d->analizer->setSourceDirName(dirName);
}

LineProp Analizer::lineProp(const QString &text) const
{
    QList<Lexem*> lexems;
    d->lexer->splitIntoLexems(text, lexems, d->gatherExtraTypeNames());
    LineProp lp(text.length(), LxTypeEmpty);
    bool delimFound = false;
    for (int i=0; i<lexems.size(); i++) {
        Lexem * lx = lexems[i];
        if (lx->type==LxTypeName) {
            if (algorhitmNames().contains(lx->data.trimmed())) {
                lx->type = LxNameAlg;
            }
            else if (moduleNames().contains(lx->data.trimmed())) {
                lx->type = LxNameModule;
            }
            else if (d->lexer->baseTypeByClassName(lx->data.trimmed())!=AST::TypeNone) {
                lx->type = LxNameClass;
            }
            else if (i>0) {
                if (lexems[0]->type==LxPriAlgHeader && !delimFound)
                    lx->type = LxNameAlg;
                else if (lexems[0]->type==LxPriModule && !delimFound)
                    lx->type = LxNameModule;
            }
        }
        else if (i>0 && !(lx->type & LxTypeName)) {
            delimFound = true;
        }
        for (int k=0; k<lx->length; k++) {
            int index = lx->linePos+k;
            if (index<lp.size())
                lp[index] = lx->type;
        }
    }
    for (int i=0; i<lexems.size(); i++)
        delete lexems[i];
    return lp;
}

QStringList Analizer::algorhitmNames() const
{
    QStringList result;
    for (int i=0; i<d->ast->modules.size(); i++) {
        if (d->ast->modules[i]->header.enabled)
        for (int j=0; j<d->ast->modules[i]->impl.algorhitms.size(); j++) {
            result << d->ast->modules[i]->impl.algorhitms[j]->header.name;
        }
    }
    return result;
}

QStringList Analizer::moduleNames() const
{
    QStringList result;
    for (int i=0; i<d->ast->modules.size(); i++) {
        result << d->ast->modules[i]->header.name;
    }
    return result;
}

AnalizerPrivate::AnalizerPrivate(KumirAnalizerPlugin * plugin,
                                 Analizer *qq)
{
    hiddenBaseLine = -1;
    q = qq;
    ast = new AST::Data();
    lexer = new Lexer(q);
    pdAutomata = new PDAutomata(q);
    analizer = new SyntaxAnalizer(lexer, AlwaysAvailableModulesName, q);
    builtinModules.resize(16);
    ActorInterface * stdFunct = new StdLibModules::RTL;
    builtinModules[0] = stdFunct;
    createModuleFromActor(stdFunct, 0xF0);
    ActorInterface * filesFunct = new StdLibModules::Files;
    builtinModules[1] = filesFunct;
    createModuleFromActor(filesFunct, 0xF1);
    ActorInterface * stringsFunct = new StdLibModules::Strings;
    builtinModules[2] = stringsFunct;
    createModuleFromActor(stringsFunct, 0xF2);
    QList<ExtensionSystem::KPlugin*> actors = plugin->loadedPlugins("Actor*");
    foreach (QObject *o, actors) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(o);
        if (actor) {
            createModuleFromActor(actor, 0);
        }
    }
}


void AnalizerPrivate::setHiddenBaseLine(int lineNo)
{
    int diff = lineNo - hiddenBaseLine;
    foreach (Statement * st, teacherStatements) {
        foreach (Lexem * lx, st->data) {
            lx->lineNo += diff;
        }
    }
    hiddenBaseLine = lineNo;
}

void AnalizerPrivate::setHiddenText(const QString &text, int baseLineNo)
{
    teacherText = text;
    hiddenBaseLine = baseLineNo;

    // Clean old teacher algorhitms
    foreach (Statement * st, teacherStatements) {
        foreach (Lexem * lx, st->data) {
            delete lx;
        }
        delete st;
    }

    teacherStatements.clear();

    for (int i=0; i<ast->modules.size(); i++) {

    }

    // Compile teacher algorhitms
    lexer->splitIntoStatements(text.split("\n"), baseLineNo, teacherStatements, gatherExtraTypeNames());

    // Build structure
    pdAutomata->init(true, teacherStatements, ast, 0);
    pdAutomata->process();
    pdAutomata->postProcess();

    // Build tables for hidden algorhitms
    analizer->init(teacherStatements, ast, 0);
    analizer->buildTables(false);

    // Do complete semantic analisys
    QList<Statement*> statementsToAnalize = statements+teacherStatements;

    foreach (Statement * st, statements) {
        foreach (AST::Variable * var, st->variables) {
            removeAllVariables(var);
        }
    }

    analizer->init(statementsToAnalize, ast, 0);
    analizer->processAnalisys();
    analizer->syncStatements();

}

AnalizerPrivate::~AnalizerPrivate()
{
    delete lexer;
    delete pdAutomata;
    delete ast;
    for (int i=0; i<statements.size(); i++) {
        Statement * statement = statements[i];
        for (int j=0; j<statement->data.size(); j++) {
            Lexem * lx = statement->data.at(j);
            delete lx;
        }
        delete statement;
    }
    for (int i=0; i<teacherStatements.size(); i++) {
        Statement * statement = teacherStatements[i];
        for (int j=0; j<statement->data.size(); j++) {
            Lexem * lx = statement->data.at(j);
            delete lx;
        }
        delete statement;
    }
}

Analizer::~Analizer()
{
    delete d;
}

void Analizer::changeSourceText(const QList<ChangeTextTransaction> & changes)
{
    for (int i=0; i<changes.size(); i++) {
        d->compileTransaction(changes[i]);
    }
}

Shared::TextAppend Analizer::closingBracketSuggestion(int lineNo) const
{
    for (int i=0; i<d->statements.size(); i++) {
        const Statement * st = d->statements.at(i);
        if (st->data.size()>0 && st->data.last()->lineNo==lineNo) {
            if (st->suggestedClosingBracket.first.length()>0)
                return st->suggestedClosingBracket;
        }
    }
    return QPair<QString,quint32>("",0);
}

QStringList Analizer::importModuleSuggestion(int lineNo) const
{
    for (int i=0; i<d->statements.size(); i++) {
        const Statement * st = d->statements.at(i);
        if (st->data.size()>0 && st->data.last()->lineNo==lineNo) {
            return st->suggestedImportModuleNames;
        }
    }
    return QStringList();
}


void AnalizerPrivate::compileTransaction(const ChangeTextTransaction & changes)
{
//    qDebug() << changes.removedLineNumbers;
//    qDebug() << changes.newLines;
//    qDebug() << lexer->metaObject()->className();
//    qDebug() << pdAutomata->metaObject()->className();
//    qDebug() << analizer->metaObject()->className();
    for (int i=0; i<statements.size(); i++) {
        Statement * st = statements.at(i);
        st->suggestedClosingBracket.first.clear();
        st->suggestedImportModuleNames.clear();
    }
    QList<Statement*> removedStatements;
    QList<Statement*> newStatements;
    QList<int> removedLineNumbers = changes.removedLineNumbers.toList();
    qSort(removedLineNumbers);
    QStringList newLines = changes.newLines;
    int insertPos = statements.size();
    int lineStart = 0;
    int lineEnd = 99999999;
    if (!removedLineNumbers.isEmpty()) {
        if (removedLineNumbers.size()>1 || removedLineNumbers[0]!=999999) {
            // We assume this set is sorted in ascending order
            lineStart = removedLineNumbers.first();
            lineEnd = removedLineNumbers.last();
        }
        else {
            // There was a flag: remove all text
        }
    }
    int it = 0;
    while (it<statements.size() && !removedLineNumbers.isEmpty()) {
        Statement * st = statements[it];
        bool remove = false;
        bool insert = false;
        foreach (const Lexem * lx, st->data) {
            if ( (lx->lineNo>=lineStart) && (insertPos==statements.size())) {
                insert = true;
            }
            if (lx->lineNo>=lineStart && lx->lineNo<=lineEnd) {
                remove = true;
                break;
            }
        }
        if (remove) {
            removedStatements << st;
            statements.removeAt(it);

        }
        if (insert) {
            insertPos = it;
        }
        if (!remove) {
            it++;
        }
    }

    lineStart = qMin(sourceText.size(), lineStart);

    QStringList newSourceText;
    if (!removedLineNumbers.isEmpty()) {
        newSourceText = sourceText.mid(0, lineStart) + newLines;
        if (lineEnd+1<sourceText.size()) {
            newSourceText += sourceText.mid(lineEnd+1);
        }
    }
    else {
        newSourceText = sourceText + newLines;
        insertPos = statements.size();
        lineStart = sourceText.size();
    }
    sourceText = newSourceText;



    QList<Statement*> preprocessorStatements;
    lexer->splitIntoStatements(sourceText, lineStart, preprocessorStatements, QStringList());

    QStringList extraTypeNames;

    foreach (Statement * st, preprocessorStatements) {
        if (st->data.size()>1 && st->data.at(0)->type==LxPriImport) {
            if (st->data.at(1)->type!=LxConstLiteral && st->data.at(1)->type!=LxPriImport) {
                const QString moduleName = st->data.at(1)->data;
                foreach (const AST::Module * pmod, ast->modules) {
                    if (pmod->header.type==AST::ModTypeExternal &&
                            pmod->header.name==moduleName)
                    {
                        foreach (const AST::Type ptype, pmod->header.types) {
                            const QString typeName = ptype.name;
                            if (!extraTypeNames.contains(typeName))
                                extraTypeNames.append(typeName);
                        }
                    }
                }
            }
        }
    }

    foreach (Statement * st, removedStatements) {
        if (st->type==LxPriImport) {
            if (st->data.size()>1 &&
                    st->data.at(1)->type!=LxConstLiteral &&
                    st->data.at(1)->type!=LxPriImport)
            {
                const QString moduleName = st->data.at(1)->data;
                foreach (AST::Module * rmod, ast->modules) {
                    if (rmod->header.name==moduleName) {
                        rmod->header.enabled = false;
                    }
                }
            }
        }
    }

    lexer->splitIntoStatements(newLines, lineStart, newStatements, extraTypeNames);

    AnalizerPrivate::AnalizeSubject subjByOld =
            analizeSubject(removedStatements);

    AnalizerPrivate::AnalizeSubject subjByNew =
            analizeSubject(newStatements);

    AnalizerPrivate::AnalizeSubject subject = subjByOld * subjByNew;

    int tailLineNumbersOffset = newLines.size() - removedLineNumbers.size();

    for (int i=insertPos; i<statements.size(); i++) {
        Statement * st = statements[i];
        foreach (Lexem * lx, st->data) {
            lx->lineNo += tailLineNumbersOffset;
        }
    }

    for (int i=0 ; i<newStatements.size(); i++) {
        statements.insert(insertPos, newStatements[i]);
        insertPos ++;
    }


    if (subject==SubjWholeText) {
        statements.clear();
        lexer->splitIntoStatements(sourceText, 0, statements, extraTypeNames);
    }

    if (removedStatements.size()>0 || newStatements.size()>0)
        doCompilation(subject, removedStatements, newStatements, statements, insertPos);

    for (int i=0; i<removedStatements.size(); i++) {
        Statement * st = removedStatements[i];
        if (st) {
            for (int j=0; j<st->data.size(); j++) {
                Lexem * lx = st->data[j];
                if (lx) {
                    delete lx;
                    st->data[j] = 0;
                }
            }
            removedStatements[i] = 0;
            delete st;
        }
    }
}

QStringList AnalizerPrivate::AlwaysAvailableModulesName;

void AnalizerPrivate::createModuleFromActor(const Shared::ActorInterface * actor, quint8 forcedId)
{
    AST::Module * mod = new AST::Module();
    mod->builtInID = forcedId;
    mod->header.type = AST::ModTypeExternal;
    mod->header.name = actor->name();
    mod->header.enabled = true;
    ast->modules << mod;
    const Shared::ActorInterface::TypeList typeList = actor->typeList();
    for (int i=0; i<typeList.size(); i++) {
        typedef Shared::ActorInterface AI;
        AI::CustomType ct = typeList[i];
        AST::Type tp;
        tp.name = ct.first;
        tp.moduleName = actor->name();
        AI::Record record = ct.second;
        for (int j=0; j<record.size(); j++) {
            AI::Field field = record[j];
            AI::FieldType ft = field.second;
            AST::Type afield;
            if (ft==AI::Int)
                afield.kind = AST::TypeInteger;
            else if (ft==AI::Real)
                afield.kind = AST::TypeReal;
            else if (ft==AI::Bool)
                afield.kind = AST::TypeBoolean;
            else if (ft==AI::Char)
                afield.kind = AST::TypeCharect;
            else if (ft==AI::String)
                afield.kind = AST::TypeString;
            tp.userTypeFields << AST::Field(field.first, afield);
        }
        tp.kind = AST::TypeUser;
        mod->header.types << tp;
    }
    const QStringList funcList = actor->funcList();
    for (int i=0; i<funcList.size(); i++) {
        AST::Algorhitm * alg = new AST::Algorhitm;
        alg->header.implType = AST::AlgorhitmExternal;
        alg->header.external.moduleName = actor->name();
        alg->header.external.id = i;
        QList<Statement*> sts;
        lexer->splitIntoStatements(QStringList() << funcList[i], -1, sts, gatherExtraTypeNames());
        Q_ASSERT_X(sts.size()==1
                   , "AnalizerPrivate::createModuleFromActor"
                   , QString("Algorhitm %1 in module %2 has syntax error")
                   .arg(funcList[i])
                   .arg(actor->name()).toLocal8Bit().data());
        alg->impl.headerLexems = sts[0]->data;
        sts[0]->alg = alg;
        sts[0]->mod = mod;
        analizer->init(sts, ast, alg);
        analizer->buildTables(true);
        foreach (const Lexem *lx, sts[0]->data) {
            if (!lx->error.isEmpty()) {
                Q_ASSERT_X(sts.size()==1
                           , "AnalizerPrivate::createModuleFromActor"
                           , QString("Algorhitm %1 in module %2 has syntax error")
                           .arg(funcList[i])
                           .arg(actor->name()).toLocal8Bit().data());
            }
            delete lx;
        }
        delete sts[0];
    }
    mod->header.enabled = AlwaysAvailableModulesName.contains(mod->header.name);
}

extern AnalizerPrivate::AnalizeSubject operator * ( const AnalizerPrivate::AnalizeSubject &first
                                                   , const AnalizerPrivate::AnalizeSubject &second )
{
    if (first==AnalizerPrivate::SubjWholeText || second==AnalizerPrivate::SubjWholeText)
        return AnalizerPrivate::SubjWholeText;
    if (first==AnalizerPrivate::SubjAlgorhtitm || second==AnalizerPrivate::SubjAlgorhtitm)
        return AnalizerPrivate::SubjAlgorhtitm;
    return AnalizerPrivate::SubjStatements;

}


AnalizerPrivate::AnalizeSubject AnalizerPrivate::analizeSubject(const QList<Statement*> &statements) const
{
    //    QList<Shared::LexemType> lexemTypes;
    //    int startLineNo = statements.isEmpty()? 0 : statements[0].data.first()->lineNo;
//    AnalizeSubject result = SubjStatements;
    AnalizeSubject result = SubjAlgorhtitm;
    foreach (const Statement * st, statements) {
        LexemType lt = st->type;
        if ( ( lt == LxPriImport)
                || ( lt == LxPriModule)
                || ( lt == LxPriEndModule)
                || ( lt == LxPriAlgHeader)
                || ( lt == LxPriAlgBegin)
                || ( lt == LxPriAlgEnd)
                || ( lt == LxPriIf)
                || ( lt == LxPriThen)
                || ( lt == LxPriElse)
                || ( lt == LxPriFi)
                || ( lt == LxPriSwitch)
                || ( lt == LxPriCase)
                || ( lt == LxPriLoop)
                || ( lt == LxPriEndLoop)
                || ( lt == LxPriPre)
                || ( lt == LxPriPost)
                || ( lt == LxPriImport)
                )
        {
            return SubjWholeText;
        }
        if (lt & LxNameClass) {
            if (findAlgorhitmByPos(ast, st->data.first()->lineNo)) {
                if (result==SubjAlgorhtitm)
                    return SubjWholeText; // more that one algorhitm affected
                result = SubjAlgorhtitm;
            }
            else {
                // Global variable
                return SubjWholeText;
            }
        }
    }
    return result;
}

AST::Algorhitm * AnalizerPrivate::findAlgorhitmByPos(AST::Data * data, int pos)
{
    if (pos==-1) {
        return 0;
    }
    foreach (AST::Module * mod, data->modules) {
        foreach (AST::Algorhitm * alg, mod->impl.algorhitms) {
            QList<Lexem*> begin = alg->impl.beginLexems;
            QList<Lexem*> end = alg->impl.endLexems;
            if (!begin.isEmpty() && !end.isEmpty()) {
                int algBegin = begin.first()->lineNo;
                int algEnd = end.first()->lineNo;
                if (algBegin!=-1 && algEnd!=-1) {
                    if (pos>algBegin && pos<algEnd) {
                        return alg;
                    }
                }
            }
        }
    }

    return 0;
}



QList<Error> Analizer::errors() const
{
    QList<Error> result;
    QList<Statement*> all = d->statements + d->teacherStatements;
    for (int i=0; i<all.size(); i++) {
        foreach (const Lexem * lx, all[i]->data) {
            if (!lx->error.isEmpty()) {
                Error err;
                err.line = lx->lineNo;
                err.start = lx->linePos;
                err.len = lx->length;
                err.code = ErrorMessages::message(
                            "KumirAnalizer",
                            AnalizerPrivate::nativeLanguage,
                            lx->error
                            );
                if (result.size()>0 && result.last().line==err.line && result.last().code==err.code) {
                    Error prev = result.last();
                    result.pop_back();
                    prev.len = (err.start+err.len-prev.start);
                    result << prev;
                }
                else {
                    result << err;
                }
            }
        }
    }
    return result;
}

QList<LineProp> Analizer::lineProperties() const
{
    QList<LineProp> result;
    QStringList lines = d->sourceText + d->teacherText.split("\n");
    for (int i=0; i<lines.size(); i++) {
        result << LineProp(lines[i].size(), LxTypeEmpty);
    }

    result << LineProp(0, LxTypeEmpty);
    QList<Statement*> all = d->statements + d->teacherStatements;

    for (int i=0; i<all.size(); i++) {
        foreach (const Lexem * lx, all[i]->data) {
            for (int j=lx->linePos; j<lx->linePos+lx->length; j++) {
                unsigned int value = lx->type;
                const unsigned int errorMask = LxTypeError;
                if (!lx->error.isEmpty()) {
                    value = value | errorMask;
                }
                const int lineNo = lx->lineNo;
                if (lineNo>=0 && lineNo < result.size() && j>=0 && j<result[lineNo].size())
                    result[lineNo][j] = LexemType(value);
            }
        }
    }
    return result;
}

QStringList Analizer::imports() const
{
    QStringList result;
    for (int i=0; i<d->ast->modules.size(); i++) {
        for (int j=0; j<d->ast->modules[i]->header.uses.size(); j++) {
            const QString import = d->ast->modules[i]->header.uses.toList()[j];
            if (!result.contains(import)) {
                result << import;
            }
        }
    }
    return result;
}

QList<QPoint> Analizer::lineRanks() const
{
    QList<QPoint> result;
    QStringList lines = d->sourceText + d->teacherText.split("\n");
    for (int i=0; i<lines.size(); i++) {
        result << QPoint(0,0);
    }
    QList<Statement*> all = d->statements + d->teacherStatements;
    for (int i=0; i<all.size(); i++) {
        Q_ASSERT (!all[i]->data.isEmpty());
        const Lexem * lx = all[i]->data.first();
        const int lineNo = lx->lineNo;
        const QPoint rank = all[i]->indentRank;
        if (lineNo>=0 && lineNo<result.size()) {
            if (result[lineNo]==QPoint(0,0))
                result[lineNo] = rank;
            else {
                QPoint p(result[lineNo].x(),
                         result[lineNo].y()+rank.x()+rank.y());
                result[lineNo] = p;
            }
        }
    }
    return result;
}

bool findAlgorhitmBounds( const QList<Statement*> & statements
                         , AST::Algorhitm * alg
                         , int &beginIndex
                         , int &endIndex)
{
    Lexem * lxFirst = alg->impl.headerLexems.isEmpty()
            ? alg->impl.beginLexems.first()
            : alg->impl.headerLexems.first();
    Lexem * lxLast = alg->impl.endLexems.first();
    Statement * begin = 0;
    Statement * end = 0;
    foreach (Statement * st, statements) {
        if (st->data.first()==lxFirst) {
            begin = st;
        }
        else if (st->data.first()==lxLast) {
            end = st;
        }
        if (begin && end) {
            break;
        }
    }
    if (begin && end) {
        beginIndex = statements.indexOf(begin);
        endIndex = statements.indexOf(end);
    }
    else {
        beginIndex = endIndex = -1;
    }
    return begin && end;
}

bool AnalizerPrivate::findInstructionsBlock(
    AST::Data *data
    , const QList<Statement*> statements
    , LAS &lst
    , int &begin
    , int &end
    , AST::Module *&mod
    , AST::Algorhitm *&alg)
{
    if (statements.isEmpty())
        return false;
    bool found = false;
    Statement * first = statements.first();
    Statement * last = statements.last();
    foreach (AST::Module * module, data->modules) {
        for (int i=0; i<module->impl.initializerBody.size(); i++)
        {
            AST::Statement * st = module->impl.initializerBody[i];
            if (st==first->statement) {
                mod = module;
                alg = 0;
                lst = &(module->impl.initializerBody);
                if (begin!=-999)
                    begin = i;
                found = true;
            }
            if (st==last->statement) {
                end = i+1;
            }
        }
        if (!found) {
            foreach (AST::Algorhitm * algorhitm, module->impl.algorhitms) {
                for (int i=0; i<algorhitm->impl.body.size(); i++) {
                    AST::Statement * st = algorhitm->impl.body[i];
                    if (st==first->statement) {
                        mod = module;
                        alg = algorhitm;
                        lst = &(algorhitm->impl.body);
                        if (begin!=-999)
                            begin = i;
                        found = true;
                    }
                    if (st==last->statement) {
                        end = i + 1;
                    }
                }
                if (begin==-999) {
                    if (last->data[0] == algorhitm->impl.endLexems[0]) {
                        found = true;
                        mod = module;
                        alg = algorhitm;
                        begin = end = algorhitm->impl.body.size();
                        lst = &(algorhitm->impl.body);
                        found = true;
                    }
                }
            }
        }
    }
    return found;
}



bool AnalizerPrivate::findInstructionsBlock(
    AST::Data *data
    , const QList<Statement *> statements
    , int pos
    , LAS &lst, int &outPos
    , AST::Module *&mod
    , AST::Algorhitm *&alg
    )
{
    if (statements.isEmpty())
        return false;
    int searchByPos;
//    if (pos==0)
//        searchByPos = pos + 1;
    if (pos==statements.size())
        searchByPos = pos - 1;
    else
        searchByPos = pos;
    QList<Statement*> nearbyStatements = QList<Statement*>() << statements[searchByPos];
    int dummy = -999;
    return findInstructionsBlock(data, nearbyStatements, lst, dummy, outPos, mod, alg);
}

void AnalizerPrivate::removeAllVariables(AST::Variable *var)
{
    foreach (AST::Module * mod, ast->modules) {
        mod->impl.globals.removeAll(var);
        foreach (AST::Algorhitm * alg, mod->impl.algorhitms) {
            alg->impl.locals.removeAll(var);
        }
    }
    delete var;
}

void AnalizerPrivate::doCompilation(AnalizeSubject whatToCompile
                                    , QList<Statement*> & oldStatements
                                    , QList<Statement*> & newStatements
                                    , QList<Statement*> & allStatements
                                    , int whereInserted
                                    )
{
    if (qApp->applicationVersion()<"2.1.0-release")
        whatToCompile = SubjWholeText; // Not tested well yet
    foreach (Statement * st, oldStatements) {
        foreach (AST::Variable * var, st->variables) {
            removeAllVariables(var);
        }
    }

    if (ast->modules.isEmpty())
        whatToCompile = SubjWholeText;
//    if (whatToCompile==SubjStatements)
//        qDebug() << "Analize some statements";
//    else if (whatToCompile==SubjAlgorhtitm)
//        qDebug() << "Analize one algorhitm";
//    else
//        qDebug() << "Analize whole text";
    QList<Statement*> analizingStatements;
    AST::Algorhitm * alg = 0;
    if (whatToCompile==SubjAlgorhtitm) {
        Q_ASSERT(!newStatements.isEmpty() || !oldStatements.isEmpty());
        Statement * firstStatement = 0;
        if (newStatements.isEmpty()) {
            firstStatement = oldStatements.first();
        }
        else {
            firstStatement = newStatements.first();
        }
        Q_ASSERT(!firstStatement->data.isEmpty());
        const Lexem * lx = firstStatement->data.first();
        const int linePos = lx->lineNo;
        alg = findAlgorhitmByPos(ast, linePos);
        if (!alg) {
            whatToCompile = SubjWholeText;
        }
    }
    if (whatToCompile==SubjWholeText) {
        foreach (Statement * st, allStatements) {
            foreach (Lexem * lx, st->data) {
                if (lx->errorStage!=AST::Lexem::Lexer)
                    lx->error = "";
            }
        }
        analizingStatements = allStatements;
        pdAutomata->init(false, analizingStatements, ast, alg);
        pdAutomata->process();
        pdAutomata->postProcess();
    }
    else if (alg) {
        int algBeginIndex = -1, algEndIndex = -1;
        if (findAlgorhitmBounds(allStatements, alg, algBeginIndex, algEndIndex)) {
            for (int i=algBeginIndex; i<=algEndIndex; i++) {
                foreach (Lexem *olx, allStatements[i]->data) {
                    if (olx->errorStage!=AST::Lexem::Lexer)
                        olx->error = "";
                }
                analizingStatements << allStatements[i];
            }
        }
        pdAutomata->init(false, analizingStatements, ast, alg);
        pdAutomata->process();
        pdAutomata->postProcess();
    }
    else {
        LAS lst;
        int begin = -1, end = -1;
        int insertPos;
        AST::Module * module = 0;
        AST::Algorhitm * algorhitm = 0;

        if (findInstructionsBlock(ast, oldStatements, lst, begin, end, module, algorhitm)) {
            int removeCount = end-begin;
            for (int i=0; i<removeCount; i++) {
                AST::Statement * aST = lst->at(begin+i);
                delete aST;
                lst->removeAt(begin+i);
            }
            insertPos = begin;
            for (int i=0; i<newStatements.size(); i++) {
                AST::Statement * instruction
                        = PDAutomata::createSimpleAstStatement(newStatements[i]);
                newStatements[i]->mod = module;
                newStatements[i]->alg = algorhitm;
                newStatements[i]->statement = instruction;
                lst->insert(insertPos, instruction);
                insertPos ++;
            }
        }
        else if (findInstructionsBlock(ast, allStatements, whereInserted, lst, insertPos, module, algorhitm)) {
            for (int i=0; i<newStatements.size(); i++) {
                AST::Statement * instruction
                        = PDAutomata::createSimpleAstStatement(newStatements[i]);
                newStatements[i]->mod = module;
                newStatements[i]->alg = algorhitm;
                newStatements[i]->statement = instruction;
                lst->insert(insertPos, instruction);
                insertPos ++;
            }
        }
        else {
            pdAutomata->init(false, newStatements, ast, alg);
            pdAutomata->process();
            pdAutomata->postProcess();
        }
        analizingStatements = newStatements;
    }
    analizer->init(analizingStatements, ast, alg);
    if (whatToCompile!=SubjStatements)
        analizer->buildTables(false);

    analizer->processAnalisys();
    analizer->syncStatements();
}

const AST::Data * Analizer::abstractSyntaxTree() const
{
    return d->ast;
}

void Analizer::setHiddenText(const QString &text, int baseLineNo)
{
    d->setHiddenText(text, baseLineNo);
}

void Analizer::setHiddenBaseLine(int lineNo)
{
    d->setHiddenBaseLine(lineNo);
}

const AST::Module * Analizer::findModuleByLine(int lineNo) const
{
    if (lineNo==-1)
        return 0;
    for (int i=0; i<d->ast->modules.size(); i++) {
        if (
                (d->ast->modules[i]->header.type==AST::ModTypeUser || d->ast->modules[i]->header.type==AST::ModTypeHidden)
                && d->ast->modules[i]->impl.beginLexems.size()>0
                && d->ast->modules[i]->impl.beginLexems[0]->lineNo>=lineNo
                && d->ast->modules[i]->impl.endLexems.size()>0
                && d->ast->modules[i]->impl.endLexems[0]->lineNo<=lineNo
                )
            return d->ast->modules[i];
    }
    for (int i=0; i<d->ast->modules.size(); i++) {
        if (
                (d->ast->modules[i]->header.type==AST::ModTypeUser || d->ast->modules[i]->header.type==AST::ModTypeHidden)
                && d->ast->modules[i]->header.name.isEmpty()
                )
            return d->ast->modules[i];
    }
    return 0;
}

const AST::Algorhitm * Analizer::findAlgorhitmByLine(const AST::Module *mod, int lineNo) const
{
    if (!mod || lineNo==-1)
        return nullptr;
    for (int i=0; i<mod->impl.algorhitms.size(); i++) {
        const AST::Algorhitm * alg = mod->impl.algorhitms.at(i);
        if (alg->impl.beginLexems.size()==0 || alg->impl.endLexems.size()==0)
            continue;
        const int algBegin = alg->impl.beginLexems.front()->lineNo;
        const int algEnd   = alg->impl.endLexems.last()->lineNo;
        if (algBegin <= lineNo && lineNo <= algEnd)
            return alg;
    }
    return nullptr;
}

QList<Suggestion> Analizer::suggestAutoComplete(int lineNo, const QString &before, const QString &after) const
{
    const AST::Module * mod = findModuleByLine(lineNo);
    const AST::Algorhitm * alg = findAlgorhitmByLine(mod, lineNo);
    QStringList beforeProgram;
    beforeProgram << before;
    QList<Statement*> beforeStatements;
    d->lexer->splitIntoStatements(beforeProgram, 0, beforeStatements, QStringList());
    QList<Lexem*> afterLexems;
    d->lexer->splitIntoLexems(after, afterLexems, QStringList());
    const Statement* lastStatement = beforeStatements.size()>0? beforeStatements.last() : nullptr;
    QList<Suggestion> result = d->analizer->suggestAutoComplete(lastStatement, afterLexems, mod, alg);
    QList<Suggestion> filteredResult;
    for (int i_sugg=0; i_sugg<result.size(); i_sugg++) {
        // filter by space at end
        const Suggestion & s = result.at(i_sugg);
        if (before.endsWith(' ') && !before.trimmed().isEmpty()) {
            // suggest only if suggestion bounds by a keyword
            if (s.kind==Suggestion::SecondaryKeyword ||
                    (lastStatement!=nullptr
                     && lastStatement->data.size()>0 &&
                        (
                        lastStatement->data.last()->type & LxTypePrimaryKwd
                        ||
                        lastStatement->data.last()->type & LxTypeSecondaryKwd
                        )
                     )
                    )
                filteredResult.push_back(s);
        }
        else {
            // regular case -- suggest it (already filtered by name)
            filteredResult.push_back(s);
        }
    }
    foreach ( Statement * st , beforeStatements )
        delete st;
    foreach ( Lexem * lx , afterLexems )
        delete lx;

    QList<Suggestion> cleanResult;
        // Remove duplicates
    foreach ( const Suggestion & s , filteredResult) {
        bool found = false;
        foreach ( const Suggestion & ss , cleanResult ) {
            if (s.value.trimmed()==ss.value.trimmed()) {
                found = true;
                break;
            }
        }
        if (!found) {
            cleanResult.append(s);
        }
    }

    return cleanResult;
}


QStringList AnalizerPrivate::gatherExtraTypeNames() const
{
    QStringList result;
    if (ast) {
        for (int i=0; i<ast->modules.size(); i++) {
            AST::Module * mod = ast->modules[i];
            if (mod && mod->header.enabled) {
                for (int j=0; j<mod->header.types.size(); j++) {
                    AST::Type tp = mod->header.types[j];
                    result << tp.name;
                }
            }
        }
    }
    return result;
}

} // namespace KumirAnalizer
