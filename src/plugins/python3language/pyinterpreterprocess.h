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
        None, Exit, Ping, Pong, BlockingCall, BlockingReturn, Exception,
        NonBlockingEval, StdOut, StdErr, InputRequest, InputResponse
    } type;

    explicit Message() : type(Type::None) {}

    explicit Message(const Type type_) : type(type_), asyncId(-1) {}
    explicit Message(const QByteArray &moduleName_,
                     const QByteArray &functionName_,
                     const QVariantList &arguments_) :
        type(Type::BlockingCall),
        moduleName(moduleName_),
        functionName(functionName_),
        arguments(arguments_), asyncId(-1) {}
    explicit Message(const QVariant &returnValue_) :
        type(Type::BlockingReturn),
        returnValue(returnValue_), asyncId(-1) {}
    explicit Message(const Type type_, const QString &stringData_) :
        type(type_), stringData(stringData_), asyncId(-1) {}
    explicit Message(const qint64 id, const QString &evalString) :
        type(Type::NonBlockingEval), stringData(evalString), asyncId(id) {}

    QByteArray moduleName;
    QByteArray functionName;
    QVariantList arguments;
    QVariant returnValue;
    QString stringData;
    qint64 asyncId;

};

class PyInterpreterProcess : public QProcess
{
    Q_OBJECT
public:
    static PyInterpreterProcess* create(QObject * parent = nullptr);
    bool waitForPong(int msec);
    QVariant blockingCall(const QByteArray &moduleName,
                          const QByteArray &functionName,
                          const QVariantList &arguments);



    void nonBlockingEval(const QString &evalString,
                         QObject * readyObject,
                         const char * readyMethod);

    void sendInput(const QString &data);

public slots:
    void sendPing();
    void sendExit();

signals:
    void stdoutReceived(const QString &message);
    void stderrReceived(const QString &message);
    void inputRequiestReceived(const QString &prompt);
    void resetReceived();


protected:
    explicit PyInterpreterProcess(QObject *parent = nullptr);
    bool launchProcess();
    void setupChildProcess();

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
    QMap<qint64, QPair<QObject*, QByteArray> > _registeredBlockingReceivers;

    static int DebugPortNumberOffset;
    static qint64 AsyncCallId;
};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_PYINTERPRETERPROCESS_H
