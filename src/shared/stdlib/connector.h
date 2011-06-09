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
    QVariantList sendRequest(const QVariantList & arguments);
signals:
    void requestReceived(const QVariantList & arguments);
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
    static Connector * m_instance;
};


} // namespace GuiRunner

extern "C" unsigned char __connected_to_kumir__();

#endif // GUIRUNNER_CONNECTOR_H
