#ifndef STDLIB_CONNECTOR_PIPE_H
#define STDLIB_CONNECTOR_PIPE_H

#include <QtCore>
#include "connector_backend.h"

namespace StdLib {

class Connector_PIPE : public AbstractConnectorBackend
{
    Q_OBJECT
    Q_ENUMS(MessageType)
public:
    enum MessageType {
        Undefined = 0x00,
        OutputMessage = 0x01,
        InputMessage = 0x02,
        CommandMessage = 0x03,
        ResetMessage = 0x04,
        ErrorMessage = 0x05,
        OkReply = 0xf1,
        ErrorReply = 0xf2
    };
    enum Mode {
        Server,
        Client
    };

    explicit Connector_PIPE(QObject *parent = 0);

signals:
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
    void handleProcessReadyReadStandardOutput();

    void flushOutBuffer();
    MessageType readMessageType();
    int readMessageSize();
    void handleMessageReceived(MessageType t, QByteArray * data);

    void waitForReply();
protected:
    void run();
    QProcess * m_process;
    QByteArray * ba_in;
    QByteArray * ba_out;
    QString buffer_base64;
    QTextStream * text_in;
    QTextStream * text_out;
    MessageType e_inMessageType;
    MessageType e_outMessageType;
    int i_messageSize;
    bool b_stopping;
    QMutex * mutex_stopping;
    Mode e_mode;
    QMutex * mutex_reply;
    bool b_replyReceived;
};

inline QDataStream & operator<<(QDataStream & stream, Connector_PIPE::MessageType t)
{
    stream << quint8(t);
    return stream;
}

inline QDataStream & operator>>(QDataStream & stream, Connector_PIPE::MessageType & t)
{
    quint8 v;
    stream >> v;
    t = Connector_PIPE::MessageType(v);
    return stream;
}

} // namespace StdLib

#endif // STDLIB_CONNECTOR_PIPE_H
