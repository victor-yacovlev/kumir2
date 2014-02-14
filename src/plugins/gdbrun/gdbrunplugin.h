#ifndef GDB_RUN_PLUGIN
#define GDB_RUN_PLUGIN

#include "extensionsystem/kplugin.h"
#include "interfaces/runinterface.h"

#include <QProcess>
#include <QFile>

namespace GDBRun {

using namespace ExtensionSystem;
using namespace Shared;

class GDBRunPlugin
        : public KPlugin
        , public RunInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::RunInterface)
public:
    explicit GDBRunPlugin();
    QString initialize(const QStringList &, const CommandLine &);
    void updateSettings(const QStringList &);
    bool loadProgram(const QString &fileName, const QByteArray &source);
    QDateTime loadedProgramVersion() const;
    bool canStepOut() const;
    void runBlind();
    void runContinuous();
    void runStepInto();
    void runStepOver();
    void runToEnd();
    void runTesting();
    bool isTestingRun() const;
    void terminate();
    bool hasMoreInstructions() const;
    bool hasTestingEntryPoint() const;
    int currentLineNo() const;
    QPair<quint32,quint32> currentColumn() const;
    QString error() const;
    QVariant valueStackTopItem() const;
    unsigned long int stepsCounted() const;
    inline QAbstractItemModel * debuggerVariablesViewModel() const { return variablesModel_; }

private:
    QProcess* gdb_;
    QFile* programFile_;
    QStandardItemModel* variablesModel_;

};

}


#endif
