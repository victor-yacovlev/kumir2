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

    bool loadProgram(const QByteArray & source, Shared::ProgramFormat format);
    bool hasMoreInstructions() const;
    QString error() const;
    QVariantList remainingValues() const;
    int currentLineNo() const ;
    inline QDateTime loadedProgramVersion() const { return dt_loadedVersion; }

    bool isGuiRequired() const;

    QVariant value(int moduleId, int algorhitmId, int variableId) const;

    QList<int> bounds(int moduleId, int algorhitmId, int variableId) const;

    QList<int> reference(int moduleId, int algorhitmId, int variableId) const;


public slots:
    void runBlind();
    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runStepOut();
    void runTesting();
    void terminate();

    void finishInput(const QVariantList & message);
    void finishExternalFunctionCall(
        const QString & error,
        const QVariant & retval,
        const QVariantList & results
        );


protected:
    void start();
    QString initialize(const QStringList &);
    QDateTime dt_loadedVersion;
    bool b_done;
    bool b_onlyOneTryToInput;

protected slots:
    void handleOutput(const QString & text);
    void handleInput(const QString & format);
    void handleThreadFinished();
    void handleLineChanged(int lineNo);


signals:
    void stopped(int reason);
    void inputRequest(const QString & format);
    void outputRequest(const QString & output);
    void lineChanged(int lineNo);
    void marginText(int lineNo, const QString & text);
    void clearMargin(int fromLine, int toLine);
    void resetModule(const QString &actorPluginName);


private:
    class Run * d;

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_PLUGIN_H
