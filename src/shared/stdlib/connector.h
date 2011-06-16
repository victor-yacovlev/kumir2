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
        quint32 pagesCount;
        quint32 currentPage;
        quint32 currentSize;
        char data[65536];
    };
public:
    explicit Connector();
    static Connector* instance();
    void connectTo(const QString &key);
    void listenFor(const QString &key);
    static bool connectedToKumir;
public slots:
    QVariantList sendRequest(const QVariantList & message);
signals:
    void requestReceived(const QVariantList & message);
    void outputTextReceived(const QString & text);
    void errorMessageReceived(const QString & message);
    void inputFormatReceived(const QString & format);
    void errorReceived(int lineNo);
protected slots:
    void handleStandardRequest(const QVariantList & message);
protected:
    void run();
    void waitForStatus(MessageSender s);
    InterprocessMessage * currentFrame();
private:


    static int PAGE_SIZE;
    enum ChatState { CS_Idle, CS_Sending, CS_Receiving } e_state;
    QSharedMemory * shm;
    QByteArray * ba_buffer;
    enum MessageSender e_otherSender;
    enum MessageSender e_me;
    static Connector * m_instance;
};


} // namespace GuiRunner

extern "C" STDLIB_EXPORT unsigned char __connected_to_kumir__();
extern "C" STDLIB_EXPORT void __try_connect_to_kumir__(int argc, char* *argv);
extern STDLIB_EXPORT void __connect_to_kumir__(const QString & key);
extern STDLIB_EXPORT void __show_actor_window__(const QString & moduleName);

#endif // GUIRUNNER_CONNECTOR_H
