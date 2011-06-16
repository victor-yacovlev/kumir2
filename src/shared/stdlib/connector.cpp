#include "connector.h"

namespace StdLib {

Connector::Connector()
    : QThread()
{
    shm = new QSharedMemory(this);
    ba_buffer = new QByteArray;
    e_state = CS_Idle;
    e_me = e_otherSender = IM_NoMessage;
    InterprocessMessage f;
    PAGE_SIZE = sizeof(f.data) / sizeof(char);
}

int Connector::PAGE_SIZE = 0;
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
    connectedToKumir = true;
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
    ba_buffer->clear();
    connect(this, SIGNAL(requestReceived(QVariantList)), this, SLOT(handleStandardRequest(QVariantList)));
    e_state = CS_Receiving;
    start();
}


QVariantList Connector::sendRequest(const QVariantList &arguments)
{
    Q_ASSERT(e_state==CS_Idle);
    Q_ASSERT(!isRunning());
    ba_buffer->clear();
    QDataStream outbox(ba_buffer, QIODevice::WriteOnly);
    outbox << arguments;
    e_state = CS_Sending;
    start();
    wait();
    Q_ASSERT(e_state==CS_Idle);
    Q_ASSERT(!isRunning());
    e_state = CS_Receiving;
    ba_buffer->clear();
    start();
    wait();
    QDataStream inbox(ba_buffer, QIODevice::ReadOnly);
    QVariantList response;
    inbox >> response;
    return response;
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
            msleep(100);
        }
    }
}

void Connector::run()
{
    QByteArray frameBuffer;
    if (e_state==CS_Sending) {
        InterprocessMessage frame;
        frame.pagesCount = ba_buffer->size() / PAGE_SIZE + 1;
        int currentPage = 0;
        int pagesCount = ba_buffer->size() / PAGE_SIZE + 1;
        int offset = 0;
        for (currentPage = 0; currentPage < pagesCount; currentPage++) {
            offset = currentPage * PAGE_SIZE;
            frameBuffer = ba_buffer->mid(offset, PAGE_SIZE);
            waitForStatus(IM_NoMessage);
            currentFrame()->type = IM_Program;
            currentFrame()->currentPage = currentPage;
            currentFrame()->currentSize = frameBuffer.size();
            currentFrame()->pagesCount = pagesCount;
            qMemCopy(currentFrame()->data, frameBuffer.data(), frameBuffer.size() * sizeof(char));
            shm->unlock();
        }
        e_state = CS_Idle;
    }
    else if (e_state==CS_Receiving) {
        QByteArray okReply;
        QDataStream b(&okReply, QIODevice::WriteOnly);
        QVariantList okMessage;
        okMessage << "ok";
        b << okMessage;
        forever {
            waitForStatus(e_otherSender);
            int currentFrameSize = currentFrame()->currentSize;
            int currentFramePagesCount = currentFrame()->pagesCount;
            int currentFramePage = currentFrame()->currentPage;
            frameBuffer.setRawData(currentFrame()->data, currentFrameSize);
            ba_buffer->append(frameBuffer);
            currentFrame()->type = e_me;
            currentFrame()->currentSize = okReply.size();
            currentFrame()->currentPage = 0;
            currentFrame()->pagesCount = 1;
            qMemCopy(currentFrame()->data, okReply.data(), okReply.size() * sizeof(char));
            bool done = currentFramePage >= (currentFramePagesCount-1);
            shm->unlock();
            if (done) {
                if (e_otherSender==IM_Kumir)
                    break;
                else {
                    QDataStream ds(ba_buffer, QIODevice::ReadOnly);
                    QVariantList response;
                    ds >> response;
                    ba_buffer->clear();
                    emit requestReceived(response);
                }
            }
        }
        e_state = CS_Idle;
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
    }
}

}

extern "C" unsigned char __connected_to_kumir__()
{
    return StdLib::Connector::connectedToKumir? 1 : 0;
}

extern "C" STDLIB_EXPORT void __try_connect_to_kumir__(int argc, char* *argv)
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

extern void __show_actor_window__(const QString & moduleName)
{
    QVariantList message;
    message << "show";
    message << moduleName;
    StdLib::Connector::instance()->sendRequest(message);
}
