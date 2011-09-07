#ifndef STDLIB_CONNECTOR_SHM_H
#define STDLIB_CONNECTOR_SHM_H

#include <QtCore>
#include "connector_backend.h"

namespace StdLib {

class Connector_SHM: public AbstractConnectorBackend
{
    Q_OBJECT
    enum MessageSender {
        IM_NoMessage = 0x0000,
        IM_Kumir = 0x0001,
        IM_Program = 0x0002
    };

    struct InterprocessMessage {
        MessageSender type;
        quint32 messageSize;
        bool replyRequired;
        char data[1024*1024];
    };

    struct Request {
        QVariantList data;
        bool replyRequired;
    };


public:
    explicit Connector_SHM(QObject *parent = 0);

signals:
    void requestReceived(const QVariantList & message);
    void outputTextReceived(const QString & text);
    void errorMessageReceived(const QString & message);
    void inputFormatReceived(const QString & format);
    void errorReceived(int lineNo);
    void resetActorReceived(const QString & name);
    void actorCommandReceived(const QString & actorName,
                              const QString & command,
                              const QVariantList & arguments);

public slots:
    bool connectTo(int argc, char* *argv);
    void listenFor(QProcess * process);

    void sendReply(const QVariantList & message);

    void output(const QString & text);
    QVariantList input(const QString & format);
    void error(const QString & message, int lineNo);
    void resetActor(const QString & pluginName);

    // Returns : [0] - error, [1] - result, [2..N-1] - opt results
    QVariantList evaluateActorCommand(const QString & pluginName,
                                      const QString & command,
                                      const QVariantList & arguments);

    void waitForEmptyAndStop();
    void stopListen();
protected slots:
    void handleStandardRequest(const QVariantList & message);
    void sendAsynchronousRequest(const QVariantList & message);
    QVariantList sendSynchronousRequest(const QVariantList & message);
protected:
    void run();
    void listenWorker();
    void requestWorker();
    void waitForStatus(MessageSender s);
    void waitForReply();
    void waitForRequest();
    InterprocessMessage * currentFrame();
private:

    QMutex * mutex_reply;
    QVariantList l_replyBuffer;

    QMutex * mutex_request;
    QQueue<Request> q_requestBuffer;

    QMutex * mutex_stopOnEmpty;
    bool b_stopOnEmpty;

    QMutex * mutex_stopServing;
    bool b_stopServing;

    enum ChatState { CS_Idle, CS_Sending, CS_Receiving } e_state;
    QSharedMemory * shm;

    enum MessageSender e_otherSender;
    enum MessageSender e_me;


};

} // namespace StdLib

#endif // STDLIB_CONNECTOR_SHM_H
