#ifndef ANALIZER_H
#define ANALIZER_H

#include "interfaces/error.h"
#include "interfaces/lineprop.h"
#include "dataformats/ast.h"
#include "dataformats/ast_module.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_type.h"
#include "interfaces/analizerinterface.h"

#include <QtCore>

namespace KumirAnalizer {

class Analizer : public QObject
{
    Q_OBJECT
    friend class AnalizerPrivate;
public:

    explicit Analizer(class KumirAnalizerPlugin * plugin, bool teacherMode);

    ~Analizer();

    /**
      * Set application-wide (while initialization)
      * Kumir source language (Russian, Ukrainian, etc.)
      */
    static void setSourceLanguage(const QLocale::Language & language);
    static void setModuleAlwaysAvailable(const QString & moduleName);

public slots:


    void setSourceDirName(const QString & dirName);   
    void setHiddenBaseLine(int lineNo);
    Shared::TextAppend closingBracketSuggestion(int lineNo) const;
    QStringList importModuleSuggestion(int lineNo) const;
    QString sourceText() const;
    void setSourceText(const QString & text);
    QList<Shared::Suggestion> suggestAutoComplete(int lineNo, const QString &before, const QString &after) const;


    QList<Shared::Error> errors() const;

    QList<Shared::LineProp> lineProperties() const;

    Shared::LineProp lineProp(int lineNo, const QString & text) const;

    QStringList algorithmsAvailabaleForModule(const AST::ModulePtr currentModule) const;
    QStringList moduleNames() const;


    QList<QPoint> lineRanks() const;

    QStringList imports() const;

    const AST::DataPtr abstractSyntaxTree() const;

    const AST::ModulePtr findModuleByLine(int lineNo) const;
private:
    const AST::AlgorithmPtr findAlgorhitmByLine(const AST::ModulePtr mod, int lineNo) const;
    struct AnalizerPrivate * d;
    bool teacherMode_;



};

} // namespace KumirAnalizer

#endif // ANALIZER_H
