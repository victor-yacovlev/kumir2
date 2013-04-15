#ifndef ANALIZER_INTERFACE_H
#define ANALIZER_INTERFACE_H

#include "error.h"
#include "lineprop.h"
#include "dataformats/ast.h"

#include <QtCore>

namespace Shared {

struct ChangeTextTransaction {
    QSet<int> removedLineNumbers;
    QStringList newLines;
};

typedef QPair<QString,quint32> TextAppend;

struct Suggestion {
    enum Kind { Local, Global, Algorithm, BuiltinModule, KumirModule, Statement, SecondaryKeyword, Other };
    inline Suggestion() { kind = Other; showOnlyInFullList = false; }
    QString value;
    QString description;
    bool showOnlyInFullList;
    Kind kind;
};

class AnalizerInterface {
public:
    virtual int newDocument() = 0;
    virtual bool primaryAlphabetIsLatin() const = 0;
    virtual bool caseInsensitiveGrammatic() const = 0;
    virtual bool supportPartialCompiling() const = 0;
    virtual void dropDocument(int documentId) = 0;
    virtual void setSourceDirName(int documentId, const QString & dirPath) = 0;
    virtual void setSourceText(int documentId, const QString &text) = 0;
    virtual void setHiddenText(int documentId, const QString &text, int baseLine) = 0;
    virtual void setHiddenTextBaseLine(int documentId, int baseLine) = 0;
    virtual void changeSourceText(int documentId, const QList<ChangeTextTransaction> & changes) = 0;

    /**
     * @brief suggestAutoComplete
     * Make a suggestion variants for a given text cursor position
     *
     * @param documentId -- the document ID
     * @param lineNo -- line number of text cursor
     * @param before -- text before the cursor
     * @param after -- text after the cursor
     * @return a list of structs { autocomplete text, the description of suggestion }
     */
    virtual QList<Suggestion> suggestAutoComplete(int documentId, int lineNo, const QString & before, const QString & after) const { return QList<Suggestion>(); }
    virtual TextAppend closingBracketSuggestion(int documentId, int lineNo) const { return QPair<QString,quint32>(QString(), 0); }
    virtual QList<Error> errors(int documentId) const = 0;
    virtual QList<LineProp> lineProperties(int documentId) const = 0;
    virtual QList<QPoint> lineRanks(int documentId) const = 0;
    virtual QStringList imports(int documentId) const = 0;
    virtual const AST::Data * abstractSyntaxTree(int documentId) const = 0;
    virtual LineProp lineProp(int documentId, const QString & text) const = 0;
    virtual std::string rawSourceData(int documentId) const = 0;
    inline virtual QList<QRegExp> supportedFileNamePattern() const {
        return QList<QRegExp>()
                << QRegExp("*",Qt::CaseInsensitive,QRegExp::Wildcard);
    }

    virtual QString createImportStatementLine(const QString &importName) const = 0;
};

}

Q_DECLARE_INTERFACE(Shared::AnalizerInterface, "kumir2.Analizer");

#endif
