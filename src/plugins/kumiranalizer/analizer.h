#ifndef ANALIZER_H
#define ANALIZER_H

#include "interfaces/analizer_instanceinterface.h"

#include "interfaces/error.h"
#include "interfaces/lineprop.h"
#include "dataformats/ast.h"
#include "dataformats/ast_module.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/ast_type.h"
#include "interfaces/analizerinterface.h"

#include <QtCore>

namespace KumirAnalizer {

class Analizer
        : public QObject
        , public Shared::Analizer::InstanceInterface
        , public Shared::Analizer::ASTCompilerInterface
        , public Shared::Analizer::HelperInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Analizer::InstanceInterface
                 Shared::Analizer::ASTCompilerInterface
                 Shared::Analizer::HelperInterface
                 )
    friend class AnalizerPrivate;
public:

    explicit Analizer(class KumirAnalizerPlugin * plugin, bool teacherMode);
    Shared::AnalizerInterface * plugin();

    ~Analizer();

    /**
      * Set application-wide (while initialization)
      * Kumir source language (Russian, Ukrainian, etc.)
      */
    static void setSourceLanguage(const QDir & resourcesRoot, const QLocale::Language & language);
    static void setModuleAlwaysAvailable(const QString & moduleName);
    void connectSignalImportsChanged(QObject *receiver, const char *slot);

public slots:


    void setSourceDirName(const QString & dirName);   
    Shared::Analizer::TextAppend closingBracketSuggestion(int lineNo) const;
    QStringList importModuleSuggestion(int lineNo) const;
    QString sourceText() const;
    std::string rawSourceData() const;
    void setSourceText(const QString & text);
    QList<Shared::Analizer::Suggestion> suggestAutoComplete(int lineNo, const QString &before, const QString &after) const;
    Shared::Analizer::ApiHelpItem itemUnderCursor(const QString & text, int lineNo, int colNo, bool includeRightBound) const;


    QList<Shared::Analizer::Error> errors() const;

    QList<Shared::Analizer::LineProp> lineProperties() const;

    bool multipleStatementsInLine(int lineNo) const;

    Shared::Analizer::LineProp lineProp(int lineNo, const QString & text) const;

    QStringList algorithmsAvailabaleForModule(const AST::ModulePtr currentModule) const;
    QStringList moduleNames() const;


    QList<QPoint> lineRanks() const;

    QStringList imports() const;
    QString createImportStatementLine(const QString &importName) const;

    const AST::DataPtr abstractSyntaxTree() const;

    const AST::ModulePtr findModuleByLine(int lineNo) const;

    QString suggestFileName() const;

private:
    const AST::AlgorithmPtr findAlgorhitmByLine(const AST::ModulePtr mod, int lineNo) const;
    struct AnalizerPrivate * d;
    bool teacherMode_;
    class KumirAnalizerPlugin * plugin_;



};

} // namespace KumirAnalizer

#endif // ANALIZER_H
