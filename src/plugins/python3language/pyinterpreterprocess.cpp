#include "pyinterpreterprocess.h"

#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QProcessEnvironment>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QApplication>
#include <QMessageBox>

#ifdef Q_OS_UNIX
#   include <signal.h>
#   include <unistd.h>
#endif
#ifdef Q_OS_LINUX
#   include <sys/prctl.h>
#endif

namespace Python3Language {

int PyInterpreterProcess::DebugPortNumberOffset = 0;
qint64 PyInterpreterProcess::AsyncCallId = 0;

PyInterpreterProcess *PyInterpreterProcess::create(bool autoRespawn, QObject *parent)
{
    PyInterpreterProcess *result = new PyInterpreterProcess(autoRespawn, parent);
    if (result->launchProcess()) {
        QVariant pythonCwd = result->blockingCall("os", "getcwd", QVariantList());
        qDebug() << "Python working directory: " << pythonCwd.toString();
        return result;
    }
    else {
        result->deleteLater();
        return nullptr;
    }
}

void PyInterpreterProcess::sendPing()
{
    sendMessage(Message (Message::Type::Ping) );
}

void PyInterpreterProcess::sendExit()
{
    _allowProcessRespawn = false;
    sendMessage(Message (Message::Type::Exit) );
    DebugPortNumberOffset = qMax(0, DebugPortNumberOffset-1);
}

bool PyInterpreterProcess::waitForPong(int msec)
{
    Message pong = waitForMessage(Message::Type::Pong, msec);
    return Message::Type::Pong == pong.type;
}

QVariant PyInterpreterProcess::blockingCall(const QByteArray &moduleName, const QByteArray &functionName, const QVariantList &arguments)
{
    sendMessage(Message(moduleName, functionName, arguments));
    Message response = waitForMessage(Message::Type::BlockingReturn, -1);
    return response.returnValue;
}

void PyInterpreterProcess::nonBlockingEval(const QString &evalString, QObject *readyObject, const char *readyMethod)
{
    QPair<QObject*, QByteArray> readyReceiver(readyObject, readyMethod);
    _registeredBlockingReceivers[++AsyncCallId] = readyReceiver;
    Message request(AsyncCallId, evalString);
    sendMessage(request);
}

void PyInterpreterProcess::sendInput(const QString &data)
{
    sendMessage(Message(Message::Type::InputResponse, data));
}



PyInterpreterProcess::PyInterpreterProcess(bool autoRespawn, QObject * parent)
    : QProcess(parent)
    , _allowProcessRespawn(autoRespawn)
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString path = pythonExtraPath();
#ifdef Q_OS_UNIX
    static const QString SEP = ":";
#else
    static const QString SEP = ";";
#endif
    if (env.contains("PYTHONPATH")) {
        path += SEP + env.value("PYTHONPATH");
    }
    env.insert("PYTHONPATH", path);
    const QString basePortNumber = env.value("PY_KUMIR_DEBUG_PORT_START_NUMBER");
    if (!basePortNumber.isEmpty()) {
        int portNumber = basePortNumber.toInt() + DebugPortNumberOffset;
        DebugPortNumberOffset++;
        qDebug() << "Python interpreter will attach to PyCharm debugger at port " << portNumber;
        env.insert("PY_KUMIR_DEBUG_PORT", QString::number(portNumber));
    }
    setProcessEnvironment(env);
    setProcessChannelMode(QProcess::ProcessChannelMode::SeparateChannels);
    connect(this, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleReadStandardOutput()));
    connect(this, SIGNAL(readyReadStandardError()),
            this, SLOT(handleReadStandardError()));
    connect(qApp, SIGNAL(aboutToQuit()),
            this, SLOT(sendExit()));
    connect(this, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(handleProcessFinished(int,QProcess::ExitStatus)));
}

bool PyInterpreterProcess::launchProcess()
{
    start(pythonExecutablePath(), {"-m", "sandbox_bridge"});
    waitForStarted();        
    if (QProcess::ProcessState::Running == state()) {
        sendPing();
        if (waitForPong(20000)) {
            return true;
        }
        else {
            qDebug() << "Python subprocess does not respond to ping request";
            return false;
        }
    }
    else {
        qDebug() << "Error staring python process";
        return false;
    }
}

void PyInterpreterProcess::setupChildProcess()
{
#ifdef Q_OS_LINUX
    ::prctl(PR_SET_PDEATHSIG, SIGKILL);
#endif
}

void PyInterpreterProcess::sendMessage(const Message &message)
{
    QJsonObject obj;
    switch (message.type) {
    case Message::Type::Ping:
        obj["type"] = "ping";
        break;
    case Message::Type::Exit:
        obj["type"] = "exit";
        break;
    case Message::Type::BlockingCall:
        obj["type"] = "blocking_call";
        obj["module_name"] = QString::fromLatin1(message.moduleName);
        obj["function_name"] = QString::fromLatin1(message.functionName);
        obj["arguments"] = QJsonArray::fromVariantList(message.arguments);
        break;
    case Message::Type::NonBlockingEval:
        obj["type"] = "non_blocking_eval";
        obj["eval_string"] = message.stringData;
        obj["async_id"] = message.asyncId;
        break;
    case Message::Type::InputResponse:
        obj["type"] = "input_response";
        obj["data"] = message.stringData;
        break;
    }

    const QByteArray raw = QJsonDocument(obj)
            .toJson(QJsonDocument::JsonFormat::Compact);
    write(raw + "\n");
    waitForBytesWritten(5000);
}

Message PyInterpreterProcess::waitForMessage(Message::Type waitType, int msec)
{
    Message result;
    int quantumNo = 1;
    const int quantumCount = msec / 500;
    while (quantumNo <= quantumCount || -1==msec) {
        if (Message::Type::None != result.type) break;
        _incomingMessagesMutex.lock();
        if (!_incomingMessages.isEmpty() &&
                (waitType==_incomingMessages.head().type || Message::Type::Exception==_incomingMessages.head().type)
                )
        {
            result = _incomingMessages.dequeue();
            if (Message::Type::Exception==result.type) {
#ifdef Q_OS_WIN32
                QMessageBox::critical(0, "Python exception", result.stringData);
#else
                qDebug() << "Python exception: " << result.stringData;
#endif
            }
        }
        _incomingMessagesMutex.unlock();
        QApplication::processEvents();
        handleReadStandardOutput();
        QThread::msleep(500);
        if (-1 != msec) {
            quantumNo++;
        }
    }
    return result;
}

QString PyInterpreterProcess::pythonExecutablePath()
{
    return "/usr/bin/python3";
}

QString PyInterpreterProcess::pythonExtraPath()
{
    QStringList targets = QStringList() << ".";
    const QString binPath = QCoreApplication::applicationDirPath();
    const QString pyPath = binPath + "/../share/kumir2/python3language/";
    QDir thirdParty(pyPath + "3rd-party");
    QList<QFileInfo> thirdPartyEntries = thirdParty.entryInfoList(QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files);
    Q_FOREACH(const QFileInfo & entryInfo, thirdPartyEntries) {
        if (entryInfo.isDir() || entryInfo.fileName().endsWith(".egg")) {
            targets.append("3rd-party/" + entryInfo.fileName());
        }
    }

#ifdef Q_OS_UNIX
    static const QString SEP = ":";
#else
    static const QString SEP = ";";
#endif
    QString path;
    Q_FOREACH(const QString &target, targets) {        
        const QString targetPath = pyPath + target;
        const QString normPath = QDir::toNativeSeparators(QDir::cleanPath(targetPath));
        if ( ! path.isEmpty()) path += SEP;
        path += normPath;
    }
    return path;
}

void PyInterpreterProcess::handleReadStandardOutput()
{
    _processStdOutLastLineBuffer += readAllStandardOutput();
    QList<QByteArray> lines = _processStdOutLastLineBuffer.split('\n');
    if (! _processStdOutLastLineBuffer.endsWith('\n')) {
        _processStdOutLastLineBuffer = lines.last();
        lines.pop_back();
    }
    else {
        _processStdOutLastLineBuffer.clear();
    }
    Q_FOREACH(const QByteArray &line, lines) {
        if (!line.isEmpty()) {
            QJsonParseError parseError;
            QJsonDocument env = QJsonDocument::fromJson(line, &parseError);
            if (!env.isNull()) {
                QJsonObject obj = env.object();
                const QString type = obj["type"].toString().toLower().trimmed();
                if ("pong" == type) {
                    _incomingMessagesMutex.lock();
                    _incomingMessages.enqueue(Message (Message::Type::Pong) );
                    _incomingMessagesMutex.unlock();
                }
                else if ("blocking_return" == type) {
                    _incomingMessagesMutex.lock();
                    _incomingMessages.enqueue(Message (obj["return_value"].toVariant()));
                    _incomingMessagesMutex.unlock();
                }
                else if ("exception" == type) {
                    if (obj.contains("async_id")) {
                        qint64 id = obj["async_id"].toVariant().toLongLong();
                        QString repr = obj["string_data"].toString();
                        if (_registeredBlockingReceivers.contains(id)) {
                            QPair<QObject*, QByteArray> receiver = _registeredBlockingReceivers[id];
                            _registeredBlockingReceivers.remove(id);
                            QMetaObject::invokeMethod(receiver.first, receiver.second.constData(),
                                                      Qt::DirectConnection,
                                                      Q_ARG(bool, false),
                                                      Q_ARG(QString, repr));
                        }
                    }
                    else {
                        _incomingMessagesMutex.lock();
                        _incomingMessages.enqueue(Message (Message::Type::Exception, obj["string_data"].toString()));
                        _incomingMessagesMutex.unlock();
                    }
                }
                else if ("eval_return" == type) {
                    qint64 id = obj["async_id"].toVariant().toLongLong();
                    bool moreLinesRequired = obj["more_lines_required"].toBool();
                    if (_registeredBlockingReceivers.contains(id)) {
                        QPair<QObject*, QByteArray> receiver = _registeredBlockingReceivers[id];
                        _registeredBlockingReceivers.remove(id);
                        QMetaObject::invokeMethod(receiver.first, receiver.second.constData(),
                                                  Qt::DirectConnection,
                                                  Q_ARG(bool, moreLinesRequired));
                    }
                }
                else if ("stdout" == type || "stderr" == type) {
                    QString msg = obj["string_data"].toString();
                    if ("stdout" == type) {
                        emit stdoutReceived(msg);
                    }
                    else {
                        emit stderrReceived(msg);
                    }
                }
                else if ("input_request" == type) {
                    const QString prompt = obj["prompt"].toString();
                    emit inputRequiestReceived(prompt);
                }
                else if ("reset" == type) {
                    _registeredBlockingReceivers.clear();
                    emit resetReceived();
                }
            }
            else {
                qDebug() << "Error parsing incoming message: " << parseError.errorString();
            }
        }
    }
}

void PyInterpreterProcess::handleReadStandardError()
{
    QStringList lines = QString::fromUtf8(readAllStandardError()).split("\n");
    Q_FOREACH( const QString stderrData, lines ) {
        qDebug() << "StdErr from Python process: " << stderrData;
    }
}

void PyInterpreterProcess::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (_allowProcessRespawn) {
        launchProcess();
        emit processRespawned(exitCode, exitStatus);
    }
}

} // namespace Python3Language
