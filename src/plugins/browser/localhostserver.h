#ifndef BROWSER_LOCALHOSTSERVER_H
#define BROWSER_LOCALHOSTSERVER_H

#include <QtCore>
#include <QtNetwork>

namespace Browser {

struct ServerResponse
{
    QString uri;
    QString mimeType;
    QByteArray data;
    int code;
    QString errorText;
};

class LocalhostServer : public QObject
{
    Q_OBJECT
public:
    explicit LocalhostServer(const QDir & webAppsRoot, QObject *parent = 0);
    QNetworkReply * serveRequest(QNetworkAccessManager * manager,
                                 QNetworkAccessManager::Operation op,
                                 const QNetworkRequest & request,
                                 QIODevice * outgoingData
                                 );

signals:

public slots:

private:
    ServerResponse GET(const QUrl & url);
    QDir m_webAppsRoot;
};

} // namespace Browser

#endif // BROWSER_LOCALHOSTSERVER_H
