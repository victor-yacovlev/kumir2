#include "connector_pipe.h"
#include <iostream>
namespace StdLib {

Connector_PIPE::Connector_PIPE(QObject *parent) :
    QThread(parent)
{
    m_process = 0;
    ba_in = new QByteArray();
    ba_out = new QByteArray();
    text_in = text_out = 0;
    i_messageSize = 0;
    e_inMessageType = Undefined;
    e_outMessageType = Undefined;
    mutex_stopping = new QMutex;
    mutex_reply = new QMutex;
    b_stopping = false;
}

bool Connector_PIPE::connectTo(int argc, char **argv)
{
    for (int i=1; i<argc; i++) {
        if (QString::fromLocal8Bit(argv[i]).startsWith("--key=")) {
            text_in = new QTextStream(stdin, QIODevice::ReadOnly);
            text_out = new QTextStream(stdout, QIODevice::WriteOnly);
            e_mode = Client;
            start();
            return true;
        }
    }
    return false;
}

void Connector_PIPE::stopListen()
{
    QMutexLocker l(mutex_stopping);
    b_stopping = true;
    text_out = text_in = 0;
    if (text_out) {
        delete text_out;
    }
    if (text_in) {
        delete text_in;
    }
    m_process = 0;
    e_inMessageType = e_outMessageType = Undefined;
}

void Connector_PIPE::waitForEmptyAndStop()
{
    QMutexLocker l(mutex_stopping);
    b_stopping = true;
    if (text_in) {
        delete text_in;
    }
    if (text_out) {
        text_out->flush();
        delete text_out;
    }
    text_out = text_in = 0;
}

void Connector_PIPE::output(const QString &text)
{
    e_outMessageType = OutputMessage;
    QDataStream ds(ba_out, QIODevice::WriteOnly);
    ds << text;
    flushOutBuffer();
}

QVariantList Connector_PIPE::input(const QString &format)
{
    e_outMessageType = InputMessage;
    QDataStream ds(ba_out, QIODevice::WriteOnly);
    ds << format;
    flushOutBuffer();
    QVariantList result;
//    fprintf(stderr, "Waiting for reply...\n");
//    fprintf(stderr, "Process is running: %s\n", isRunning()? "yes" : "no");
    waitForReply();
//    fprintf(stderr, "...reply received\n");
    QDataStream di(ba_in, QIODevice::ReadOnly);
    di >> result;

    return result;
}

void Connector_PIPE::error(const QString &message, int lineNo)
{
    e_outMessageType = ErrorMessage;
    QDataStream ds(ba_out, QIODevice::WriteOnly);
    ds << lineNo << message;
    flushOutBuffer();
}

QVariantList Connector_PIPE::evaluateActorCommand(
    const QString &pluginName,
    const QString &command,
    const QVariantList &arguments)
{
    e_outMessageType = CommandMessage;
    QDataStream ds(ba_out, QIODevice::WriteOnly);
    ds << pluginName << command << arguments;
    flushOutBuffer();
    QVariantList result;
    waitForReply();
    QDataStream di(ba_in, QIODevice::ReadOnly);
    di >> result;

    return result;
}

void Connector_PIPE::resetActor(const QString &pluginName)
{
    e_outMessageType = ResetMessage;
    QDataStream ds(ba_out, QIODevice::WriteOnly);
    ds << pluginName;
    flushOutBuffer();
}

void Connector_PIPE::sendReply(const QVariantList &message)
{
    e_outMessageType = OkReply;
    QDataStream ds(ba_out, QIODevice::WriteOnly);
    ds << message;
    flushOutBuffer();
}

void Connector_PIPE::listenFor(QProcess *process)
{
    stopListen();
    m_process = process;
    process->setProcessChannelMode(QProcess::SeparateChannels);
    process->setReadChannel(QProcess::StandardOutput);
//    process->setStandardErrorFile("log.err");
    stopListen();
    text_in = new QTextStream(process);
    text_out = new QTextStream(process);
    b_stopping = false;
    e_mode = Server;
    start();
}

#ifdef Q_OS_WIN32
#define INTERVAL 50
#else
#define INTERVAL 100
#endif

void Connector_PIPE::run()
{
    forever {
        mutex_stopping->lock();
        if (b_stopping) {
            mutex_stopping->unlock();
            return;
        }
        mutex_stopping->unlock();
        msleep(INTERVAL);
        handleProcessReadyReadStandardOutput();
    }
}


void Connector_PIPE::handleProcessReadyReadStandardOutput()
{
    if (text_in==0)
        return;
    if (e_inMessageType==Undefined) {
        e_inMessageType = readMessageType();
        i_messageSize = readMessageSize();
        buffer_base64.clear();
    }
    if (e_inMessageType!=Undefined) {
        if (buffer_base64.size()<i_messageSize) {
            buffer_base64 += text_in->read(i_messageSize - buffer_base64.size());
            buffer_base64 = buffer_base64.trimmed();
        }
        int size = buffer_base64.size();

        if (size==i_messageSize) {
            ba_in->clear();
            ba_in->append(QByteArray::fromBase64(buffer_base64.toAscii()));
            if (e_mode==Server)
                handleMessageReceived(e_inMessageType, ba_in);
            else {
                mutex_reply->lock();
                b_replyReceived = true;
                mutex_reply->unlock();
            }
            e_inMessageType = Undefined;
        }
    }
}

void Connector_PIPE::waitForReply()
{
    forever {
//        fprintf(stderr, "aaa\n");
        mutex_reply->lock();
//        fprintf(stderr, "bbb\n");
        if (b_replyReceived) {
            mutex_reply->unlock();
//            fprintf(stderr, "Reply received\n");
            return;
        }
        else {
//            fprintf(stderr, "Reply NOT received\n");
            mutex_reply->unlock();
            msleep(INTERVAL);
        }
    }
}

void Connector_PIPE::handleMessageReceived(
    MessageType t,
    QByteArray *data)
{
    QDataStream ds(data, QIODevice::ReadOnly);
    if (t==OutputMessage) {
        QString text;
        ds >> text;
        emit outputTextReceived(text);
    }
    else if (t==InputMessage) {
        QString format;
        ds >> format;
        emit inputFormatReceived(format);
    }
    else if (t==CommandMessage) {
        QString name;
        QString command;
        QVariantList args;
        ds >> name >> command >> args;
        emit actorCommandReceived(name, command, args);
    }
    else if (t==ResetMessage) {
        QString name;
        ds >> name;
        emit resetActorReceived(name);
    }
    else if (t==ErrorMessage) {
        int lineNo;
        QString message;
        ds >> lineNo >> message;
        emit errorReceived(lineNo);
        emit errorMessageReceived(message);
    }
}

void Connector_PIPE::flushOutBuffer()
{
    QString b64 = QString::fromAscii(ba_out->toBase64()).trimmed();
    *text_out << QString::number(quint8(e_outMessageType), 16);
    *text_out << " ";
    *text_out << QString::number(b64.size());
    *text_out << " ";
    *text_out << b64;
    *text_out << "\n";
    text_out->flush();
    ba_out->clear();
    e_outMessageType = Undefined;
}

Connector_PIPE::MessageType Connector_PIPE::readMessageType()
{
    QString aType;
    QString buf;
    forever {
        text_in->device()->waitForReadyRead(5);
        buf = text_in->read(1);
        aType += buf;
        buf = buf.trimmed();
        aType = aType.trimmed();
        if (buf.trimmed().isEmpty() && !aType.isEmpty()) {
            break;
        }
    }
    bool ok = false;
    quint8 no = aType.toUInt(&ok, 16);
    if (ok)
        return MessageType(no);
    else
        return ErrorReply;
}

int Connector_PIPE::readMessageSize()
{
    QString aSize;
    QString buf;
    forever {
        buf = text_in->read(1);
        aSize += buf;
        buf = buf.trimmed();
        aSize = aSize.trimmed();
        if (buf.trimmed().isEmpty() && !aSize.isEmpty()) {
            break;
        }
    }
    bool ok = false;
    int size = aSize.toInt(&ok);
    if (ok)
        return size;
    else
        return 0;
}


} // namespace StdLib
