#include "connector.h"

#ifdef SHM_METHOD
#   include "connector_shm.h"
#endif
#ifdef PIPE_METHOD
#   include "connector_pipe.h"
#endif


namespace StdLib {

bool Connector::connectedToKumir = false;

Connector::Connector()
    : QObject(0)
{
#ifdef SHM_METHOD
    d = new Connector_SHM(this);
#endif
#ifdef PIPE_METHOD
    d = new Connector_PIPE(this);
#endif


    connect(d, SIGNAL(outputTextReceived(QString)),
            this, SIGNAL(outputTextReceived(QString)));

    connect(d, SIGNAL(errorMessageReceived(QString)),
            this, SIGNAL(errorMessageReceived(QString)));

    connect(d, SIGNAL(errorReceived(int)),
            this, SIGNAL(errorReceived(int)));

    connect(d, SIGNAL(inputFormatReceived(QString)),
            this, SIGNAL(inputFormatReceived(QString)));

    connect(d, SIGNAL(resetActorReceived(QString)),
            this, SLOT(resetActor(QString)));

    connect(d, SIGNAL(actorCommandReceived(QString,QString,QVariantList)),
            this, SIGNAL(actorCommandReceived(QString,QString,QVariantList)));
}

void Connector::listenFor(QProcess * process)
{
    d->listenFor(process);
}

void Connector::connectToKumir(int argc, char* *argv)
{
    connectedToKumir = d->connectTo(argc, argv);
}



void Connector::sendReply(const QVariantList &message)
{
    d->sendReply(message);
}

void Connector::output(const QString & text)
{
    d->output(text);
}

QVariantList Connector::input(const QString & format)
{
    return d->input(format);
}

void Connector::error(const QString & message, int lineNo)
{
    d->error(message, lineNo);
}

void Connector::resetActor(const QString & pluginName)
{
    d->resetActor(pluginName);
}

QVariantList Connector::evaluateActorCommand(const QString & pluginName,
                                  const QString & command,
                                  const QVariantList & arguments)
{
    return d->evaluateActorCommand(pluginName, command, arguments);
}


void Connector::waitForEmptyAndStop()
{
    d->waitForEmptyAndStop();
}

void Connector::stopListen()
{
    d->stopListen();
}



Connector* Connector::instance() {
    if (m_instance==0)
        m_instance = new Connector;
    return m_instance;
}

Connector* Connector::m_instance = 0;

}

extern "C" unsigned char __connected_to_kumir__()
{
    return StdLib::Connector::connectedToKumir? 1 : 0;
}

extern "C" void __try_connect_to_kumir__(int argc, char* *argv)
{
    __connect_to_kumir__(argc, argv);
}

extern void __connect_to_kumir__(int argc, char* *argv)
{
    StdLib::Connector::instance()->connectToKumir(argc, argv);
}

extern void __reset_actor__(const QString & moduleName)
{
    StdLib::Connector::instance()->resetActor(moduleName);
}

extern ActorResponse __run_actor_command__(
    const QString & actor,
    const QString & command,
    const QVariantList & arguments
    )
{
    QVariantList response = StdLib::Connector::instance()->evaluateActorCommand(actor, command, arguments);
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
