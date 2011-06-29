#ifndef ANALIZER_INTERFACE_H
#define ANALIZER_INTERFACE_H

#include "error.h"
#include "lineprop.h"
#include "abstractsyntaxtree/ast.h"

#include <QtCore>

namespace Shared {

struct ChangeTextTransaction {
    QSet<int> removedLineNumbers;
    QStringList newLines;
};

class AnalizerInterface {
public:
    virtual int newDocument() = 0;
    virtual void dropDocument(int documentId) = 0;
    virtual void setSourceText(int documentId, const QString &text) = 0;
    virtual void changeSourceText(int documentId, const QList<ChangeTextTransaction> & changes) = 0;
    virtual QList<Error> errors(int documentId) const = 0;
    virtual QList<LineProp> lineProperties(int documentId) const = 0;
    virtual QList<QPoint> lineRanks(int documentId) const = 0;
    virtual QStringList imports(int documentId) const = 0;
    virtual const AST::Data * abstractSyntaxTree(int documentId) const = 0;
    virtual LineProp lineProp(int documentId, const QString & text) const = 0;
    inline virtual QList<QRegExp> supportedFileNamePattern() const {
        return QList<QRegExp>()
                << QRegExp("*",Qt::CaseInsensitive,QRegExp::Wildcard);
    }
};

}

Q_DECLARE_INTERFACE(Shared::AnalizerInterface, "kumir2.Analizer");

#endif
