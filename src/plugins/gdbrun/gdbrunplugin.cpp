#include "gdbrunplugin.h"

namespace GdbRun {

GdbRunPlugin::GdbRunPlugin()
    : gdbClient_(0)
    , gdbServer_(0)
    , variablesModel_(0)
    , inferiorPid_(Q_PID())
    , gdbState_(NotStarted)
    , gdbStateLocker_(new QMutex)
    , stateWaiter_(new StateWaiter(this))
    , gdbCommandsQueueLocker_(new QMutex)
    , symbolsLoaded_(false)
    , breakLine1Inserted_(false)
{
    connect(this, SIGNAL(finishInput(QVariantList)),
            this, SLOT(handleInputDone(QVariantList)));
}

QString GdbRunPlugin::initialize(const QStringList &conf, const CommandLine &)
{
    gdbClient_ = new QProcess(this);
    gdbClient_->setProcessChannelMode(QProcess::SeparateChannels);
    connect(gdbClient_, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleGdbClientReadStdOut()), Qt::DirectConnection);
    connect(gdbClient_, SIGNAL(readyReadStandardError()),
            this, SLOT(handleGdbClientReadStdErr()), Qt::DirectConnection);


    gdbServer_ = new QProcess(this);
    gdbServer_->setProcessChannelMode(QProcess::SeparateChannels);
    connect(gdbServer_, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleGdbServerReadStdOut()), Qt::DirectConnection);
    connect(gdbServer_, SIGNAL(readyReadStandardError()),
            this, SLOT(handleGdbServerReadStdErr()), Qt::DirectConnection);

    variablesModel_ = 0;
    QString ioCodec;
    for (int i=0; i<conf.size(); i++) {
        if (conf[i].toLower().startsWith("io=")) {
            ioCodec = conf[i].mid(3).toUpper();
        }
    }
    if (ioCodec.length() > 0) {
        ioCodec_ = QTextCodec::codecForName(ioCodec.toAscii());
    }
    else {
        ioCodec_ = QTextCodec::codecForLocale();
    }
    return "";
}

void GdbRunPlugin::updateSettings(const QStringList &)
{

}

bool GdbRunPlugin::loadProgram(const QString &fileName, const QByteArray &)
{
    breakLine1Inserted_ = symbolsLoaded_ = false;
    inferiorPid_ = Q_PID();
    if (QProcess::Running == gdbServer_->state()) {
        gdbServer_->terminate();
        gdbServer_->waitForFinished();
    }
//    if (QProcess::Running == gdbClient_->state()) {
//        gdbClient_->terminate();
//        gdbClient_->waitForFinished();
//    }
    if (variablesModel_) {
        variablesModel_->deleteLater();
    }
    QString comm = "localhost:0";
    QStringList gdbServerArguemnts = QStringList()
            << comm
            << QDir::toNativeSeparators(fileName)
               ;
    gdbServer_->start(gdbServerCommand(), gdbServerArguemnts);
    gdbServer_->waitForStarted();

    programFileName_ = fileName;
    variablesModel_ = new QStandardItemModel(this);    
    bool ok = QProcess::Running == gdbServer_->state();
    QMutexLocker lock(gdbStateLocker_);
    gdbState_ = ok? StartedServer : NotStarted;
    return ok;
}

QString GdbRunPlugin::gdbCommand()
{
#ifdef Q_OS_WIN32
    QString path = QCoreApplication::applicationDirPath() +
            "/../mingw/bin/gdb.exe";
    path = QDir::cleanPath(path);
    path = QDir::toNativeSeparators(path);
    return path;
#else
    return "gdb";
#endif
}

QString GdbRunPlugin::gdbServerCommand()
{
#ifdef Q_OS_WIN32
    QString path = QCoreApplication::applicationDirPath() +
            "/../mingw/bin/gdbserver.exe";
    path = QDir::cleanPath(path);
    path = QDir::toNativeSeparators(path);
    return path;
#else
    return "gdbserver";
#endif
}


QDateTime GdbRunPlugin::loadedProgramVersion() const
{
    if (programFileName_.length() == 0 || !QFile(programFileName_).exists()) {
        return QDateTime();
    }
    else {
        QFileInfo fi(programFileName_);
        return fi.lastModified();
    }
}

void GdbRunPlugin::handleInputDone(const QVariantList &data)
{
#ifdef Q_OS_WIN32
    static const QByteArray NL = "\r\n";
#else
    static const QByteArray NL = "\n";
#endif
    QString inputString;
    for (int i=0; i<data.size(); i++) {
        QString lexem = data[i].toString().trimmed();
        if (lexem.length() == 0 && data[i].type() == QVariant::Char)
            lexem = " ";
        inputString += lexem;
        if (i<data.size()-1)
            inputString += " ";
    }
    const QByteArray rawString = ioCodec_->fromUnicode(inputString) + NL;
    gdbServer_->write(rawString);
    gdbServer_->waitForBytesWritten();
    sendGdbCommand("exec-continue");
    gdbState_ = Running;
}

void GdbRunPlugin::handleGdbServerReadStdErr()
{
    QByteArray serverOut = gdbServer_->readAllStandardError().replace('\r',"");
    if (StartedServer == gdbState_ && QProcess::Running==gdbServer_->state()) {
        // take pid and port name, and start gdb client
        QStringList gdbClientArguments = QStringList()
                << "--interpreter=mi"
                   ;
        if (QProcess::Running != gdbClient_->state()) {
            gdbClient_->start(gdbCommand(), gdbClientArguments);
            gdbClient_->waitForStarted();
        }
        gdbState_ = StartedBoth;

        QString gdbServerOutput = QString::fromAscii(serverOut);
        QRegExp rxPortNum("Listening on port (\\d+)");
        QString portNumber = "0";
        if (-1 != rxPortNum.indexIn(gdbServerOutput)) {
            portNumber = rxPortNum.cap(1);
        }
        QByteArray comm = "localhost:" + portNumber.toAscii();
        queueGdbCommand("target-select remote "+comm,
                        StartedBoth);
//        queueGdbCommand("exec-arguments >"+QDir::toNativeSeparators(programFileName_+".out").toLocal8Bit(), StartedBoth);
        QByteArray symbolFile = QDir::toNativeSeparators(programFileName_).toLocal8Bit();
#ifdef Q_OS_WIN32
        symbolFile.replace("\\", "\\\\");
#endif
        queueGdbCommand("file-symbol-file "+symbolFile,
                        StartedBoth);
        queueGdbCommand("break-insert fpc_get_input", StartedBoth);
//        queueGdbCommand("break-insert fpc_write_end", StartedBoth);
//        queueGdbCommand("break-insert fpc_writeln_end", StartedBoth);
    }
    else if (Running == gdbState_) {
        QStringList message = ioCodec_->toUnicode(serverOut).split('\n', QString::SkipEmptyParts);
        QRegExp rxGarbage1("Child exited with status \\d+");
        QRegExp rxGarbage2("GDBserver exiting");
        for (int i=0; i<message.size(); i++) {
            if (rxGarbage1.exactMatch(message[i]) || rxGarbage2.exactMatch(message[i])) {
                qDebug() << message[i];
            }
            else {
                emit errorOutputRequest(message[i]);
            }
        }
    }
}

void GdbRunPlugin::handleGdbServerReadStdOut()
{
    QByteArray serverOut = gdbServer_->readAllStandardOutput().replace('\r',"");
    QString message = ioCodec_->toUnicode(serverOut);
    emit outputRequest(message);
}

bool GdbRunPlugin::canStepOut() const
{
    return false;
}

void GdbRunPlugin::runBlind()
{
    queueGdbCommand("exec-continue", Paused);
    flushGdbCommands();
}

void GdbRunPlugin::runContinuous()
{    
    if (!symbolsLoaded_) {
        loadSymbols();
    }
    queueGdbCommand("exec-continue", Paused);
    flushGdbCommands();
}

void GdbRunPlugin::runStepInto()
{
    if (!symbolsLoaded_) {
        loadSymbols();
    }
    if (!breakLine1Inserted_) {
        queueGdbCommand("break-insert main", Paused);
        breakLine1Inserted_ = true;
    }
    queueGdbCommand("exec-step", Paused);
    flushGdbCommands();
}

void GdbRunPlugin::runStepOver()
{
    if (!breakLine1Inserted_) {
        queueGdbCommand("break-insert main", Paused);
        breakLine1Inserted_ = true;
    }
    if (!symbolsLoaded_) {
        loadSymbols();
    }
    queueGdbCommand("exec-next", Paused);
    flushGdbCommands();
}

void GdbRunPlugin::runToEnd()
{
    if (!symbolsLoaded_) {
        loadSymbols();
    }
    queueGdbCommand("exec-finish", Paused);
    flushGdbCommands();
}

void GdbRunPlugin::runTesting()
{
    runBlind(); // not implemented
}

bool GdbRunPlugin::isTestingRun() const
{
    return false;
}

void GdbRunPlugin::loadSymbols()
{

}

void GdbRunPlugin::terminate()
{
    gdbState_ = Terminating;
    gdbServer_->terminate();
    gdbClient_->terminate();
    gdbState_ = NotStarted;
    emit stopped(SR_UserTerminated);
}

bool GdbRunPlugin::hasMoreInstructions() const
{
    return true;
}

bool GdbRunPlugin::hasTestingEntryPoint() const
{
    return false;
}

unsigned long int GdbRunPlugin::stepsCounted() const
{
    return 0;
}

void GdbRunPlugin::sendGdbCommand(const QByteArray &command)
{
#ifdef Q_OS_WIN32
    static const QByteArray NL = "\r\n";
#else
    static const QByteArray NL = "\n";
#endif
    gdbClient_->write("-" + command + NL);
}

void GdbRunPlugin::queueGdbCommand(const QByteArray &command, GdbState condition)
{
    QMutexLocker l(gdbCommandsQueueLocker_);
    ConditionalCommand cmd;
    cmd.command = command;
    cmd.cond = condition;
    gdbCommandsQueue_.push_back(cmd);
}

void GdbRunPlugin::queueGdbCommands(const QList<QByteArray> &commands, GdbState condition)
{
    QMutexLocker l(gdbCommandsQueueLocker_);
    for (int i=0; i<commands.size(); i++) {
        ConditionalCommand cmd;
        cmd.command = commands[i];
        cmd.cond = condition;
        gdbCommandsQueue_.push_back(cmd);
    }
}

void GdbRunPlugin::flushGdbCommands()
{
    QMutexLocker l(gdbCommandsQueueLocker_);
    for (QLinkedList<ConditionalCommand>::iterator it=gdbCommandsQueue_.begin();
         it!=gdbCommandsQueue_.end(); )
    {
        ConditionalCommand cmd = *it;
        if (cmd.cond == gdbState_) {
            sendGdbCommand(cmd.command);
            it = gdbCommandsQueue_.erase(it);
        }
        else {
            it ++;
        }
    }
}

int GdbRunPlugin::currentLineNo() const
{
    return -1;
}

QPair<quint32,quint32> GdbRunPlugin::currentColumn() const
{
    return QPair<quint32,quint32>(0u, 0u);
}

QString GdbRunPlugin::error() const
{
    return "";
}

QVariant GdbRunPlugin::valueStackTopItem() const
{
    return 0;
}

void GdbRunPlugin::handleGdbClientReadStdErr()
{
    QMutexLocker lock(gdbStateLocker_);
    qDebug() << "GDB STDERR: " << gdbClient_->readAllStandardError().replace('\r',"");
}

void GdbRunPlugin::handleGdbClientReadStdOut()
{
    QMutexLocker lock(gdbStateLocker_);
    QList<QByteArray> lines = gdbClient_->readAllStandardOutput().replace('\r',"").split('\n');
#if !defined(NDEBUG) && !defined(QT_NO_DEBUG)
    qDebug() << lines;
#endif
    for (int i=0; i<lines.size(); i++) {
        if (lines[i].length()>0) {
            const char first = lines.at(i).at(0);
            switch (first) {
            case '^':
                handleGdbStatusStream(lines.at(i).mid(1));
                break;
            case '~':
                handleGdbInteractionStream(lines.at(i).mid(1));
                break;
            case '&':
                handleGdbLogOutputStream(lines.at(i).mid(1));
                break;
            case '*':
                handleGdbAsyncMessageStream(lines.at(i).mid(1));
                break;
            case '=':
                handleGdbAsyncLogStream(lines.at(i).mid(1));
                break;
            default:
                break;
            }
        }
    }
    flushGdbCommands();
}

void GdbRunPlugin::handleGdbAsyncLogStream(const QByteArray &resultStream)
{
    const QString rawMsg = QString::fromLocal8Bit(resultStream);
    QMap<QString,QVariant> msg = parseGdbMiCommandOutput(rawMsg);
    if (msg.contains("thread-group-started")) {
        inferiorPid_ = (Q_PID)(msg["pid"].toULongLong());
        gdbState_ = StartedBoth;
    }
}

QMap<QString,QVariant> GdbRunPlugin::parseGdbMiCommandOutput(const QString &out)
{
    enum Mode { Key, Value, Literal, Block, Array };
    Mode currentMode = Key;
    QMap<QString,QVariant> result;
    QString buffer;
    QString key;
    QVariant value;
    int braceDeep = 0;
    int sqBraceDeep = 0;
    for (int i=0; i<out.size(); i++) {
        if (Key == currentMode) {
            if ('=' == out[i]) {
                key = buffer;
                currentMode = Value;
                buffer.clear();
            }
            else if (',' == out[i]) {
                key = buffer;
                result[key] = QVariant();  // just a flag
                currentMode = Key;
                buffer.clear();
                key.clear();
                value.clear();
            }
            else {
                buffer.append(out[i]);
            }
        }
        else if (Value == currentMode) {
            if ('"' == out[i]) {
                currentMode = Literal;
            }
            else if ('[' == out[i]) {
                currentMode = Array;
                value = QVariantList();
                sqBraceDeep ++;
            }
            else if ('{' == out[i]) {
                currentMode = Block;
                braceDeep ++;
            }
            else if (',' == out[i]) {
                result[key] = value;
                currentMode = Key;
                buffer.clear();
                key.clear();
                value.clear();
            }
        }
        else if (Literal == currentMode) {
            if ('"' == out[i]) {
                currentMode = Value;
                value = QVariant(buffer);
            }
            else {
                buffer.append(out[i]);
            }
        }
        else if (Array == currentMode) {
            if ('[' == out[i]) sqBraceDeep ++;
            else if (']' == out[i]) sqBraceDeep --;
            if (
                    (0==sqBraceDeep && ']'==out[i])
               )
            {
                QVariantList alist;
                QStringList selems = splitByTopLevelComas(buffer);
                Q_FOREACH(QString selem, selems) {
                    QVariant elem;
                    if (selem.startsWith('"')) {
                        elem = selem.mid(1, selem.length()-2);
                    }
                    else {
                        if (selem.startsWith("{"))
                            selem = selem.mid(1, selem.length()-2);
                        elem = parseGdbMiCommandOutput(selem);
                    }
                    alist.append(elem);

                }
                buffer.clear();
                value = alist;
                currentMode = Value;

            }
            else {
                buffer.append(out[i]);
            }
        }
        else if (Block == currentMode) {
            if ('{' == out[i]) braceDeep ++;
            else if ('}' == out[i]) braceDeep --;
            if (0 == braceDeep) {
                currentMode = Value;
                QMap<QString,QVariant> sub = parseGdbMiCommandOutput(buffer);
                value = QVariant(sub);
            }
            else {
                buffer.append(out[i]);
            }
        }
    }
    if (key.length() > 0) {
        result[key] = value;
    }
    return result;
}

QStringList GdbRunPlugin::splitByTopLevelComas(const QString &in)
{
    int deep = 0;
    QStringList result;
    if (in.isEmpty())
        return result;
    result.append(QString());
    for (int i=0; i<in.length(); i++) {
        if (deep == 0 && in[i]==',') {
            result.append(QString());
        }
        else {
            if (in[i] == '{' || in[i] == '[')
                deep++;
            if (in[i] == '}' || in[i] == '}')
                deep--;
            result.last().append(in[i]);
        }
    }
    return result;
}

void GdbRunPlugin::handleGdbAsyncMessageStream(const QByteArray &resultStream)
{
    const QString rawMsg = QString::fromLocal8Bit(resultStream);
    const QMap<QString,QVariant> msg = parseGdbMiCommandOutput(rawMsg);
    if (msg.contains("stopped")) {
        if (msg.contains("reason")) {
            gdbState_ = Terminating;
            StopReason stopReason = SR_UserInteraction;
            if (msg["reason"].toString() == "signal-received") {
                const QString sigName = msg["signal-name"].toString();
                if ("SIGINT" == sigName) {
                    stopReason = SR_UserTerminated;
                }
                else {
                    stopReason = SR_Error;
                }
            }
            else if (msg["reason"] == "exited" && msg.contains("exit-code")) {
                int exitCode = msg.value("exit-code").toInt();
                stopReason = exitCode == 0? SR_Done : SR_Error;
            }
            else if (msg["reason"] == "exited-normally") {
                stopReason = SR_Done;
            }
            else if (msg["reason"] == "breakpoint-hit") {
                const QMap<QString,QVariant> frame = msg["frame"].toMap();
                const QString func = frame.value("func").toString();
                if ("fpc_get_input" == func) {
                    gdbState_ = Querying;
                    handleGdbServerReadStdOut();
                    extractInputFormat();
                    stopReason = SR_InputRequest;
                }
                else if ("fpc_write_end" == func || "fpc_writeln_end" == func) {
                    gdbState_ = Running;
                    sendGdbCommand("exec-finish");
                    handleGdbServerReadStdOut();
                    sendGdbCommand("exec-continue");
                }
                else {
                    if (frame.contains("line")) {
                        gdbState_ = Paused;
                        stopReason = SR_UserInteraction;
                        int lineNo = frame.value("line").toInt() - 1;
                        emit lineChanged(lineNo, 0u, 0u);
                    }
                    else {
                        gdbState_ = Querying;
                        stopReason = SR_UserInteraction;
                        bkptNoQuery_ = msg.value("bkptno").toString().toAscii();
                        sendGdbCommand("break-info " + bkptNoQuery_);
                    }
                }
            }
            else if (msg["reason"] == "function-finished") {
                gdbState_ = Querying;
                stopReason = SR_InputRequest;
            }
            else if (msg["reason"] == "end-stepping-range") {
                const QMap<QString,QVariant> frame = msg["frame"].toMap();
                gdbState_ = Paused;
                stopReason = SR_UserInteraction;
                int lineNo = frame.value("line").toInt() - 1;
                emit lineChanged(lineNo, 0u, 0u);
            }
            if (Querying != gdbState_ || Running != gdbState_)
                emit stopped(stopReason);  // not a stop
        }
        else {
            gdbState_ = Paused;
        }
        if (Terminating == gdbState_) {
            sendGdbCommand("gdb-exit");
        }
    }
    else if (msg.contains("running") && Querying!=gdbState_) {
        gdbState_ = Running;
    }
}

void GdbRunPlugin::extractInputFormat()
{
    // FPC implementation!
    // for other languagese -- reimplement this method
    const QByteArray execReturn = "exec-finish";
    const QByteArray dissassemble = "data-disassemble -s $pc -e \"$pc+200\" -- 0";
    const QList<QByteArray> cmds = QList<QByteArray>()
            << execReturn << dissassemble;
    queueGdbCommands(cmds, Querying);
}

void GdbRunPlugin::handleGdbInteractionStream(const QByteArray &resultStream)
{

}

void GdbRunPlugin::handleGdbLogOutputStream(const QByteArray &resultStream)
{

}

void GdbRunPlugin::handleGdbStatusStream(const QByteArray &resultStream)
{
    if (Querying == gdbState_) {
        QMap<QString,QVariant> response = parseGdbMiCommandOutput(ioCodec_->toUnicode(resultStream));
        processGdbQueryResponse(response);
    }
}


void GdbRunPlugin::processGdbQueryResponse(const QMap<QString, QVariant> &response)
{
    if (response.contains("done") && response.contains("asm_insns")) {
        // Find terminating fpc_read_end and collect fpc_read_text_??? calls
        const QVariantList asm_insns = response.value("asm_insns").toList();
        QStringList callees;
        for (int i=0; i<asm_insns.size(); i++) {
            const QMap<QString,QVariant> details = asm_insns.at(i).toMap();
            const QString & inst = details.value("inst").toString();
            if (inst.startsWith("call")) {
                const QStringList cmd = inst.split(QRegExp("\\s+"), QString::SkipEmptyParts);
                QString name = cmd.last();
                if (name.startsWith('<') && name.endsWith('>')) {
                    name = name.mid(1, name.length()-2);
                }
                if ("fpc_read_end" == name) {
                    break;
                }
                if (name.startsWith("fpc_read_")) {
                    callees << name;
                }
            }
        }
        // Prepare format for input request
        QStringList formats;
//        qDebug() << callees;
        for (int i=0; i<callees.size(); i++) {
            const QString & s = callees[i];
            if ("fpc_read_text_sint"==s || "fpc_read_text_lint"==s)
                formats.append("i");
            else if ("fpc_read_text_float"==s || "fpc_read_text_double"==s)
                formats.append("r");
            else if ("fpc_read_text_char"==s)
                formats.append("c");
            else if ("fpc_read_text_shortstr"==s)
                formats.append("s");
        }
        gdbState_ = Paused;
        const QString format = formats.join(";");
        emit stopped(SR_InputRequest);
        emit inputRequest(format);
    }
    else if (Querying == gdbState_ &&
             response.contains("done") &&
             response.contains("BreakpointTable")
             )
    {
        const QMap<QString,QVariant> table = response.value("BreakpointTable").toMap();
        const QVariantList body = table.value("body").toList();
        Q_FOREACH(QVariant record, body) {
            const QMap<QString,QVariant> bkpt = record.toMap().value("bkpt").toMap();
            const QByteArray number = bkpt.value("number").toByteArray();
            const QByteArray line = bkpt.value("line").toByteArray();
            if (bkptNoQuery_ == number) {
                int lineNo = line.toInt() - 1;
                emit lineChanged(lineNo, 0u, 0u);
                gdbState_ = Paused;
                emit stopped(SR_UserInteraction);
            }
        }
    }
}

void GdbRunPlugin::StateWaiter::waitForState(GdbState targetState)
{
    t = targetState;
    start();
    wait();
}

void GdbRunPlugin::StateWaiter::run()
{
    GdbRunPlugin::GdbState state;
    Q_FOREVER {
        p->gdbStateLocker_->lock();
        state = p->gdbState_;
        p->gdbStateLocker_->unlock();
        if (state == t) {
            break;
        }
        QCoreApplication::processEvents();
        msleep(50);
    }
}

}


Q_EXPORT_PLUGIN2(GDBRunPlugin, GdbRun::GdbRunPlugin)
