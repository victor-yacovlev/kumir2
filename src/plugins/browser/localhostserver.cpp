#include "localhostserver.h"

namespace Browser {


class NetworkReply
        : public QNetworkReply
{
public :
    explicit NetworkReply(const ServerResponse & response, const QNetworkRequest & request, QObject * parent = 0);
    qint64 bytesAvailable() const;
    inline bool isSequential() const { return true; }
    inline void abort() { }
protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 position;
private:
    QByteArray data;
    QString contentType;
};

qint64 NetworkReply::readData(char *buffer, qint64 maxlen)
{
    qint64 cnt = qMin<qint64>(maxlen, data.size()-position);
    for (int i=0; i<cnt; i++) {
        const char ch = data.at(position+i);
        buffer[i] = ch;
    }
    position += cnt;
    return cnt;
}

qint64 NetworkReply::bytesAvailable() const
{
    return data.size();
}

NetworkReply::NetworkReply(const ServerResponse &response, const QNetworkRequest &, QObject *parent)
    : QNetworkReply(parent)
{
    QNetworkReply::open(QIODevice::ReadOnly);
    position = 0;
    contentType = response.mimeType;
    if (response.code==200) {
        data = response.data;
    }
    else {
        QString error = QString("<html><head><title>Error %1</title></head><body><h1>Error %1</h1><p>%2</p></body></html>")
                .arg(response.code)
                .arg(response.errorText);
        data = error.toUtf8();
    }
    QUrl qualifedUrl;
    qualifedUrl.setScheme("http");
    qualifedUrl.setHost("localhost");
    qualifedUrl.setPath(response.uri);
    setUrl(qualifedUrl);
    setHeader(QNetworkRequest::LastModifiedHeader, QDateTime::currentDateTime());
    setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    setHeader(QNetworkRequest::ContentTypeHeader, response.mimeType);
    setHeader(QNetworkRequest::LocationHeader, qualifedUrl);
    setRawHeader(QByteArray("Accept-Ranges"), QByteArray("bytes"));
    setRawHeader(QByteArray("Server"), QByteArray("Kumir Localhost Server ")+qApp->applicationVersion().toAscii());
    setOperation(QNetworkAccessManager::GetOperation);

    QMetaObject::invokeMethod(this, "metaDataChanged", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "downloadProgress", Qt::QueuedConnection,
                              Q_ARG(qint64, data.size()), Q_ARG(qint64, data.size()));
    QMetaObject::invokeMethod(this, "readyRead", Qt::QueuedConnection);
    QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection);
}

LocalhostServer::LocalhostServer(const QDir & webAppsRoot, QObject *parent)
    : QObject(parent)
    , m_webAppsRoot(webAppsRoot)
{

}

ServerResponse LocalhostServer::GET(const QUrl &url) {
    const QString path = "/"+url.path();
    ServerResponse resp;
    QString fileName = m_webAppsRoot.absolutePath()+"/"+path;
    while (fileName.contains("//")) {
        fileName.replace("//","/");
    }
    if (fileName.endsWith("/")) {
        fileName += "index.html";
    }
    if (QFile::exists(fileName)) {
        QFile f(fileName);
        if (f.open(QIODevice::ReadOnly)) {
            resp.data = f.readAll();
            resp.code = 200;
            f.close();
        }
        else {
            resp.code = 503;
            resp.errorText = "File access denied: "+fileName;
        }
    }
    else {
        resp.code = 404;
        resp.errorText = "File not found: "+fileName;
    }
    if (fileName.endsWith(".js")) {
        resp.mimeType = "text/javascript";
    }
    else if (fileName.endsWith(".json")) {
        resp.mimeType = "text/json";
    }
    else if (fileName.endsWith(".svg")) {
        resp.mimeType = "image/svg+xml";
    }
    else if (fileName.endsWith(".css")) {
        resp.mimeType = "text/css";
    }
    else if (fileName.endsWith(".png")) {
        resp.mimeType = "image/png";
    }
    else if (fileName.endsWith(".jpg")||fileName.endsWith(".jpeg")) {
        resp.mimeType = "image/jpeg";
    }
    else if (fileName.endsWith(".gif")) {
        resp.mimeType = "image/gif";
    }
    else if (fileName.endsWith(".xml")) {
        resp.mimeType = "text/xml";
    }
    else {
        resp.mimeType = "text/html";
    }
    resp.uri = fileName.mid(m_webAppsRoot.path().length());
    return resp;
}

QNetworkReply * LocalhostServer::serveRequest(QNetworkAccessManager * manager, QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    ServerResponse resp;
    if (op==QNetworkAccessManager::PostOperation) {
        QByteArray postData = outgoingData->readAll();
//        resp = d->POST(request.url(), postData);
//        qDebug() << "Serving POST operation";
        Q_UNUSED(postData); resp = GET(request.url()); // TODO implement POST method
    }
    else {
//        qDebug() << "Serving GET operation";
        resp = GET(request.url());
    }
//    qDebug() << "Creating reply";
    NetworkReply * reply = new NetworkReply(resp, request, manager);
//    qDebug() << "Sending reply";
    return reply;
}

} // namespace Browser
