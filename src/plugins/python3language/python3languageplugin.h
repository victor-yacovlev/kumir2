#ifndef PYTHON3_LANGUAGE_PLUGIN_H
#define PYTHON3_LANGUAGE_PLUGIN_H

#include "interfaces/analizerinterface.h"
#include "interfaces/runinterface.h"
#include "extensionsystem/kplugin.h"
#include <QtCore>
#include "Python.h"

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
    inline bool primaryAlphabetIsLatin() const { return true; }
    inline bool caseInsensitiveGrammatic() const { return false; }
    inline bool supportPartialCompiling() const { return false; }
    inline virtual bool indentsSignificant() const { return true; }
    bool hasTestingEntryPoint() const;
    unsigned long int stepsCounted() const;
    void dropDocument(int documentId);
    void setSourceText(int documentId, const QString &text);
    void setHiddenText(int documentId, const QString &text, int baseLine);
    void setHiddenTextBaseLine(int documentId, int baseLine);
    void changeSourceText(int documentId, const QList<ChangeTextTransaction> &changes);
    QList<Error> errors(int documentId) const;
    QList<LineProp> lineProperties(int documentId) const;
    QList<QPoint> lineRanks(int documentId) const;
    QStringList imports(int documentId) const;
    inline const AST::DataPtr abstractSyntaxTree(int documentId) const { return AST::DataPtr(); }
    LineProp lineProp(int documentId, int lineNo, const QString &text) const;
    std::string rawSourceData(int documentId) const;
    QString createImportStatementLine(const QString &importName) const;
    inline QString defaultDocumentFileNameSuffix() const { return ".py"; }
    inline QString languageName() const { return "Python"; }
    inline ResultType resultType() const { return RT_Source; }

    // Runner interface methods
    bool loadProgram(const QString &fileName, const QByteArray & source, ProgramFormat format);
    QDateTime loadedProgramVersion() const;
    void setSourceDirName(int documentId, const QString &dirPath);
    QVariant valueStackTopItem() const;

    QMap<QString,QVariant> getScalarLocalValues(int frameNo) const;
    QMap<QString,QVariant> getScalarGlobalValues(const QString & moduleName) const;
    QVariantList getLocalTableValues(int frameNo, int maxCount, const QString & name,
                                     const QList< QPair<int,int> > & ranges, bool & complete) const;
    QVariantList getGlobalTableValues(const QString & moduleName, int maxCount, const QString & name,
                                      const QList< QPair<int,int> > & ranges, bool & complete) const;
    QVariant getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const;
    QVariant getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const;
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
    inline QPair<quint32,quint32> currentColumn() const {
        return QPair<quint32,quint32>(0u,0u);
    }  // Python can't handle columns
    QString error() const;

    // methods to access within self static functions
    void handlePythonOutput(const QString & message);
    void handlePythonError(const QString & message);
    void handlePythonLineChanged(int lineNo);
    void handlePythonExecutionPaused();
    void handlePythonExecutionFinished();
    void handlePythonExecutionTerminated();
    void handlePythonExecutionError();
    void handlePythonMarginText(int lineNo, const QString &text);
    void handlePythonStepsCounterChanged(unsigned long int);

Q_SIGNALS:
    void updateStepsCounter(ulong);
    void stopped(int reason);
    void outputRequest(const QString & output);
    void errorOutputRequest(const QString & output);
    void lineChanged(int lineNo);

    void finishInput(const QVariantList & data);
    void inputRequest(const QString & format);
    void marginText(int lineNo, const QString & text);
    void replaceMarginText(int lineNo, const QString & text, bool redFgColor);
    void clearMargin(int fromLine, int toLine);
    void resetModule(const QString &actorPluginName);

    // Signals for debugger window
    void debuggerReset();
    void debuggerSetGlobals(
            /** module name */           const QString & moduleName,
            /** variable names */         const QStringList & names,
            /** variable base types */const QStringList & baseTypes,
            /** variable dimensions */ const QList<int> & dimensions
            );
    void debuggerPushContext(
            /** context header */       const QString & contextName,
            /** variable names */         const QStringList & names,
            /** variable base types */const QStringList & baseTypes,
            /** variable dimensions */ const QList<int> & dimensions
            );
    void debuggerPopContext();
    void debuggerUpdateLocalVariable(
            /** variable name */       const QString & name,
            /** value */              const QString & value
            );
    void debuggerUpdateGlobalVariable(
            /** module name */   const QString & moduleName,
            /** variable name */       const QString & name,
            /** value */              const QString & value
            );
    void debuggerUpdateLocalTableBounds(
            /** variable name */       const QString & name,
            /** bounds */          const QList<int> & bounds
            );
    void debuggerUpdateGlobalTableBounds(
            /** module name */   const QString & moduleName,
            /** variable name */       const QString & name,
            /** bounds */          const QList<int> & bounds
            );

    void debuggerSetLocalReference(
            /** variable name */             const QString & name,
            /** target name */         const QString & targetName,
            /** target array indeces */const QList<int> & indeces,
            /** stack frames back */                     int back,
            /** module name for a global value*/ const QString & moduleName
            );
    void debuggerForceUpdateValues();
    void debuggerUpdateLocalTableValue(
            /** variable name */            const QString & name,
            /** indeces */             const QList<int> & indeces
            );
    void debuggerUpdateGlobalTableValue(
            /** module name */        const QString & moduleName,
            /** variable name */            const QString & name,
            /** indeces */             const QList<int> & indeces
            );


protected:
    QString initialize(const QStringList &arguments);
    void setBlindMode(bool flag);

protected Q_SLOTS:
    void prepareAnalizer();
    void saveAnalizer();

public: // actually private, but required for python-access function
    ::PyObject * analizerModule_;
//    ::PyObject * runModule_;
    class RunInteractionWaiter * runInteractionWaiter_;
    class RunWorker * runWorker_;
    ::PyThreadState * mainPyThreadState_;
    QString analizerState_;
};

enum RunMode { RM_Blind, RM_Continuous, RM_StepOver, RM_StepIn, RM_StepOut };

struct RunInteractionWaiter
        : public QThread
{
    RunInteractionWaiter(Python3LanguagePlugin * parent);
    bool isWaiting() const;
    void continueExecution();
    void tryToPause();

    RunMode mode;
    QMutex * mutex;
    bool flag;
};

struct RunWorker
        : public QThread
{   
    static const int DONE = 0;
    static const int TERMINATED = 1;
    static const int ERROR = 2;

    void setTerminate();
    bool isTerminate();
    void clearTerminate();

    QString fileName;
    QString source;
    QString error;
    int currentLineNo;
    int previousLineNo;
    ::PyObject * previousFrameLocals;
    ::PyObject * previousFrameGlobals;
    ::PyObject * currentFrameLocals;
    ::PyObject * currentFrameGlobals;
    RunMode mode;
    bool terminateFlag;
    QMutex * terminateMutex;
    bool justStarted;
    unsigned long int stepsCounted;
    QMutex * stepsCounterMutex;

    QVector<QStringList> lvalueAtoms;


    RunWorker(Python3LanguagePlugin * parent);
    void run();
};


} // namespace

#endif
