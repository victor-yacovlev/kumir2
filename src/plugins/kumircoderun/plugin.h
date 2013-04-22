#ifndef KUMIRCODERUN_PLUGIN_H
#define KUMIRCODERUN_PLUGIN_H

#include <QtCore>
#include "extensionsystem/kplugin.h"
#include "interfaces/runinterface.h"

namespace KumirCodeRun {

class Plugin
        : public ExtensionSystem::KPlugin
        , public Shared::RunInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::RunInterface)
public:
    explicit Plugin();
    ~Plugin();


    bool canStepOut() const;

    bool loadProgram(const QString & fileName, const QByteArray & source, Shared::ProgramFormat format);
    bool hasMoreInstructions() const;
    QString error() const;

    int currentLineNo() const ;
    inline QDateTime loadedProgramVersion() const { return loadedVersion_; }

    inline bool isGuiRequired() { return false; }

    QMap<QString,QVariant> getScalarLocalValues(int frameNo) const;
    QMap<QString,QVariant> getScalarGlobalValues(const QString & moduleName) const;
    QVariantList getLocalTableValues(
            int frameNo,
            int maxCount,
            const QString & name,
            const QList< QPair<int,int> > & ranges,
            bool & complete
            ) const;
    QVariantList getGlobalTableValues(
            const QString & moduleName,
            int maxCount,
            const QString & name,
            const QList< QPair<int,int> > & ranges,
            bool & complete
            ) const;
    QVariant getLocalTableValue(int frameNo, const QString & name, const QList<int> & indeces) const;
    QVariant getGlobalTableValue(const QString & moduleName, const QString & name, const QList<int> & indeces) const;


public slots:
    void runBlind();
    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runStepOut();
    void runTesting();
    void terminate();




protected:
    void start();
    void timerEvent(QTimerEvent *);
    QString initialize(const QStringList &);
    void prepareCommonRun();
    void prepareConsoleRun();
    void prepareGuiRun();
    QDateTime loadedVersion_;
    bool done_;
    bool onlyOneTryToInput_;

protected slots:
    void handleThreadFinished();
    void handleLineChanged(int lineNo);


signals:
    void stopped(int reason);
    void finishInput(const QVariantList & data);
    void inputRequest(const QString & format);
    void outputRequest(const QString & output);
    void lineChanged(int lineNo);
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

private:
    class Run * pRun_;
    struct CommonFunctors * common_;
    struct ConsoleFunctors * console_;
    struct GuiFunctors * gui_;

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_PLUGIN_H
