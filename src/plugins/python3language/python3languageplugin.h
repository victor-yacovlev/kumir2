#ifndef PYTHON3_LANGUAGE_PLUGIN_H
#define PYTHON3_LANGUAGE_PLUGIN_H

#include <QtCore>

extern "C" {
#include <Python.h>
}


#include "interfaces/analizerinterface.h"
#include "interfaces/runinterface.h"
#include "extensionsystem/kplugin.h"


namespace Python3Language {

struct Python3Module;
class PythonRunThread;
class PyFileHandler;

using namespace Shared;

class Python3LanguagePlugin
        : public ExtensionSystem::KPlugin
        , public AnalizerInterface
        , public RunInterface
{
    friend class PythonRunThread;
    friend class PythonAnalizerInstance;
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.Python3Language" FILE "")
#endif
    Q_INTERFACES(Shared::AnalizerInterface Shared::RunInterface)
public:
    Python3LanguagePlugin();   

    // Analizer interface methods
    Analizer::InstanceInterface * createInstance();
    Analizer::SourceFileInterface * sourceFileHandler();
    inline bool primaryAlphabetIsLatin() const { return true; }
    inline bool caseInsensitiveGrammatic() const { return false; }
    inline virtual bool indentsSignificant() const { return true; }    
    inline QString defaultDocumentFileNameSuffix() const { return "py"; }
    inline QString languageName() const { return "Python"; }

    // Runner interface methods
    bool loadProgram(const RunnableProgram & program);
    inline QDateTime loadedProgramVersion() const { return loadedProgramVersion_; }
    bool canStepOut() const;
    bool isTestingRun() const;
    bool hasMoreInstructions() const;
    bool hasTestingEntryPoint() const;
    int currentLineNo() const;
    QPair<quint32,quint32> currentColumn() const;
    QString error() const;
    QVariant valueStackTopItem() const;
    unsigned long int stepsCounted() const;
    QAbstractItemModel * debuggerVariablesViewModel() const;
    void setStdInTextStream(QTextStream *stream);
    void setStdOutTextStream(QTextStream *stream);

Q_SIGNALS:
    void errorOutputRequest(const QString &);
    void outputRequest(const QString & output);
    void inputRequest(const QString & format);
    void finishInput(const QVariantList & data);
    void stopped(int reason);
    void updateStepsCounter(quint64);
    void lineChanged(int lineNo, quint32 colStart, quint32 colEnd);

public Q_SLOTS:
    // Runner interface slots
    void runBlind();
    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runToEnd();
    void runTesting();
    void terminate();
    void terminateAndWaitForStopped();

protected:
    void connectRunThreadSignals();
    QString initialize(const QStringList &, const ExtensionSystem::CommandLine &);
    void stop();
    inline void updateSettings(const QStringList &) {}
    static QString extractFunction(const QString & source, const QString &funcName);

protected Q_SLOTS:

private /*fields*/:
    PyFileHandler * fileHandler_;
    PyThreadState * pyMain_;
    PythonRunThread * runner_;
    QDateTime loadedProgramVersion_;
    QList<class PythonAnalizerInstance*> analizerInstances_;

};

} // namespace

#endif
