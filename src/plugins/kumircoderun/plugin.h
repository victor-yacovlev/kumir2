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

    bool loadProgram(QIODevice * source, Shared::ProgramFormat format);
    bool hasMoreInstructions() const;
    QString error() const;

public slots:

    void runContinuous();
    void runStepOver();
    void runStepInto();
    void runStepOut();
    void terminate();

    void replyToRequest(const QVariantList & message);

protected:
    QString initialize(const QStringList &);

signals:
    void stopped();
    void externalRequest(const QString &actorPluginName, const QString & algName, const QVariantList & args);
    void inputRequest(const QString & format);
    void outputRequest(const QVariantList & output);

private:
    class Run * d;

};

} // namespace KumirCodeRun

#endif // KUMIRCODERUN_PLUGIN_H
