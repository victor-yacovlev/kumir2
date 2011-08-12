#include "connector_shm.h"

namespace StdLib {

Connector_SHM::Connector_SHM(QObject * parent)
    : QThread(parent)
{
    shm = new QSharedMemory(this);
    e_state = CS_Idle;
    e_me = e_otherSender = IM_NoMessage;
    mutex_reply = new QMutex;
    mutex_request = new QMutex;
    mutex_stopOnEmpty = new QMutex;
    mutex_stopServing = new QMutex;
    b_stopOnEmpty = false;
    b_stopServing = false;
}


bool Connector_SHM::connectTo(int pid)
{
    QString key = QString("kumir-%1").arg(pid);
    shm->setKey(key);
    if (!shm->attach()) {
        QString err = QString::fromAscii("Can't attach to Kumir: ")
                +shm->errorString();
        qFatal(err.toLocal8Bit().data());
        return false;
    }
    e_otherSender = IM_Kumir;
    e_me = IM_Program;
    e_state = CS_Sending;
    start();
    return true;
}



void Connector_SHM::listenFor(int pid)
{
#ifdef Q_OS_MAC
    return;
#endif
    QString key = QString("kumir-%1").arg(pid);
    shm->setKey(key);
    if (!shm->create(sizeof(InterprocessMessage))) {
        QString err = QString::fromAscii("Can't listen SHM: ")
                +shm->errorString();
        qFatal(err.toLocal8Bit().data());
    }
    qDebug() << "Listening for " << key;
    e_otherSender = IM_Program;
    e_me = IM_Kumir;
    currentFrame()->type = IM_NoMessage;
    e_state = CS_Receiving;
    connect(this, SIGNAL(requestReceived(QVariantList)), this, SLOT(handleStandardRequest(QVariantList)));
    start();
}

void Connector_SHM::output(const QString &text)
{
    sendAsynchronousRequest(QVariantList() << "output" << text);
}

QVariantList Connector_SHM::input(const QString &format)
{
    return sendSynchronousRequest(QVariantList() << "input" << format);
}

void Connector_SHM::error(const QString &message, int lineNo)
{
    sendAsynchronousRequest(QVariantList() << "error" << lineNo << message);
}

void Connector_SHM::resetActor(const QString &pluginName)
{
    sendAsynchronousRequest(QVariantList() << "reset" << pluginName);
}

QVariantList Connector_SHM::evaluateActorCommand(const QString &pluginName,
                                         const QString &command,
                                         const QVariantList &arguments)
{
    QVariantList message;
    message << "run" << pluginName << command;
    message += arguments;
    return sendSynchronousRequest(message);
}

void Connector_SHM::handleStandardRequest(const QVariantList &message)
{
    if (message.size()>0) {
        const QString operation = message[0].toString().toLower().trimmed();
        if (operation=="input") {
            Q_ASSERT(message.size()==2);
            emit inputFormatReceived(message[1].toString());
        }
        else if (operation=="output") {
            Q_ASSERT(message.size()==2);
            emit outputTextReceived(message[1].toString());
        }
        else if (operation=="error") {
            Q_ASSERT(message.size()==3);
            emit errorReceived(message[1].toInt());
            emit errorMessageReceived(message[2].toString());
        }
        else if (operation=="reset") {
            Q_ASSERT(message.size()==2);
            emit resetActorReceived(message[1].toString());
        }
        else if (operation=="run") {
            Q_ASSERT(message.size()>=3);
            const QString actor = message[1].toString();
            const QString command = message[2].toString();
            QVariantList arguments;
            if (message.size()>3) {
                arguments = message.mid(3);
            }
            emit actorCommandReceived(actor, command, arguments);
        }
    }
}


Connector_SHM::InterprocessMessage * Connector_SHM::currentFrame()
{
    Q_CHECK_PTR(shm->data());
    return static_cast<InterprocessMessage*>(shm->data());
}

void Connector_SHM::waitForStatus(MessageSender s)
{
    forever {
        shm->lock();
        if (currentFrame()->type==s) {
            return;
            // Do not unlock!!! We must keep captured data
        }
        else {
            shm->unlock();
            mutex_stopServing->lock();
            bool stop = b_stopServing;
            mutex_stopServing->unlock();
            if (stop)
                return;
            msleep(1);
        }
    }
}

void Connector_SHM::waitForReply()
{
    forever {
        mutex_reply->lock();
        if (!l_replyBuffer.isEmpty()) {
            return;
        }
        else {
            mutex_reply->unlock();
            mutex_stopServing->lock();
            bool stop = b_stopServing;
            mutex_stopServing->unlock();
            if (stop)
                return;
            msleep(1);
        }
    }
}

void Connector_SHM::waitForRequest()
{
    forever {
        mutex_request->lock();
        if (!q_requestBuffer.isEmpty()) {
            return;
        }
        else {
            mutex_request->unlock();
            mutex_stopOnEmpty->lock();
            bool stop = b_stopOnEmpty;
            mutex_stopOnEmpty->unlock();
            if (stop)
                return;
            msleep(1);
        }
    }
}

void Connector_SHM::sendReply(const QVariantList &message)
{
    QMutexLocker l(mutex_reply);
    l_replyBuffer = message;
}

void Connector_SHM::sendAsynchronousRequest(const QVariantList &message)
{
    Request req;
    req.data = message;
    req.replyRequired = false;
    QMutexLocker l(mutex_request);
    q_requestBuffer.enqueue(req);
}

QVariantList Connector_SHM::sendSynchronousRequest(const QVariantList &message)
{
    Request req;
    req.data = message;
    req.replyRequired = true;
    mutex_request->lock();
    q_requestBuffer.enqueue(req);
    mutex_request->unlock();
    mutex_reply->lock();
    l_replyBuffer.clear();
    mutex_reply->unlock();
    waitForReply();
    QVariantList result = l_replyBuffer;
    mutex_reply->unlock();
    return result;
}

void Connector_SHM::listenWorker()
{
    QByteArray buffer;
    forever {
        waitForStatus(e_otherSender);
        mutex_stopServing->lock();
        bool stop = b_stopServing;
        mutex_stopServing->unlock();
        if (stop) {
            shm->unlock();
            return;
        }
        int messageSize = currentFrame()->messageSize;
        buffer = QByteArray(currentFrame()->data, messageSize);
        if (!currentFrame()->replyRequired) {
            currentFrame()->type = IM_NoMessage;
            shm->unlock();
        }

        QDataStream ds(&buffer, QIODevice::ReadOnly);
        QVariantList response;
        ds >> response;
        buffer.clear();
        emit requestReceived(response);
        if (currentFrame()->replyRequired) {
            waitForReply();
            mutex_stopServing->lock();
            bool stop = b_stopServing;
            mutex_stopServing->unlock();
            if (stop) {
                shm->unlock();
                return;
            }
            QDataStream os(&buffer, QIODevice::WriteOnly);
            os << l_replyBuffer;
            l_replyBuffer.clear();
            mutex_reply->unlock();
            currentFrame()->type = e_me;
            currentFrame()->messageSize = buffer.size();
            qMemCopy(currentFrame()->data, buffer, buffer.size()*sizeof(char));
            shm->unlock();
        }
    }
}

void Connector_SHM::waitForEmptyAndStop()
{
    mutex_stopOnEmpty->lock();
    b_stopOnEmpty = true;
    mutex_stopOnEmpty->unlock();
    wait();
    shm->detach();
}

void Connector_SHM::stopListen()
{
    mutex_stopServing->lock();
    b_stopServing = true;
    mutex_stopServing->unlock();
    wait();
    shm->detach();
}

void Connector_SHM::requestWorker()
{
    QByteArray buffer;
    forever {
        waitForRequest();
        mutex_stopOnEmpty->lock();
        bool stop = b_stopOnEmpty && q_requestBuffer.isEmpty();
        mutex_stopOnEmpty->unlock();
        if (stop)
            return;
        QDataStream out(&buffer, QIODevice::WriteOnly);
        Request req = q_requestBuffer.dequeue();
        out << req.data;
        mutex_request->unlock();
        waitForStatus(IM_NoMessage);
        currentFrame()->messageSize = buffer.size();
        currentFrame()->type = e_me;
        qMemCopy(currentFrame()->data, buffer, buffer.size()*sizeof(char));
        buffer.clear();
        currentFrame()->replyRequired = req.replyRequired;
        shm->unlock();
        if (req.replyRequired) {
            waitForStatus(e_otherSender);
            int messageSize = currentFrame()->messageSize;
            buffer = QByteArray(currentFrame()->data, messageSize);
            currentFrame()->type = IM_NoMessage;
            shm->unlock();
            QDataStream ds(&buffer, QIODevice::ReadOnly);
            mutex_reply->lock();
            l_replyBuffer.clear();
            ds >> l_replyBuffer;
            mutex_reply->unlock();
            buffer.clear();
        }
    }
}

void Connector_SHM::run()
{
    if (e_state==CS_Sending) {
        requestWorker();
    }
    else if (e_state==CS_Receiving) {
        listenWorker();
    }
}



}

