#include "connector.h"

namespace StdLib {

Connector::Connector()
    : QThread()
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

bool Connector::connectedToKumir = false;

Connector* Connector::instance() {
    if (m_instance==0)
        m_instance = new Connector;
    return m_instance;
}

Connector* Connector::m_instance = 0;

void Connector::connectTo(const QString &key)
{
    shm->setKey(key);
    if (!shm->attach()) {
        qFatal(QString("Can't attach to Kumir: %1").arg(shm->errorString()).toLocal8Bit().data());
    }
    e_otherSender = IM_Kumir;
    e_me = IM_Program;
    e_state = CS_Sending;
    connectedToKumir = true;
    start();
}

void Connector::listenFor(const QString &key)
{
    shm->setKey(key);
    if (!shm->create(sizeof(InterprocessMessage))) {
        qFatal(QString("Can't listen SHM: %1").arg(shm->errorString()).toLocal8Bit().data());
    }
    qDebug() << "Listening for " << key;
    e_otherSender = IM_Program;
    e_me = IM_Kumir;
    currentFrame()->type = IM_NoMessage;
    connect(this, SIGNAL(requestReceived(QVariantList)), this, SLOT(handleStandardRequest(QVariantList)));
    e_state = CS_Receiving;
    start();
}


Connector::InterprocessMessage * Connector::currentFrame()
{
    Q_CHECK_PTR(shm->data());
    return static_cast<InterprocessMessage*>(shm->data());
}

void Connector::waitForStatus(MessageSender s)
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

void Connector::waitForReply()
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

void Connector::waitForRequest()
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

void Connector::sendReply(const QVariantList &message)
{
    QMutexLocker l(mutex_reply);
    l_replyBuffer = message;
}

void Connector::sendAsynchronousRequest(const QVariantList &message)
{
    Request req;
    req.data = message;
    req.replyRequired = false;
    QMutexLocker l(mutex_request);
    q_requestBuffer.enqueue(req);
}

QVariantList Connector::sendSynchronousRequest(const QVariantList &message)
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

void Connector::listenWorker()
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
        buffer.setRawData(currentFrame()->data, messageSize);
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

void Connector::waitForEmptyAndStop()
{
    mutex_stopOnEmpty->lock();
    b_stopOnEmpty = true;
    mutex_stopOnEmpty->unlock();
    wait();
    shm->detach();
}

void Connector::stopListen()
{
    mutex_stopServing->lock();
    b_stopServing = true;
    mutex_stopServing->unlock();
    wait();
    shm->detach();
}

void Connector::requestWorker()
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
            buffer.setRawData(currentFrame()->data, messageSize);
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

void Connector::run()
{
    if (e_state==CS_Sending) {
        requestWorker();
    }
    else if (e_state==CS_Receiving) {
        listenWorker();
    }
}

void Connector::handleStandardRequest(const QVariantList &message)
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

}

extern "C" unsigned char __connected_to_kumir__()
{
    return StdLib::Connector::connectedToKumir? 1 : 0;
}

extern "C" void __try_connect_to_kumir__(int argc, char* *argv)
{
    for (int i=1; i<argc; i++) {
        const QString arg = QString::fromAscii(argv[i]);
        if (arg.startsWith("--key=")) {
            const QString key = arg.mid(6);
            qDebug() << "Connecting to kumir with key = "  << key;
            __connect_to_kumir__(arg.mid(6));
            break;
        }
    }
}

extern void __connect_to_kumir__(const QString & key)
{
    StdLib::Connector::instance()->connectTo(key);
}

extern void __reset_actor__(const QString & moduleName)
{
    QVariantList message;
    message << "reset";
    message << moduleName;
    StdLib::Connector::instance()->sendAsynchronousRequest(message);
}

extern ActorResponse __run_actor_command__(
    const QString & actor,
    const QString & command,
    const QVariantList & arguments
    )
{
    QVariantList message;
    message << "run";
    message << actor;
    message << command;
    message += arguments;
    QVariantList response = StdLib::Connector::instance()->sendSynchronousRequest(message);
    Q_ASSERT(response.size()>=1);
    ActorResponse result;
    result.error = response[0].toString();
    if (response.size()>1) {
        result.result = response[1];
    }
    if (response.size()>2) {
        result.res = response.mid(2);
    }
    return result;
}

extern "C" void __wait_for_output_queue_flushed__()
{
    if (__connected_to_kumir__()) {
        StdLib::Connector::instance()->waitForEmptyAndStop();
    }
}
