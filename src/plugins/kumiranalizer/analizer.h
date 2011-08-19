#ifndef ANALIZER_H
#define ANALIZER_H

#include "interfaces/error.h"
#include "interfaces/lineprop.h"
#include "dataformats/ast.h"
#include "interfaces/analizerinterface.h"

#include <QtCore>

namespace KumirAnalizer {

class Analizer : public QObject
{
    Q_OBJECT
public:

    explicit Analizer(class KumirAnalizerPlugin * plugin);

    ~Analizer();

    /**
      * Set application-wide (while initialization)
      * Kumir source language (Russian, Ukrainian, etc.)
      */
    static void setSourceLanguage(const QLocale::Language & language);

public slots:

    void changeSourceText(const QList<Shared::ChangeTextTransaction> & changes);

    QList<Shared::Error> errors() const;

    QList<Shared::LineProp> lineProperties() const;

    Shared::LineProp lineProp(const QString & text) const;

    QStringList algorhitmNames() const;
    QStringList moduleNames() const;


    QList<QPoint> lineRanks() const;

    QStringList imports() const;

    const AST::Data * abstractSyntaxTree() const;

private:
    class AnalizerPrivate * d;

};

} // namespace KumirAnalizer

#endif // ANALIZER_H
