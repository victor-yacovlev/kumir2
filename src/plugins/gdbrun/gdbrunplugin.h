#ifndef GDB_RUN_PLUGIN
#define GDB_RUN_PLUGIN

#include "extensionsystem/kplugin.h"
#include "interfaces/runinterface.h"

#include <QProcess>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>


namespace GdbRun {

using namespace ExtensionSystem;
using namespace Shared;



class GdbRunPlugin
        : public KPlugin
        , public RunInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::RunInterface)
public:
    explicit GdbRunPlugin();
    QString initialize(const QStringList &conf, const CommandLine &);
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

signals:
    void stopped(int reason);
    void outputRequest(const QString & output);
    void errorOutputRequest(const QString &);
    void inputRequest(const QString & format);
    void finishInput(const QVariantList & data);

protected slots:
    void handleInputDone(const QVariantList &data);

    void handleGdbClientReadStdOut();
    void handleGdbClientReadStdErr();

    void handleGdbServerReadStdOut();
    void handleGdbServerReadStdErr();

    void handleGdbStatusStream(const QByteArray & resultStream);
    void handleGdbInteractionStream(const QByteArray & resultStream);
    void handleGdbLogOutputStream(const QByteArray & resultStream);
    void handleGdbAsyncMessageStream(const QByteArray & resultStream);
    void handleGdbAsyncLogStream(const QByteArray & resultStream);

protected:
    virtual void extractInputFormat();
    virtual void processGdbQueryResponse(const QMap<QString,QVariant>& response);


private:    
    enum GdbState {
        NotStarted,
        StartedServer,
        StartedBoth,
        Querying,
        Paused,
        Running,
        Terminating
    };


    void sendGdbCommand(const QByteArray& command);
    void queueGdbCommand(const QByteArray& command, GdbState condition);
    void queueGdbCommands(const QList<QByteArray>& command, GdbState condition);
    void flushGdbCommands();
    static QMap<QString,QVariant> parseGdbMiCommandOutput(const QString & out);
    static QStringList splitByTopLevelComas(const QString &in);
    static QString gdbCommand();
    static QString gdbServerCommand();


    QProcess* gdbClient_;
    QProcess* gdbServer_;
    QString programFileName_;    
    QStandardItemModel* variablesModel_;
    Q_PID inferiorPid_;
    QTextCodec* ioCodec_;
    GdbState gdbState_;
    QMutex* gdbStateLocker_;
    struct StateWaiter: public QThread {
        inline StateWaiter(GdbRunPlugin* parent): QThread(parent), p(parent) {}
        GdbRunPlugin* p;
        GdbState t;
        void run();
        void waitForState(GdbState targetState);
    }* stateWaiter_;
    struct ConditionalCommand {
        QByteArray command;
        GdbState cond;
    };
    QLinkedList<ConditionalCommand> gdbCommandsQueue_;
    QMutex* gdbCommandsQueueLocker_;

};

}


#endif
