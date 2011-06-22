#ifndef GUIRUNNER_CONNECTOR_H
#define GUIRUNNER_CONNECTOR_H

#include <QtCore>

#ifdef STDLIB_LIBRARY
#define STDLIB_EXPORT Q_DECL_EXPORT
#else
#define STDLIB_EXPORT Q_DECL_IMPORT
#endif


namespace StdLib {



class STDLIB_EXPORT Connector
        : public QThread
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
    explicit Connector();
    static Connector* instance();
    static bool connectedToKumir;

public slots:

    void connectTo(const QString &key);
    void listenFor(const QString &key);

    void sendAsynchronousRequest(const QVariantList & message);
    QVariantList sendSynchronousRequest(const QVariantList & message);
    void sendReply(const QVariantList & message);

    void waitForEmptyAndStop();
    void stopListen();

signals:
    void requestReceived(const QVariantList & message);
    void outputTextReceived(const QString & text);
    void errorMessageReceived(const QString & message);
    void inputFormatReceived(const QString & format);
    void errorReceived(int lineNo);
    void resetActorReceived(const QString & name);
    void actorCommandReceived(const QString & actorName, const QString & command, const QVariantList & arguments);
protected slots:
    void handleStandardRequest(const QVariantList & message);
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
    static Connector * m_instance;
};


} // namespace GuiRunner

struct ActorResponse { QString error; QVariant result; QVariantList res; };

extern "C" STDLIB_EXPORT unsigned char __connected_to_kumir__();
extern "C" STDLIB_EXPORT void __try_connect_to_kumir__(int argc, char* *argv);
extern "C" STDLIB_EXPORT void __wait_for_output_queue_flushed__();
extern STDLIB_EXPORT void __connect_to_kumir__(const QString & key);
extern STDLIB_EXPORT void __reset_actor__(const QString & moduleName);
extern STDLIB_EXPORT ActorResponse __run_actor_command__(const QString &actor, const QString & command, const QVariantList & arguments);


#endif // GUIRUNNER_CONNECTOR_H
