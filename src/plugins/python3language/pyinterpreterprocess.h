#ifndef PYTHON3LANGUAGE_PYINTERPRETERPROCESS_H
#define PYTHON3LANGUAGE_PYINTERPRETERPROCESS_H

#include <QObject>
#include <QJsonDocument>
#include <QProcess>
#include <QByteArray>
#include <QQueue>
#include <QMutex>
#include <QVariant>
#include <QVariantList>

namespace Python3Language {

struct Message {
    enum class Type {
        None, Exit, Ping, Pong, BlockingCall, BlockingReturn, Exception
    } type;

    explicit Message() : type(Type::None) {}
    explicit Message(const Type type_) : type(type_) {}
    explicit Message(const QByteArray &moduleName_,
                     const QByteArray &functionName_,
                     const QVariantList &arguments_) :
        type(Type::BlockingCall),
        moduleName(moduleName_),
        functionName(functionName_),
        arguments(arguments_) {}
    explicit Message(const QVariant &returnValue_) :
        type(Type::BlockingReturn),
        returnValue(returnValue_) {}
    explicit Message(const Type type_, const QString &stringData_) :
        type(type_), stringData(stringData_) {}

    QByteArray moduleName;
    QByteArray functionName;
    QVariantList arguments;
    QVariant returnValue;
    QString stringData;

};

class PyInterpreterProcess : protected QProcess
{
    Q_OBJECT
public:
    static PyInterpreterProcess* create(QObject * parent = nullptr);
    bool waitForPong(int msec);
    QVariant blockingCall(const QByteArray &moduleName,
                          const QByteArray &functionName,
                          const QVariantList &arguments);

public slots:
    void sendPing();
    void sendExit();


protected:
    explicit PyInterpreterProcess(QObject *parent = nullptr);
    bool launchProcess();

    void sendMessage(const Message &message);
    Message waitForMessage(Message::Type waitType, int msec);



    static QString pythonExecutablePath();
    static QString pythonExtraPath();

protected slots:
    void handleReadStandardOutput();
    void handleReadStandardError();

protected /* fields */:
    QByteArray _processStdOutLastLineBuffer;
    QQueue<Message> _incomingMessages;
    QMutex _incomingMessagesMutex;

    static int DebugPortNumberOffset;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_PYINTERPRETERPROCESS_H
