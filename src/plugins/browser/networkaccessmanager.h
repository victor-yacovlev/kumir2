#ifndef BROWSER_NETWORKACCESSMANAGER_H
#define BROWSER_NETWORKACCESSMANAGER_H

#include <QtCore>
#include <QtNetwork>

namespace Browser {

class LocalhostServer;

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    NetworkAccessManager(QObject * parent = 0);
    void setLocalhostServer(LocalhostServer * localhostServer);
protected:
    QNetworkReply * createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
private:
    LocalhostServer * m_localhostServer;

};

} // namespace Browser

#endif // BROWSER_NETWORKACCESSMANAGER_H
