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

    bool canStepInto() const;
    bool canStepOut() const;

    bool loadProgram(QIODevice * source, Shared::ProgramFormat format);
    bool hasMoreInstructions() const;
    QString error() const;
    int currentLineNo() const ;
    inline QDateTime loadedProgramVersion() const { return dt_loadedVersion; }

public slots:

    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runStepOut();
    void runTesting();
    void terminate();

    void finishInput(const QVariantList & message);

protected:
    QString initialize(const QStringList &);
    QDateTime dt_loadedVersion;
    bool b_done;

protected slots:
    void handleOutput(const QString & text);
    void handleInput(const QString & format);
    void handleThreadFinished();
    void handleLineChanged(int lineNo);


signals:
    void stopped(int reason);
    void externalRequest(const QString &actorPluginName, const QString & algName, const QVariantList & args);
    void inputRequest(const QString & format);
    void outputRequest(const QString & output);
    void lineChanged(int lineNo);


private:
    class Run * d;

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_PLUGIN_H
