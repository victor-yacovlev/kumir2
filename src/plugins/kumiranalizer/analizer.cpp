#include "analizer.h"
#include "analizer_p.h"
#include "interfaces/error.h"
#include "interfaces/lexemtype.h"
#include "lexer.h"
#include "pdautomata.h"
#include "syntaxanalizer.h"
#include "errormessages/errormessages.h"
#include "kumiranalizerplugin.h"

using namespace Shared;

namespace KumirAnalizer {

QLocale::Language AnalizerPrivate::nativeLanguage = QLocale::Russian;

void Analizer::setSourceLanguage(const QLocale::Language &language)
{
    Lexer::setLanguage(language);
    AnalizerPrivate::nativeLanguage = language;
}

Analizer::Analizer(KumirAnalizerPlugin * plugin) :
    QObject(plugin)
{
    d = new AnalizerPrivate(plugin, this);
}

LineProp Analizer::lineProp(const QString &text) const
{
    QList<Lexem*> lexems;
    d->lexer->splitIntoLexems(text, lexems);
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

AnalizerPrivate::AnalizerPrivate(KumirAnalizerPlugin * plugin, Analizer *qq)
{
    q = qq;
    ast = new AST::Data();
    lexer = new Lexer(q);
    pdAutomata = new PDAutomata(q);
    analizer = new SyntaxAnalizer(lexer, q);
    ActorInterface * stdFunct = qobject_cast<ActorInterface*>(plugin->myDependency("st_funct"));
    Q_ASSERT_X(stdFunct, "constructor AnalizerPrivate", "Can't' load st_func module");
    createModuleFromActor(stdFunct);
    QList<ExtensionSystem::KPlugin*> actors = plugin->loadedPlugins("Actor*");
    foreach (QObject *o, actors) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(o);
        if (actor) {
//            qDebug() << "Loading actor " << actor->name();
            createModuleFromActor(actor);
        }
    }
}

AnalizerPrivate::~AnalizerPrivate()
{
    delete lexer;
    delete pdAutomata;
    delete ast;
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


void AnalizerPrivate::compileTransaction(const ChangeTextTransaction & changes)
{
    QList<Statement*> removedStatements;
    QList<Statement*> newStatements;
    QList<int> removedLineNumbers = changes.removedLineNumbers.toList();
    QStringList newLines = changes.newLines;
    int insertPos = statements.size();
    int lineStart = 0;
    int lineEnd = 99999999;
    if (!removedLineNumbers.isEmpty()) {
        // We assume this set is sorted in ascending order
        lineStart = removedLineNumbers.first();
        lineEnd = removedLineNumbers.last();
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

    lexer->splitIntoStatements(newLines, lineStart, newStatements);

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

    if (removedStatements.size()>0 || newStatements.size()>0)
        doCompilation(subject, removedStatements, newStatements, statements, insertPos);

    foreach (Statement * st, removedStatements) {
        foreach (Lexem * lx, st->data) {
            delete lx;
        }
        delete st;
    }

}

QString AnalizerPrivate::StandartFunctionsModuleName = "Standart functions";

void AnalizerPrivate::createModuleFromActor(const Shared::ActorInterface * actor)
{
    AST::Module * mod = new AST::Module();
    mod->header.type = AST::ModTypeExternal;
    mod->header.name = actor->name();
    mod->header.enabled = mod->header.name==StandartFunctionsModuleName;
    mod->header.cReference.nameSpace = actor->name(Shared::PL_C);
    mod->header.cReference.moduleLibraries = actor->actorLibraries();
    mod->header.cReference.usedQtLibraries = actor->usedQtLibraries();
    mod->header.cReference.requiresGuiEventLoop = actor->requiresGui();
    ast->modules << mod;
    for (int i=0; i<actor->funcList().size(); i++) {
        AST::Algorhitm * alg = new AST::Algorhitm;
        alg->header.implType = AST::AlgorhitmExternal;
        alg->header.external.moduleName = actor->name();
        alg->header.external.id = actor->funcList()[i].id;
        alg->header.cHeader = actor->funcList()[i].cHeader;
        QList<Statement*> sts;
        lexer->splitIntoStatements(QStringList() << actor->funcList()[i].kumirHeader, -1, sts);
        Q_ASSERT_X(sts.size()==1
                   , "AnalizerPrivate::createModuleFromActor"
                   , QString("Algorhitm %1 in module %2 has syntax error").arg(actor->funcList()[i].kumirHeader).arg(actor->name()).toLocal8Bit().data());
        alg->impl.headerLexems = sts[0]->data;
        sts[0]->alg = alg;
        sts[0]->mod = mod;
        analizer->init(sts, ast, alg);
        analizer->buildTables();
        foreach (const Lexem *lx, sts[0]->data) {
            if (!lx->error.isEmpty()) {
                Q_ASSERT_X(sts.size()==1
                           , "AnalizerPrivate::createModuleFromActor"
                           , QString("Algorhitm %1 in module %2 has syntax error").arg(actor->funcList()[i].kumirHeader).arg(actor->name()).toLocal8Bit().data());
            }
            delete lx;
        }
        delete sts[0];
    }
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
    for (int i=0; i<d->statements.size(); i++) {
        foreach (const Lexem * lx, d->statements[i]->data) {
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
                result << err;
            }
        }
    }
    return result;
}

QList<LineProp> Analizer::lineProperties() const
{
    QList<LineProp> result;
    QStringList lines = d->sourceText;
    for (int i=0; i<lines.size(); i++) {
        result << LineProp(lines[i].size(), LxTypeEmpty);
    }

    result << LineProp(0, LxTypeEmpty);

    for (int i=0; i<d->statements.size(); i++) {
        foreach (const Lexem * lx, d->statements[i]->data) {
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
    QStringList lines = d->sourceText;
    for (int i=0; i<lines.size(); i++) {
        result << QPoint(0,0);
    }
    for (int i=0; i<d->statements.size(); i++) {
        Q_ASSERT (!d->statements[i]->data.isEmpty());
        const Lexem * lx = d->statements[i]->data.first();
        const int lineNo = lx->lineNo;
        const QPoint rank = d->statements[i]->indentRank;
        result[lineNo] = rank;
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


void AnalizerPrivate::doCompilation(AnalizeSubject whatToCompile
                                    , QList<Statement*> & oldStatements
                                    , QList<Statement*> & newStatements
                                    , QList<Statement*> & allStatements
                                    , int whereInserted
                                    )
{
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
    if (whatToCompile==SubjWholeText) {
        foreach (Statement * st, allStatements) {
            foreach (Lexem * lx, st->data) {
                if (!lx->lexerError)
                    lx->error = "";
            }
        }
        analizingStatements = allStatements;
        pdAutomata->init(analizingStatements, ast, alg);
        pdAutomata->process();
        pdAutomata->postProcess();
    }
    else if (whatToCompile==SubjAlgorhtitm) {

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
        Q_CHECK_PTR(alg);
        int algBeginIndex = -1, algEndIndex = -1;
        if (findAlgorhitmBounds(allStatements, alg, algBeginIndex, algEndIndex)) {
            for (int i=algBeginIndex; i<=algEndIndex; i++) {
                foreach (Lexem *olx, allStatements[i]->data) {
                    if (!olx->lexerError)
                        olx->error = "";
                }
                analizingStatements << allStatements[i];
            }
        }
        pdAutomata->init(analizingStatements, ast, alg);
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
            pdAutomata->init(newStatements, ast, alg);
            pdAutomata->process();
            pdAutomata->postProcess();
        }
        analizingStatements = newStatements;
    }
    analizer->init(analizingStatements, ast, alg);
    if (whatToCompile!=SubjStatements)
        analizer->buildTables();

    analizer->processAnalisys();
}

const AST::Data * Analizer::abstractSyntaxTree() const
{
    return d->ast;
}

} // namespace KumirAnalizer
