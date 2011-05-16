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


void Analizer::setSourceText(const QString &text)
{
    for (int i=0; i<d->statements.size(); i++) {
        for (int j=0; j<d->statements[i].data.size(); j++) {
            delete d->statements[i].data[j];
        }
    }
    d->statements.clear();
    d->lexer->splitIntoStatements(text
                                  , 0
                                  , d->statements
                                  );
    d->sourceText = text;
    d->doCompilation(AnalizerPrivate::SubjWholeText);

}

void Analizer::changeSourceText(const QList<int> & removedLineNumbers, const QStringList & newLines)
{
//    int startLine = d->sourceText.left(pos).count("\n");
//    int endLine = d->sourceText.left(pos+len).count("\n");
//    QList<Shared::Error>::iterator errorsIterator = d->errors.begin();
//    while (errorsIterator!=d->errors.end()) {
//        int line = (*errorsIterator).line;
//        if (line>=startLine || line<=endLine) {
//            errorsIterator = d->errors.erase(errorsIterator);
//        }
//        else {
//            errorsIterator ++;
//        }
//    }
//    QList<Statement> oldStatements;
//    int oldStatementsStart = -1;
//    for (int i=0; i<d->statements.size(); i++) {
//        int line = d->statements[i].data.first()->lineNo;
//        if (line>=startLine && line<=endLine) {
//            if (oldStatementsStart==-1)
//                oldStatementsStart = i;
//            oldStatements << d->statements[i];
//        }
//    }
//    QList<Statement> newStatements;
//    d->lexer->splitIntoStatements(repl
//                                  , startLine
//                                  , newStatements
//                                  );
//    int linesOffset = repl.count("\n") - (endLine-startLine);
//    for (int i=oldStatementsStart; i<d->statements.size(); i++) {
//        for (int j=0; j<d->statements[i].data.size(); j++)
//            d->statements[i].data[j]->lineNo += linesOffset;
//    }
//    d->statements = d->statements.mid(0, oldStatementsStart)
//            + newStatements
//            + d->statements.mid(oldStatementsStart+oldStatements.size());

//    d->sourceText.replace(pos, len, repl);
//    AnalizerPrivate::AnalizeSubject oldSubject = d->analizeSubject(oldStatements);
//    AnalizerPrivate::AnalizeSubject newSubject = d->analizeSubject(newStatements);
//    AnalizerPrivate::AnalizeSubject subject = oldSubject * newSubject;
//    d->doCompilation(subject);
    QStringList oldLines = d->sourceText.split("\n");
    QStringList n;
    if (removedLineNumbers.isEmpty()) {
        n = oldLines + newLines;
    }
    else {
        n = oldLines.mid(0, removedLineNumbers.first());
        n += newLines;
        if (removedLineNumbers.last()+1<oldLines.size()) {
            n += oldLines.mid(removedLineNumbers.last()+1);
        }
    }
    QString nt = n.join("\n");
    qDebug() << "New text is: \n" << nt;
    setSourceText(nt);

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


AnalizerPrivate::AnalizeSubject AnalizerPrivate::analizeSubject(const QList<Lexem*> &lexems, int startLineNo) const
{
    QList<Shared::LexemType> lexemTypes;
    foreach (const Lexem * lx, lexems) {
        lexemTypes << lx->type;
    }
    return analizeSubject(lexemTypes, startLineNo);
}

AnalizerPrivate::AnalizeSubject AnalizerPrivate::analizeSubject(const QList<Statement> &statements) const
{
    QList<Shared::LexemType> lexemTypes;
    int startLineNo = statements.isEmpty()? 0 : statements[0].data.first()->lineNo;
    foreach (const Statement &st, statements) {
        foreach (const Lexem * lx, st.data) {
            lexemTypes << lx->type;
        }
    }
    return analizeSubject(lexemTypes, startLineNo);
}

AnalizerPrivate::AnalizeSubject AnalizerPrivate::analizeSubject(const QList<Shared::LexemType> &lexemTypes, int startLineNo) const
{
    Q_UNUSED(startLineNo); // TODO check algorhitm range by startLineNo
    // Current implementation deals only in two modes:
    //  - whole text;
    //  - modified statements.
    foreach (const LexemType & lt, lexemTypes) {
        if ( ( lt & LxNameClass)
                || ( lt & LxPriImport)
                || ( lt & LxPriModule)
                || ( lt & LxPriEndModule)
                || ( lt & LxPriAlgHeader)
                || ( lt & LxPriAlgBegin)
                || ( lt & LxPriAlgEnd)
                || ( lt & LxPriIf)
                || ( lt & LxPriThen)
                || ( lt & LxPriElse)
                || ( lt & LxPriFi)
                || ( lt & LxPriSwitch)
                || ( lt & LxPriCase)
                || ( lt & LxPriLoop)
                || ( lt & LxPriEndLoop)
                )
        {
            return SubjWholeText;
        }
    }
    return SubjStatements;
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
    QStringList lines = d->sourceText.split("\n");
    for (int i=0; i<lines.size(); i++) {
        result << LineProp(lines[i].size(), LxTypeEmpty);
    }
    if (d->sourceText.endsWith("\n")) {
        result << LineProp(0, LxTypeEmpty);
    }
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
    QStringList lines = d->sourceText.split("\n");
    for (int i=0; i<lines.size(); i++) {
        result << QPoint(0,0);
    }
    for (int i=0; i<d->statements.size(); i++) {
        result[d->statements[i].data.first()->lineNo] = d->statements[i].indentRank;
    }
    return result;
}

void AnalizerPrivate::doCompilation(const AnalizeSubject &whatToCompile)
{
    Q_UNUSED(whatToCompile);
    // At this moment we compile everething
    pdAutomata->init(&statements, ast, 0);
    pdAutomata->process();
    pdAutomata->postProcess();
    analizer->init(&statements, ast, 0);
    analizer->buildTables();
    // TODO load unresolved imports here
    analizer->processAnalisys();
}

const AST::Data * Analizer::abstractSyntaxTree() const
{
    return d->ast;
}

} // namespace KumirAnalizer
