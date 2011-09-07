#ifndef CONNECTOR_BACKEND_H
#define CONNECTOR_BACKEND_H

#include <QtCore>

namespace StdLib {

class AbstractConnectorBackend
        : public QThread
{
    Q_OBJECT
public:
    inline explicit AbstractConnectorBackend(QObject * parent)
        : QThread(parent) {}

public slots:
    virtual bool connectTo(int argc, char* *argv) = 0;
    virtual void listenFor(QProcess * process) = 0;

    virtual void sendReply(const QVariantList & message) = 0;

    virtual void output(const QString & text) = 0;
    virtual QVariantList input(const QString & format) = 0;
    virtual void error(const QString & message, int lineNo) = 0;
    virtual void resetActor(const QString & pluginName) = 0;

    // Returns : [0] - error, [1] - result, [2..N-1] - opt results
    virtual QVariantList evaluateActorCommand(const QString & pluginName,
                                      const QString & command,
                                      const QVariantList & arguments) = 0;

    virtual void waitForEmptyAndStop() = 0;
    virtual void stopListen() = 0;
signals:
    void requestReceived(const QVariantList & message);
    void outputTextReceived(const QString & text);
    void errorMessageReceived(const QString & message);
    void inputFormatReceived(const QString & format);
    void errorReceived(int lineNo);
    void resetActorReceived(const QString & name);
    void actorCommandReceived(const QString & actorName,
                              const QString & command,
                              const QVariantList & arguments);
};

}


#endif // CONNECTOR_BACKEND_H
