#include "networkaccessmanager.h"
#include "localhostserver.h"

namespace Browser {

NetworkAccessManager::NetworkAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
    m_localhostServer = 0;
}

void NetworkAccessManager::setLocalhostServer(LocalhostServer *localhostServer)
{
    m_localhostServer = localhostServer;
}

QNetworkReply * NetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    if (request.url().host()=="localhost" && m_localhostServer) {
        return m_localhostServer->serveRequest(this, op, request, outgoingData);
    }
    else {
        return QNetworkAccessManager::createRequest(op, request, outgoingData);
    }
}

} // namespace Browser
