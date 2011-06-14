#include "connector.h"

namespace StdLib {

Connector::Connector()
    : QThread()
{
    shm = new QSharedMemory(this);
    ba_buffer = new QByteArray;
    e_state = CS_Idle;
    e_otherSender = IM_NoMessage;
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
    connectedToKumir = true;
}

void Connector::listenFor(const QString &key)
{
    shm->setKey(key);
    if (!shm->create(sizeof(InterprocessMessage))) {
        qFatal(QString("Can't listen SHM: %1").arg(shm->errorString()).toLocal8Bit().data());
    }
    e_otherSender = IM_Program;
    currentFrame()->type = IM_NoMessage;
    ba_buffer->clear();
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
        forever {
            waitForStatus(e_otherSender);
            frameBuffer.setRawData(currentFrame()->data, currentFrame()->currentSize);
            ba_buffer->append(frameBuffer);
            currentFrame()->type = IM_NoMessage;
            bool done = currentFrame()->currentPage >= currentFrame()->pagesCount;
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

extern "C" unsigned char __connected_to_kumir__()
{
    return StdLib::Connector::connectedToKumir? 1 : 0;
}

} // namespace GuiRunner
