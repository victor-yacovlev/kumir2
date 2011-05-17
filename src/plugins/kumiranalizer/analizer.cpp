#include "analizer.h"
#include "analizer_p.h"
#include "interfaces/error.h"
#include "interfaces/lexemtype.h"
#include "lexer.h"
#include "pdautomata.h"
#include "syntaxanalizer.h"

using namespace Shared;

namespace KumirAnalizer {

void Analizer::setSourceLanguage(const QLocale::Language &language)
{
    Lexer::setLanguage(language);
}

Analizer::Analizer(QObject *parent) :
    QObject(parent)
{
    d = new AnalizerPrivate(this);
}

AnalizerPrivate::AnalizerPrivate(Analizer *qq)
{
    q = qq;
    ast = new AST::Data();
    lexer = new Lexer(q);
    pdAutomata = new PDAutomata(q);
    analizer = new SyntaxAnalizer(lexer, q);
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


void Analizer::changeSourceText(const QList<int> & removedLineNumbers, const QStringList & newLines)
{
    QList<Statement>::iterator it = d->statements.begin();
    QList<Statement> removedStatements;
    QList<Statement> newStatements;
    QList<Statement>::iterator insertPos = d->statements.end();
    int lineStart = 0;
    int lineEnd = 99999999;
    if (!removedLineNumbers.isEmpty()) {
        // We assume this set is sorted in ascending order
        lineStart = removedLineNumbers.first();
        lineEnd = removedLineNumbers.last();
    }
    while (it!=d->statements.end()) {
        Statement st = (*it);
        bool remove = false;
        bool insert = false;
        foreach (const Lexem * lx, st.data) {
            if ( (lx->lineNo>=lineStart) && (insertPos==d->statements.end())) {
                insert = true;
            }
            if (lx->lineNo>=lineStart && lx->lineNo<=lineEnd) {
                remove = true;
                break;
            }
        }

        if (remove) {
            removedStatements << st;
            it = d->statements.erase(it);
        }
        if (insert) {
            insertPos = it;
        }
        if (!remove) {
            it++;
        }
    }


    QStringList newSourceText = d->sourceText.mid(0, lineStart) + newLines;
    if (lineEnd+1<d->sourceText.size()) {
        newSourceText += d->sourceText.mid(lineEnd+1);
    }
    d->sourceText = newSourceText;

    d->lexer->splitIntoStatements(newLines, lineStart, newStatements);

    AnalizerPrivate::AnalizeSubject subjByOld =
            d->analizeSubject(removedStatements);

    AnalizerPrivate::AnalizeSubject subjByNew =
            d->analizeSubject(newStatements);

    AnalizerPrivate::AnalizeSubject subject = subjByOld * subjByNew;

    Statement fordebug;
    if (insertPos!=d->statements.end())
         fordebug = (*insertPos);
    Q_UNUSED(fordebug);

    for (QList<Statement>::iterator it=insertPos; it!=d->statements.end(); it++) {
        Statement st = (*it);
        foreach (Lexem * lx, st.data) {
            lx->lineNo += newLines.size() - removedLineNumbers.size();
        }
    }

    for (int i=0 ; i<newStatements.size(); i++) {
        insertPos = d->statements.insert(insertPos, newStatements[i]);
        insertPos ++;
    }

    d->doCompilation(subject, removedStatements, newStatements, d->statements);

    foreach (Statement st, removedStatements) {
        foreach (Lexem * lx, st.data) {
            delete lx;
        }
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


AnalizerPrivate::AnalizeSubject AnalizerPrivate::analizeSubject(const QList<Statement> &statements) const
{
//    QList<Shared::LexemType> lexemTypes;
//    int startLineNo = statements.isEmpty()? 0 : statements[0].data.first()->lineNo;
    AnalizeSubject result = SubjStatements;
    foreach (const Statement &st, statements) {
        LexemType lt = st.type;
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
            if (findAlgorhitmByPos(ast, st.data.first()->lineNo)) {
                if (result==SubjAlgorhtitm)
                    return SubjWholeText; // more that one algorhitm affected
                result = SubjAlgorhtitm;
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
        foreach (const Lexem * lx, d->statements[i].data) {
            if (!lx->error.isEmpty()) {
                Error err;
                err.line = lx->lineNo;
                err.start = lx->linePos;
                err.len = lx->length;
                err.code = lx->error;
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
        foreach (const Lexem * lx, d->statements[i].data) {
            for (int j=lx->linePos; j<lx->linePos+lx->length; j++) {
                unsigned int value = lx->type;
                const unsigned int errorMask = LxTypeError;
                if (!lx->error.isEmpty()) {
                    value = value | errorMask;
                }
                const int lineNo = lx->lineNo;
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
        Q_ASSERT (!d->statements[i].data.isEmpty());
        const Lexem * lx = d->statements[i].data.first();
        const int lineNo = lx->lineNo;
        const QPoint rank = d->statements[i].indentRank;
        result[lineNo] = rank;
    }
    return result;
}

bool AnalizerPrivate::findInstructionsBlock(
    AST::Data *data
    , const QList<Statement> statements
    , LAS &lst
    , LASI &begin
    , LASI &end
    , AST::Module *&mod
    , AST::Algorhitm *&alg)
{
    if (statements.isEmpty())
        return false;
    bool found = false;
    Statement first = statements.first();
    Statement last = statements.last();
    foreach (AST::Module * module, data->modules) {
        for (LASI it = module->impl.initializerBody.begin();
             it != module->impl.initializerBody.end();
             it++)
        {
            AST::Statement * st = (*it);
            if (st==first.statement) {
                mod = module;
                alg = 0;
                lst = module->impl.initializerBody;
                begin = it;
                found = true;
            }
            if (st==last.statement) {
                end = it + 1;
            }
        }
        if (!found) {
            foreach (AST::Algorhitm * algorhitm, module->impl.algorhitms) {
                for (LASI it = algorhitm->impl.body.begin();
                     it != algorhitm->impl.body.end();
                     it++)
                {
                    AST::Statement * st = (*it);
                    if (st==first.statement) {
                        mod = module;
                        alg = 0;
                        lst = module->impl.initializerBody;
                        begin = it;
                        found = true;
                    }
                    if (st==last.statement) {
                        end = it + 1;
                    }
                }
            }
        }
    }
    return found;
}

void AnalizerPrivate::doCompilation(AnalizeSubject whatToCompile
                                    , QList<Statement> & oldStatements
                                    , QList<Statement> & newStatements
                                    , QList<Statement> & allStatements
                                    )
{
    if (ast->modules.isEmpty())
        whatToCompile = SubjWholeText;
    AST::Algorhitm * alg = 0;
    if (whatToCompile==SubjWholeText) {
        pdAutomata->init(&allStatements, ast, alg);
        pdAutomata->process();
        pdAutomata->postProcess();
    }
    else if (whatToCompile==SubjAlgorhtitm) {
        Q_ASSERT(!newStatements.isEmpty());
        const Statement firstStatement = newStatements.first();
        Q_ASSERT(!firstStatement.data.isEmpty());
        const Lexem * lx = firstStatement.data.first();
        const int linePos = lx->lineNo;
        alg = findAlgorhitmByPos(ast, linePos);
        Q_CHECK_PTR(alg);
        pdAutomata->init(&newStatements, ast, alg);
        pdAutomata->process();
        pdAutomata->postProcess();
    }
    else {
        LAS lst;
        LASI begin, end;
        AST::Module * module = 0;
        AST::Algorhitm * algorhitm = 0;
        bool found = findInstructionsBlock(
                    ast,
                    oldStatements,
                    lst, begin, end,
                    module, algorhitm);
        if (found) {
            for (LASI it = begin; it!=end; it++) {
                delete (*it);
            }
            lst.erase(begin, end);
            LASI insertPos = end;
            for (int i=0; i<newStatements.size(); i++) {
                AST::Statement * instruction
                        = PDAutomata::createSimpleAstStatement(newStatements[i]);
                newStatements[i].mod = module;
                newStatements[i].alg = algorhitm;
                insertPos = lst.insert(insertPos, instruction);
                insertPos ++;
            }
        }
        else {
            pdAutomata->init(&newStatements, ast, alg);
            pdAutomata->process();
            pdAutomata->postProcess();
        }
    }
    analizer->init(&newStatements, ast, alg);
    if (whatToCompile!=SubjStatements)
        analizer->buildTables();

    analizer->processAnalisys();
}

const AST::Data * Analizer::abstractSyntaxTree() const
{
    return d->ast;
}

} // namespace KumirAnalizer
