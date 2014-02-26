/****************************************************************************
**
** Some parts of code taken from QtCreator 2.8.1 by Digia Plc
**
** Licensed under the terms of GNU Lesser GPL Licensee 2.1
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
****************************************************************************/

#include "midebuggerprivate.h"
#include "gdbmivalue.h"
#include "stopevent.h"
#include "runevent.h"

#include <QDebug>
#include <QFile>

#define BREAK_ASSERT(cond, action) if (cond) {} else { action; }
#define QTC_ASSERT(cond, action) if (cond) {} else { /*QTC_ASSERT_STRING(#cond);*/ action; } do {} while (0)
#define QTC_CHECK(cond) if (cond) {} else { /*QTC_ASSERT_STRING(#cond);*/ } do {} while (0)

static QString _(const char * s) { return QString::fromUtf8(s); }

namespace GdbMi {

void MiDebuggerPrivate::initialize()
{
    process_ = new QProcess(this);
    codec_ = QTextCodec::codecForLocale();
    connect(process_, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleStdOutReady()));
}

void MiDebuggerPrivate::handleStdOutReady()
{
    const QByteArray buff = process_->readAllStandardOutput();
    handleResponse(buff);
}

static bool isNameChar(char c)
{
    // could be 'stopped' or 'shlibs-added'
    return (c >= 'a' && c <= 'z') || c == '-';
}

void MiDebuggerPrivate::handleResponse(const QByteArray &buff)
{
    if (buff.isEmpty() || "(gdb)" == buff) {
        return;
    }
    const char * from = buff.constData();
    const char * to = from + buff.size();
    const char * inner;
    int token = -1;

    for (inner = from; inner != to; ++inner) {
        if (*inner < '0' || *inner > '9') {
            break;
        }
    }

    if (from != inner) {
        // the response starts with token prefix
        token = QByteArray(from, inner - from).toInt();
        from = inner;
    }

    const char c = *from++;
    switch (c) {
    case '*':
    case '+':
    case '=': {
        QByteArray asyncClass;
        for (; from != to; ++from) {
            const char c = *from;
            if (!isNameChar(c))
                break;
            asyncClass += *from;
        }

        GdbMiValue result;
        while (from != to) {
            GdbMiValue data;
            if (*from != ',') {
                // happens on archer where we get
                // 23^running <NL> *running,thread-id="all" <NL> (gdb)
                result.m_type = GdbMiValue::Tuple;
                break;
            }
            ++from; // skip ','
            data.parseResultOrValue(from, to);
            if (data.isValid()) {
                //qDebug() << "parsed result:" << data.toString();
                result.m_children += data;
                result.m_type = GdbMiValue::Tuple;
            }
        }
        if (asyncClass == "stopped") {
            handleStopResponse(result);
            m_pendingLogStreamOutput.clear();
            m_pendingConsoleStreamOutput.clear();
        } else if (asyncClass == "running") {
            GdbMiValue threads = result["thread-id"];
            if (state() == InferiorRunOk || state() == InferiorSetupRequested) {
                // We get multiple *running after thread creation and in Windows terminals.
//                showMessage(QString::fromLatin1("NOTE: INFERIOR STILL RUNNING IN STATE %1.").
//                            arg(QLatin1String(DebuggerEngine::stateName(state()))));
            } else {
                handleRunningResponse(threads);
            }
        } else if (asyncClass == "library-loaded") {
            // Archer has 'id="/usr/lib/libdrm.so.2",
            // target-name="/usr/lib/libdrm.so.2",
            // host-name="/usr/lib/libdrm.so.2",
            // symbols-loaded="0"

            // id="/lib/i386-linux-gnu/libc.so.6"
            // target-name="/lib/i386-linux-gnu/libc.so.6"
            // host-name="/lib/i386-linux-gnu/libc.so.6"
            // symbols-loaded="0",thread-group="i1"
//            QByteArray id = result["id"].data();
//            if (!id.isEmpty())
//                showStatusMessage(tr("Library %1 loaded").arg(_(id)), 1000);
//            progressPing();
//            invalidateSourcesList();
//            Module module;
//            module.startAddress = 0;
//            module.endAddress = 0;
//            module.hostPath = _(result["host-name"].data());
//            module.modulePath = _(result["target-name"].data());
//            module.moduleName = QFileInfo(module.hostPath).baseName();
//            modulesHandler()->updateModule(module);
        } else if (asyncClass == "library-unloaded") {
            // Archer has 'id="/usr/lib/libdrm.so.2",
            // target-name="/usr/lib/libdrm.so.2",
            // host-name="/usr/lib/libdrm.so.2"
//            QByteArray id = result["id"].data();
//            progressPing();
//            showStatusMessage(tr("Library %1 unloaded").arg(_(id)), 1000);
//            invalidateSourcesList();
        } else if (asyncClass == "thread-group-added") {
            // 7.1-symbianelf has "{id="i1"}"
        } else if (asyncClass == "thread-group-created"
                || asyncClass == "thread-group-started") {
            // Archer had only "{id="28902"}" at some point of 6.8.x.
            // *-started seems to be standard in 7.1, but in early
            // 7.0.x, there was a *-created instead.
//            progressPing();
            // 7.1.50 has thread-group-started,id="i1",pid="3529"
            QByteArray id = result["id"].data();
//            showStatusMessage(tr("Thread group %1 created").arg(_(id)), 1000);
            int pid = id.toInt();
            if (!pid) {
                id = result["pid"].data();
                pid = id.toInt();
            }
            if (pid)
                notifyInferiorPid(pid);
//            handleThreadGroupCreated(result);
        } else if (asyncClass == "thread-created") {
            //"{id="1",group-id="28902"}"
//            QByteArray id = result["id"].data();
//            showStatusMessage(tr("Thread %1 created").arg(_(id)), 1000);
//            ThreadData thread;
//            thread.id = ThreadId(id.toLong());
//            thread.groupId = result["group-id"].data();
//            threadsHandler()->updateThread(thread);
        } else if (asyncClass == "thread-group-exited") {
            // Archer has "{id="28902"}"
//            QByteArray id = result["id"].data();
//            showStatusMessage(tr("Thread group %1 exited").arg(_(id)), 1000);
//            handleThreadGroupExited(result);
        } else if (asyncClass == "thread-exited") {
            //"{id="1",group-id="28902"}"
//            QByteArray id = result["id"].data();
//            QByteArray groupid = result["group-id"].data();
//            showStatusMessage(tr("Thread %1 in group %2 exited")
//                .arg(_(id)).arg(_(groupid)), 1000);
//            threadsHandler()->removeThread(ThreadId(id.toLong()));
        } else if (asyncClass == "thread-selected") {
//            QByteArray id = result["id"].data();
//            showStatusMessage(tr("Thread %1 selected").arg(_(id)), 1000);
            //"{id="2"}"
//        } else if (m_isMacGdb && asyncClass == "shlibs-updated") {
            // Apple's gdb announces updated libs.
//            invalidateSourcesList();
//        } else if (m_isMacGdb && asyncClass == "shlibs-added") {
            // Apple's gdb announces added libs.
            // {shlib-info={num="2", name="libmathCommon.A_debug.dylib",
            // kind="-", dyld-addr="0x7f000", reason="dyld", requested-state="Y",
            // state="Y", path="/usr/lib/system/libmathCommon.A_debug.dylib",
            // description="/usr/lib/system/libmathCommon.A_debug.dylib",
            // loaded_addr="0x7f000", slide="0x7f000", prefix=""}}
//            invalidateSourcesList();
//        } else if (m_isMacGdb && asyncClass == "resolve-pending-breakpoint") {
            // Apple's gdb announces resolved breakpoints.
            // new_bp="1",pended_bp="1",new_expr="\"gdbengine.cpp\":1584",
            // bkpt={number="1",type="breakpoint",disp="keep",enabled="y",
            // addr="0x0000000115cc3ddf",func="foo()",file="../foo.cpp",
            // line="1584",shlib="/../libFoo_debug.dylib",times="0"}
//            const GdbMi bkpt = result["bkpt"];
//            const BreakpointResponseId rid(bkpt["number"].data());
//            if (!isQmlStepBreakpoint(rid)) {
//                BreakHandler *handler = breakHandler();
//                BreakpointModelId id = handler->findBreakpointByResponseId(rid);
//                BreakpointResponse br = handler->response(id);
//                updateResponse(br, bkpt);
//                handler->setResponse(id, br);
//                attemptAdjustBreakpointLocation(id);
//            }
        } else if (asyncClass == "breakpoint-modified") {
            // New in FSF gdb since 2011-04-27.
            // "{bkpt={number="3",type="breakpoint",disp="keep",
            // enabled="y",addr="<MULTIPLE>",times="1",
            // original-location="\\",simple_gdbtest_app.cpp\\":135"},
            // {number="3.1",enabled="y",addr="0x0805ff68",
            // func="Vector<int>::Vector(int)",
            // file="simple_gdbtest_app.cpp",
            // fullname="/data/...line="135"},{number="3.2"...}}"

            // Note the leading comma in original-location. Filter it out.
            // We don't need the field anyway.
            QByteArray ba = result.toString();
            ba = '[' + ba.mid(6, ba.size() - 7) + ']';
            const int pos1 = ba.indexOf(",original-location");
            const int pos2 = ba.indexOf("\":", pos1 + 2);
            const int pos3 = ba.indexOf('"', pos2 + 2);
            ba.remove(pos1, pos3 - pos1 + 1);
            result = GdbMiValue();
            result.fromString(ba);
//            BreakHandler *handler = breakHandler();
//            BreakpointModelId id;
//            BreakpointResponse br;
//            foreach (const GdbMiValue &bkpt, result.children()) {
//                const QByteArray nr = bkpt["number"].data();
//                BreakpointResponseId rid(nr);
//                if (!isHiddenBreakpoint(rid)) {
//                    if (nr.contains('.')) {
//                        // A sub-breakpoint.
//                        BreakpointResponse sub;
//                        updateResponse(sub, bkpt);
//                        sub.id = rid;
//                        sub.type = br.type;
//                        handler->insertSubBreakpoint(id, sub);
//                    } else {
//                        // A primary breakpoint.
//                        id = handler->findBreakpointByResponseId(rid);
//                        //qDebug() << "NR: " << nr << "RID: " << rid
//                        //    << "ID: " << id;
//                        //BreakpointModelId id =
//                        //    handler->findBreakpointByResponseId(rid);
//                        br = handler->response(id);
//                        updateResponse(br, bkpt);
//                        handler->setResponse(id, br);
//                    }
//                }
//            }
            m_hasBreakpointNotifications = true;
        } else if (asyncClass == "breakpoint-created") {
            // "{bkpt={number="1",type="breakpoint",disp="del",enabled="y",
            //  addr="<PENDING>",pending="main",times="0",
            //  original-location="main"}}" -- or --
            // {bkpt={number="2",type="hw watchpoint",disp="keep",enabled="y",
            //  what="*0xbfffed48",times="0",original-location="*0xbfffed48"
            BreakHandler *handler = breakHandler();
            foreach (const GdbMiValue &bkpt, result.children()) {
                BreakpointResponse br;
                br.type = BreakpointByFileAndLine;
                updateResponse(br, bkpt);
                handler->handleAlienBreakpoint(br, pClass_);
            }
        } else if (asyncClass == "breakpoint-deleted") {
            // "breakpoint-deleted" "{id="1"}"
            // New in FSF gdb since 2011-04-27.
            BreakHandler *handler = breakHandler();
            QByteArray nr = result["id"].data();
            BreakpointResponseId rid(nr);
            BreakpointModelId id = handler->findBreakpointByResponseId(rid);
            if (id.isValid()) {
                // This also triggers when a temporary breakpoint is hit.
                // We do not really want that, as this loses all information.
                // FIXME: Use a special marker for this case?
                if (!handler->isOneShot(id))
                    handler->removeAlienBreakpoint(id);
            }
        } else if (asyncClass == "cmd-param-changed") {
            // New since 2012-08-09
            //  "{param="debug remote",value="1"}"
        } else if (asyncClass == "memory-changed") {
            // New since 2013
            //   "{thread-group="i1",addr="0x0918a7a8",len="0x10"}"
        } else {
            qDebug() << "IGNORED ASYNC OUTPUT"
                << asyncClass << result.toString();
        }
        break;
    }
    default: {

    }

    }
}

void MiDebuggerPrivate::updateResponse(BreakpointResponse &response, const GdbMiValue &bkpt)
{
    QTC_ASSERT(bkpt.isValid(), return);

    QByteArray originalLocation;

    response.multiple = false;
    response.enabled = true;
    response.pending = false;
    response.condition.clear();
    QByteArray file, fullName;
    foreach (const GdbMiValue &child, bkpt.children()) {
        if (child.hasName("number")) {
            response.id = BreakpointResponseId(child.data());
        } else if (child.hasName("func")) {
            response.functionName = _(child.data());
        } else if (child.hasName("addr")) {
            // <MULTIPLE> happens in constructors, inline functions, and
            // at other places like 'foreach' lines. In this case there are
            // fields named "addr" in the response and/or the address
            // is called <MULTIPLE>.
            //qDebug() << "ADDR: " << child.data() << (child.data() == "<MULTIPLE>");
            if (child.data() == "<MULTIPLE>")
                response.multiple = true;
            if (child.data().startsWith("0x"))
                response.address = child.toAddress();
        } else if (child.hasName("file")) {
            file = child.data();
        } else if (child.hasName("fullname")) {
            fullName = child.data();
        } else if (child.hasName("line")) {
            // The line numbers here are the uncorrected ones. So don't
            // change it if we know better already.
            if (response.correctedLineNumber == 0)
                response.lineNumber = child.toInt();
        } else if (child.hasName("cond")) {
            // gdb 6.3 likes to "rewrite" conditions. Just accept that fact.
            response.condition = child.data();
        } else if (child.hasName("enabled")) {
            response.enabled = (child.data() == "y");
        } else if (child.hasName("pending")) {
            // Any content here would be interesting only if we did accept
            // spontaneously appearing breakpoints (user using gdb commands).
            if (file.isEmpty())
                file = child.data();
            response.pending = true;
        } else if (child.hasName("at")) {
            // Happens with gdb 6.4 symbianelf.
            QByteArray ba = child.data();
            if (ba.startsWith('<') && ba.endsWith('>'))
                ba = ba.mid(1, ba.size() - 2);
            response.functionName = _(ba);
        } else if (child.hasName("thread")) {
            response.threadSpec = child.toInt();
        } else if (child.hasName("type")) {
            // "breakpoint", "hw breakpoint", "tracepoint", "hw watchpoint"
            // {bkpt={number="2",type="hw watchpoint",disp="keep",enabled="y",
            //  what="*0xbfffed48",times="0",original-location="*0xbfffed48"
            if (child.data().contains("tracepoint")) {
                response.tracepoint = true;
            } else if (child.data() == "hw watchpoint" || child.data() == "watchpoint") {
                QByteArray what = bkpt["what"].data();
                if (what.startsWith("*0x")) {
                    response.type = WatchpointAtAddress;
                    response.address = what.mid(1).toULongLong(0, 0);
                } else {
                    response.type = WatchpointAtExpression;
                    response.expression = QString::fromLocal8Bit(what);
                }
            }
        } else if (child.hasName("original-location")) {
            originalLocation = child.data();
        }
        // This field is not present.  Contents needs to be parsed from
        // the plain "ignore" response.
        //else if (child.hasName("ignore"))
        //    response.ignoreCount = child.data();
    }

    QString name;
    if (!fullName.isEmpty()) {
        name = cleanupFullName(QFile::decodeName(fullName));
        response.fileName = name;
        //if (data->markerFileName().isEmpty())
        //    data->setMarkerFileName(name);
    } else {
        name = QFile::decodeName(file);
        // Use fullName() once we have a mapping which is more complete than
        // gdb's own. No point in assigning markerFileName for now.
    }
    if (!name.isEmpty())
        response.fileName = name;

    if (response.fileName.isEmpty())
        response.updateLocation(originalLocation);
}

QString MiDebuggerPrivate::cleanupFullName(const QString &fileName)
{
    QString cleanFilePath = fileName;

    // Gdb running on windows often delivers "fullnames" which
    // (a) have no drive letter and (b) are not normalized.
#ifdef Q_OS_WIN32
        QTC_ASSERT(!fileName.isEmpty(), return QString());
        QFileInfo fi(fileName);
        if (fi.isReadable())
            cleanFilePath = QDir::cleanPath(fi.absoluteFilePath());
#endif
//    if (startMode() == StartRemoteGdb) {
//        cleanFilePath.replace(0, startParameters().remoteMountPoint.length(),
//            startParameters().localMountDir);
//    }

        return cleanFilePath;
//    if (!debuggerCore()->boolSetting(AutoEnrichParameters))

//    const QString sysroot = startParameters().sysRoot;
//    if (QFileInfo(cleanFilePath).isReadable())
//        return cleanFilePath;
//    if (!sysroot.isEmpty() && fileName.startsWith(QLatin1Char('/'))) {
//        cleanFilePath = sysroot + fileName;
//        if (QFileInfo(cleanFilePath).isReadable())
//            return cleanFilePath;
//    }
//    if (m_baseNameToFullName.isEmpty()) {
//        QString debugSource = sysroot + QLatin1String("/usr/src/debug");
//        if (QFileInfo(debugSource).isDir()) {
//            QDirIterator it(debugSource, QDirIterator::Subdirectories);
//            while (it.hasNext()) {
//                it.next();
//                QString name = it.fileName();
//                if (!name.startsWith(QLatin1Char('.'))) {
//                    QString path = it.filePath();
//                    m_baseNameToFullName.insert(name, path);
//                }
//            }
//        }
//    }

//    cleanFilePath.clear();
//    const QString base = QFileInfo(fileName).fileName();

//    QMap<QString, QString>::const_iterator jt = m_baseNameToFullName.find(base);
//    while (jt != m_baseNameToFullName.end() && jt.key() == base) {
//        // FIXME: Use some heuristics to find the "best" match.
//        return jt.value();
//        //++jt;
//    }

//    return cleanFilePath;
}

void MiDebuggerPrivate::handleStopResponse(const GdbMiValue &data)
{
    StopEvent event;
    event.breakpointNo = data["bkptno"].toInt();
    event.exitCode = data["exitCode"].toInt();
    event.reason = StopEvent::Unknown;
    event.signalName = data["signal-name"].toLatin1();
    event.threadId = data["thread-id"].toInt();
    event.frame = 0;
    const QByteArray reason = data["reason"].toString();
    if ("breakpoint-hit" == reason)
        event.reason = StopEvent::BreakpointHit;
    else if ("watchpoint-trigger" == reason)
        event.reason = StopEvent::WatchpointTrigger;
    else if ("function-finished" == reason)
        event.reason = StopEvent::FunctionFinished;
    else if ("end-stepping-range" == reason)
        event.reason = StopEvent::EndSteppingRange;
    else if ("exited" == reason)
        event.reason = StopEvent::Exited;
    else if ("exited-normally")
        event.reason = StopEvent::ExitedNormally;
    else if ("signal-received")
        event.reason = StopEvent::SignalReceived;
    emit pClass_->stopped(event);
}

void MiDebuggerPrivate::handleRunningResponse(const GdbMiValue &data)
{

}

} // namespace GdbMi
