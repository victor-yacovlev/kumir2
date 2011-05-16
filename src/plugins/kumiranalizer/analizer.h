#ifndef ANALIZER_H
#define ANALIZER_H

#include "interfaces/error.h"
#include "interfaces/lineprop.h"
#include "abstractsyntaxtree/ast.h"

#include <QtCore>

namespace KumirAnalizer {

class Analizer : public QObject
{
    Q_OBJECT
public:
    /**
      * Constructor
      */
    explicit Analizer(QObject * = 0);

    /**
      * Destructor
      */
    ~Analizer();

    /**
      * Set application-wide (while initialization)
      * Kumir source language (Russian, Ukrainian, etc.)
      */
    static void setSourceLanguage(const QLocale::Language & language);

public slots:

    /**
      * Sets initial source program text
      */
    void setSourceText(const QString &text);

    /**
      * Changes current program source text
      */
    void changeSourceText(const QList<int> & removedLineNumbers, const QStringList & newLines);

    /**
      * Get all analizer errors
      */
    QList<Shared::Error> errors() const;

    /**
      * Get line properties (as bitmasks) for syntax highlighting etc.
      */
    QList<Shared::LineProp> lineProperties() const;

    /**
      * Get calculated line indents
      */
    QList<QPoint> lineRanks() const;

    /**
      * Get module import list
      */
    QStringList imports() const;

    /**
      * Get (for read only!) abstract syntax tree pointer
      */
    const AST::Data * abstractSyntaxTree() const;

private:
    class AnalizerPrivate * d;

};

} // namespace KumirAnalizer

#endif // ANALIZER_H
