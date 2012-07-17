#ifndef PYTHON3_LANGUAGE_PLUGIN_H
#define PYTHON3_LANGUAGE_PLUGIN_H

#include <QtCore>
#include "interfaces/analizerinterface.h"
#include "interfaces/runinterface.h"
#include "extensionsystem/kplugin.h"
#include "python3.2mu/Python.h"

namespace Python3Language {

struct Python3Module;
using namespace Shared;

class Python3LanguagePlugin
        : public ExtensionSystem::KPlugin
        , public Shared::AnalizerInterface
        , public Shared::RunInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::AnalizerInterface Shared::RunInterface)
public:
    Python3LanguagePlugin();
    // Analizer interface methods
    int newDocument();
    inline bool primaryAlphabetIsLatin() const { return 1; }
    inline bool caseInsensitiveGrammatic() const { return 0; }
    inline bool supportPartialCompiling() const { return 0; }
    void dropDocument(int documentId);
    void setSourceText(int documentId, const QString &text);
    void setHiddenText(int documentId, const QString &text, int baseLine);
    void setHiddenTextBaseLine(int documentId, int baseLine);
    void changeSourceText(int documentId, const QList<ChangeTextTransaction> &changes);
    QList<Error> errors(int documentId) const;
    QList<LineProp> lineProperties(int documentId) const;
    QList<QPoint> lineRanks(int documentId) const;
    QStringList imports(int documentId) const;
    inline const AST::Data* abstractSyntaxTree(int documentId) const { return 0; }
    LineProp lineProp(int documentId, const QString &text) const;
    QStringList algorhitmsAvailableFor(int documentId, int lineNo) const;
    QStringList globalsAvailableFor(int documentId, int lineNo) const;
    QStringList localsAvailableFor(int documentId, int lineNo) const;
    std::string rawSourceData(int documentId) const;

    // Runner interface methods
    bool loadProgram(QIODevice * source, ProgramFormat format);
    QDateTime loadedProgramVersion() const;

    bool canStepOut() const;
    void runBlind();
    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runStepOut();
    void runTesting();
    void terminate();
    bool hasMoreInstructions() const;
    int currentLineNo() const;
    QString error() const;
    QVariantList remainingValues() const;
    // Get variable value or QVariant::Invalid if not initialized
    QVariant value(int moduleId, int algorhitmId, int variableId) const;
    // Get array bounds or empty list if not initialized
    QList<int> bounds(int moduleId, int algorhitmId, int variableId) const;
    // Get triple  { module id, algorhitm id, variable id } for reference target or empty list if not initialized
    QList<int> reference(int moduleId, int algorhitmId, int variableId) const;

    void finishInput(const QVariantList & message);
    void finishExternalFunctionCall(
        const QString & error,
        const QVariant & retval,
        const QVariantList & results);
protected:
    QString initialize(const QStringList &arguments);
private:
    QVector<struct Python3Module*> m_documents;
    ::PyObject * py_analizer;
};


} // namespace

#endif
