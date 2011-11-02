#include "kumirprogram.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "kumirvariableswebobject.h"
#include "dataformats/ast_algorhitm.h"

namespace CoreGUI {

using Shared::ActorInterface;
using namespace ExtensionSystem;

KumirProgram::KumirProgram(QObject *parent)
    : QObject(parent)
    , e_state(Idle)
    , m_ast(0)
    , m_process(0)
    , m_terminal(0)
    , plugin_nativeGenerator(0)
    , plugin_bytcodeGenerator(0)
    , plugin_bytecodeRun(0)
    , plugin_editor(0)
    , a_fastRun(0)
    , a_blindRun(0)
    , a_regularRun(0)
    , a_testingRun(0)
    , a_stepRun(0)
    , a_stepIn(0)
    , a_stepOut(0)
    , a_stop(0)
    , gr_actions(0)
    , m_connector(0)
    , m_variablesWebObject(0)
    , w_mainWidget(0)
{
    b_blind = false;

    a_regularRun = new QAction(tr("Regular run"), this);
    a_regularRun->setIcon(QIcon::fromTheme("media-playback-start", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/media-playback-start.png")));
    connect(a_regularRun, SIGNAL(triggered()), this, SLOT(regularRun()));
#ifndef Q_OS_MAC
    a_regularRun->setShortcut(QKeySequence("F9"));
#else
    a_regularRun->setShortcut(QKeySequence("F5"));
#endif
    a_regularRun->setToolTip(a_regularRun->text()+" <b>"+a_regularRun->shortcut().toString()+"</b>");

    a_testingRun = new QAction(tr("Testing run"), this);
    connect(a_testingRun, SIGNAL(triggered()), this, SLOT(testingRun()));
    a_testingRun->setShortcut(QKeySequence("Ctrl+T"));
    a_testingRun->setToolTip(a_testingRun->text()+" <b>"+a_testingRun->shortcut().toString()+"</b>");

    a_stepRun = new QAction(tr("Step over"), this);
    a_stepRun->setIcon(QIcon::fromTheme("debug-step-over", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-over.png")));
    connect(a_stepRun, SIGNAL(triggered()), this, SLOT(stepRun()));
#ifndef Q_OS_MAC
    a_stepRun->setShortcut(QKeySequence("F8"));
#else
    a_stepRun->setShortcut(QKeySequence("F6"));
#endif
    a_stepRun->setToolTip(tr("Do big step")+" <b>"+a_stepRun->shortcut().toString()+"</b>");

    a_stepIn = new QAction(tr("Step in"), this);
    a_stepIn->setIcon(QIcon::fromTheme("debug-step-into", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-into.png")));
    connect(a_stepIn, SIGNAL(triggered()), this, SLOT(stepIn()));
#ifndef Q_OS_MAC
    a_stepIn->setShortcut(QKeySequence("F7"));
#else
    a_stepIn->setShortcut(QKeySequence("F7"));
#endif
    a_stepIn->setToolTip(tr("Do small step")+" <b>"+a_stepIn->shortcut().toString()+"</b>");

    a_stepOut = new QAction(tr("Step to end"), this);
    a_stepOut->setIcon(QIcon::fromTheme("debug-step-out", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-out.png")));
    connect(a_stepOut, SIGNAL(triggered()), this, SLOT(stepOut()));
#ifndef Q_OS_MAC
    a_stepOut->setShortcut(QKeySequence("Shift+F10"));
#else
    a_stepOut->setShortcut(QKeySequence("Shift+F10"));
#endif
    a_stepOut->setToolTip(tr("Run to end of algorhitm")+" <b>"+a_stepOut->shortcut().toString()+"</b>");

    a_stop = new QAction(tr("Stop"), this);
    a_stop->setIcon(QIcon::fromTheme("media-playback-stop", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/media-playback-stop.png")));
    connect(a_stop, SIGNAL(triggered()), this, SLOT(stop()));
#ifndef Q_OS_MAC
    a_stop->setShortcut(QKeySequence("Esc"));
#else
    a_stop->setShortcut(QKeySequence("Esc"));
#endif
    a_stop->setToolTip(a_stop->text()+" <b>"+a_stop->shortcut().toString()+"</b>");

    a_stepIn->setEnabled(false);
    a_stepOut->setEnabled(false);
    a_stop->setEnabled(false);

    QAction * s1 = new QAction(this);
    s1->setSeparator(true);

    QAction * s2 = new QAction(this);
    s2->setSeparator(true);

    a_fastRun = new QAction(tr("Fast run"), this);
    a_fastRun->setIcon(QIcon::fromTheme("system-run", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/system-run.png")));
    connect(a_fastRun, SIGNAL(triggered()), this, SLOT(fastRun()));
#ifndef Q_OS_MAC
    a_fastRun->setShortcut(QKeySequence("Ctrl+Shift+F9"));

#else
    a_fastRun->setShortcut(QKeySequence("Ctrl+Shift+R"));
#endif
    a_fastRun->setToolTip(a_fastRun->text()+" <b>"+a_fastRun->shortcut().toString()+"</b>");

    a_blindRun = new QAction(tr("Blind run"), this);
    a_blindRun->setIcon(QIcon::fromTheme("media-seek-forward", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/media-seek-forward.png")));
    connect(a_blindRun, SIGNAL(triggered()), this, SLOT(blindRun()));
#ifndef Q_OS_MAC
    a_blindRun->setShortcut(QKeySequence("Shift+F9"));

#else
    a_blindRun->setShortcut(QKeySequence("Ctrl+R"));
#endif
    a_blindRun->setToolTip(a_blindRun->text()+" <b>"+a_blindRun->shortcut().toString()+"</b>");


    gr_actions = new QActionGroup(this);
    gr_actions->addAction(a_fastRun);
    gr_actions->addAction(a_blindRun);
    gr_actions->addAction(a_regularRun);
    gr_actions->addAction(a_testingRun);
    gr_actions->addAction(s1);
    gr_actions->addAction(a_stepRun);
    gr_actions->addAction(a_stepIn);
    gr_actions->addAction(a_stepOut);
    gr_actions->addAction(s2);
    gr_actions->addAction(a_stop);

    m_variablesWebObject = new KumirVariablesWebObject(this);
    i_timerId = startTimer(1000);
}

void KumirProgram::setAST(const AST::Data *ast)
{
    m_ast = ast;
    m_variablesWebObject->setProgram(m_ast);    
}

KumirVariablesWebObject * KumirProgram::variablesWebObject()
{
    return m_variablesWebObject;
}

void KumirProgram::setNativeGenerator(GeneratorInterface *cpp)
{
    plugin_nativeGenerator = cpp;
    if (cpp) {
        m_process = new QProcess(this);
        connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
                this, SLOT(handleProcessFinished(int,QProcess::ExitStatus)));
        connect(m_process, SIGNAL(error(QProcess::ProcessError)),
                this, SLOT(handleProcessError(QProcess::ProcessError)));


        m_connector = new Connector();
        connect(m_connector, SIGNAL(actorCommandReceived(QString,QString,QVariantList)),
                this, SLOT(handleActorCommand(QString,QString,QVariantList)));
        connect(m_connector, SIGNAL(resetActorReceived(QString)),
                this, SLOT(handleActorResetRequest(QString)));

        m_connector->listenFor(m_process);

    }
    else {
        a_fastRun->deleteLater();
        a_fastRun = 0;
    }
}

void KumirProgram::handleMarginTextRequest(int lineNo, const QString &text)
{
    if (lineNo!=-1 && !text.isEmpty())
        plugin_editor->appendMarginText(i_documentId, lineNo, text);
    if (e_state==StepRun)
        m_variablesWebObject->refreshRoot();
}

void KumirProgram::handleMarginClearRequest(int fromLine, int toLine)
{
    plugin_editor->clearMargin(i_documentId, fromLine, toLine);
}

void KumirProgram::setTerminal(Term *t, QDockWidget * w)
{
    m_terminal = t;
    m_terminalWindow = w;
    if (m_connector) {
        connect(m_connector, SIGNAL(inputFormatReceived(QString)),
                m_terminal, SLOT(input(QString)));
        connect(m_connector, SIGNAL(outputTextReceived(QString)),
                m_terminal, SLOT(output(QString)));
        connect(m_connector, SIGNAL(errorMessageReceived(QString)),
                m_terminal, SLOT(error(QString)));
    }
    connect(m_terminal, SIGNAL(inputFinished(QVariantList)),
            this, SLOT(handleInputDone(QVariantList)));
    connect(m_terminal, SIGNAL(showWindowRequest()),
            m_terminalWindow, SLOT(show()));
}

void KumirProgram::handleInputDone(const QVariantList &data)
{
    if (e_state==RegularRun || e_state==StepRun) {
        Q_CHECK_PTR(plugin_bytecodeRun);
        plugin_bytecodeRun->finishInput(data);
    }
//    else if (e_state==FastRun && m_process->state()==QProcess::Running) {
    else {
        m_connector->sendReply(data);
    }
    m_terminal->clearFocus();
}



void KumirProgram::setBytecodeRun(KPlugin *run)
{
    plugin_bytecodeRun = qobject_cast<RunInterface*>(run);
    connect(run, SIGNAL(inputRequest(QString)), m_terminal, SLOT(input(QString)));
    connect(run, SIGNAL(outputRequest(QString)), m_terminal, SLOT(output(QString)));
    connect(run, SIGNAL(inputRequest(QString)), m_terminalWindow, SLOT(show()));
    connect(run, SIGNAL(inputRequest(QString)), m_terminalWindow, SLOT(show()));
    connect(run, SIGNAL(stopped(int)),
            this, SLOT(handleRunnerStopped(int)));
    connect(run, SIGNAL(lineChanged(int)), this, SLOT(handleLineChanged(int)));
    connect(run, SIGNAL(marginText(int,QString)), this, SLOT(handleMarginTextRequest(int,QString)));
    connect(run, SIGNAL(clearMargin(int,int)), this, SLOT(handleMarginClearRequest(int,int)));
    connect(run, SIGNAL(externalRequest(QString,QString,QVariantList)),
            this, SLOT(handleActorCommand(QString,QString,QVariantList)));
    connect(run, SIGNAL(resetModule(QString)), this, SLOT(handleActorResetRequest(QString)));
}

void KumirProgram::addActor(KPlugin *a, QDockWidget *w)
{
    connect(a, SIGNAL(sync()), this, SLOT(handleActorCommandFinished()));
    m_actors[a->pluginSpec().name] = qobject_cast<ActorInterface*>(a);
    m_actorWindows[a->pluginSpec().name] = w;
}

void KumirProgram::fastRun()
{
    b_processUserTerminated = false;
    s_endStatus = "";
    if (e_state!=Idle) {
        return;
    }
    emit giveMeAProgram();

    QString suffix;
#ifdef Q_OS_WIN32
    suffix = ".exe";
#else
    suffix = ".bin";
#endif
    const QString exeFileName = s_sourceFileName.mid(0, s_sourceFileName.length()-4)+suffix;
    bool mustRegenerate = true;
    if (QFile::exists(exeFileName)) {
        QFileInfo fi(exeFileName);
        QDateTime astTime = m_ast->lastModified;
        QDateTime fileTime = fi.lastModified();
        mustRegenerate = !astTime.isValid() || fileTime < astTime;
    }
    bool ok = !mustRegenerate;
    if (mustRegenerate) {

        QFile exeFile(exeFileName);
        if (exeFile.exists()) {
            if (!exeFile.remove()) {
                qDebug() << "Can't remove existing file " << exeFileName;
            }
        }

#ifdef Q_OS_WIN32
        plugin_nativeGenerator->setVerbose(true);
#endif
        QStringList usedDlls;
        QPair<QString,QString> res = plugin_nativeGenerator->generateExecuable(m_ast, &exeFile, &usedDlls);
        if (!res.first.isEmpty()) {
            qDebug() << "Error generating execuable: " << res.first;
        }
        else {
            ok = true;
        }
        exeFile.close();
#ifdef Q_OS_WIN32
        for (int i=0; i<usedDlls.size(); i++) {
            if (!QFile::exists(usedDlls[i])) {
                if (!QFile::copy(qApp->applicationDirPath()+"/"+usedDlls[i], usedDlls[i])) {
                    ok = false;
                    qDebug() << "Can't put dll into working directory: " << usedDlls[i];
                }
            }
        }
#endif
        QFile::Permissions perms = exeFile.permissions();
        perms |= QFile::ExeOwner;
        perms |= QFile::ExeUser;
        perms |= QFile::ExeGroup;
        perms |= QFile::ExeOther;
        exeFile.setPermissions(perms);

    }
    if (ok) {
        const QString cmd = QString("%1 --key=%2")
                .arg(exeFileName)
                .arg(QCoreApplication::applicationPid());
        m_terminal->start(exeFileName);
        m_process->start(cmd);
        m_process->waitForStarted();
        e_state = FastRun;
        qDebug() << "Started subprocess with PID " << m_process->pid();

        PluginManager::instance()->switchGlobalState(GS_Running);
    }
}

void KumirProgram::blindRun()
{

    if (e_state==FastRun)
        return;
    b_blind = true;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        prepareKumirRunner();
    }
    e_state = RegularRun;
    PluginManager::instance()->switchGlobalState(GS_Running);
    plugin_bytecodeRun->runBlind();
}

void KumirProgram::testingRun()
{
    if (e_state==FastRun)
        return;
    b_blind = true;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        bool hasTestingAlgorhitm = false;
        for (int i=0; i<m_ast->modules.size(); i++) {
            if (m_ast->modules[i]->header.type==AST::ModTypeHidden) {
                for (int j=0; j<m_ast->modules[i]->header.algorhitms.size(); j++) {
                    if (m_ast->modules[i]->header.algorhitms[j]->header.specialType==AST::AlgorhitmTypeTesting) {
                        hasTestingAlgorhitm = true;
                        break;
                    }
                }
            }
        }
        if (!hasTestingAlgorhitm) {
            QMessageBox::information(w_mainWidget, a_testingRun->text(),
                                  tr("This program does not have testing algorithm")
                                  );
            return;
        }
        prepareKumirRunner();
    }
    e_state = RegularRun;
    PluginManager::instance()->switchGlobalState(GS_Running);
    plugin_bytecodeRun->runTesting();
}

void KumirProgram::regularRun()
{

    if (e_state==FastRun)
        return;
    b_blind = false;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        prepareKumirRunner();
    }
    e_state = RegularRun;
    m_variablesWebObject->reset(plugin_bytecodeRun);
    PluginManager::instance()->switchGlobalState(GS_Running);
    plugin_bytecodeRun->runContinuous();
}

void KumirProgram::prepareKumirRunner()
{
    bool mustRegenerate = !m_ast->lastModified.isValid() ||
            !plugin_bytecodeRun->loadedProgramVersion().isValid() ||
            m_ast->lastModified > plugin_bytecodeRun->loadedProgramVersion();
    if (mustRegenerate) {
        QByteArray bufArray;
        QBuffer buffer(&bufArray);
        QPair<QString,QString> res = plugin_bytcodeGenerator->generateExecuable(m_ast, &buffer);

        buffer.open(QIODevice::ReadOnly);
        if (!res.first.isEmpty()) {
            qDebug() << "Error generating execuable: " << res.first;
        }
        else {
            plugin_bytecodeRun->loadProgram(&buffer, Shared::FormatBinary);
        }
        buffer.close();
    }
    const QString exeFileName = s_sourceFileName.mid(0, s_sourceFileName.length()-4)+".kum";
    m_variablesWebObject->setProgram(m_ast);
    m_terminal->start(exeFileName);
}

void KumirProgram::stepRun()
{
    if (e_state==FastRun)
        return;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        prepareKumirRunner();
        m_variablesWebObject->reset(plugin_bytecodeRun);
    }
    e_state = StepRun;
    a_stepRun->setIcon(QIcon::fromTheme("debug-step-over",  QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-over.png")));
    PluginManager::instance()->switchGlobalState(GS_Running);
    plugin_bytecodeRun->runStepOver();
}

void KumirProgram::stepIn()
{
    if (e_state!=StepRun) {
        stepRun();
    }
    else {
        plugin_bytecodeRun->runStepInto();
    }
}

void KumirProgram::stepOut()
{
    if (e_state!=StepRun)
        return;
    plugin_bytecodeRun->runStepOut();
}

void KumirProgram::stop()
{
    if (e_state==StepRun || e_state==RegularRun) {
        plugin_bytecodeRun->terminate();
    }
    else if (e_state==FastRun) {
        b_processUserTerminated = true;
        m_process->kill();
    }
}

void KumirProgram::handleProcessError(QProcess::ProcessError error)
{
    if (b_processUserTerminated)
        return;
    qDebug() << "Process error " << error;
    e_state = Idle;
    m_terminal->error(tr("Unknown error"));
    s_endStatus = tr("Evaluation error");
    PluginManager::instance()->switchGlobalState(GS_Observe);
}

void KumirProgram::handleProcessFinished(int exitCode, QProcess::ExitStatus status)
{
    qDebug() << "Process finished with code " << exitCode << " and status " << status;
    e_state = Idle;
    m_terminal->finish();
    s_endStatus = exitCode==0? tr("Evaluation finished") : tr("Evaluation error");

    PluginManager::instance()->switchGlobalState(GS_Observe);
}

void KumirProgram::handleRunnerStopped(int rr)
{

    Shared::RunInterface::StopReason reason = Shared::RunInterface::StopReason (rr);
    if (reason==Shared::RunInterface::InputRequest) {
        PluginManager::instance()->switchGlobalState(GS_Input);
    }
    else if (reason==Shared::RunInterface::UserInteraction) {
        PluginManager::instance()->switchGlobalState(GS_Pause);
//        a_stepIn->setEnabled(plugin_bytecodeRun->canStepInto());
//        a_stepOut->setEnabled(plugin_bytecodeRun->canStepOut());
    }
    else if (reason==Shared::RunInterface::UserTerminated) {
        m_variablesWebObject->refreshRoot();
        s_endStatus = tr("Evaluation terminated");
        m_terminal->finish();
        PluginManager::instance()->switchGlobalState(GS_Observe);
        e_state = Idle;
        m_terminal->clearFocus();

        plugin_editor->unhighlightLine(i_documentId);
    }
    else if (reason==Shared::RunInterface::Error) {
        s_endStatus = tr("Evaluation error");
        m_variablesWebObject->refreshRoot();
        m_terminal->error(plugin_bytecodeRun->error());
        plugin_editor->highlightLineRed(i_documentId, plugin_bytecodeRun->currentLineNo());
        PluginManager::instance()->switchGlobalState(GS_Observe);
        e_state = Idle;
        m_terminal->clearFocus();
    }
    else if (reason==Shared::RunInterface::Done) {
        s_endStatus = tr("Evaluation finished");
        m_terminal->finish();
        m_variablesWebObject->refreshRoot();
        PluginManager::instance()->switchGlobalState(GS_Observe);
        e_state = Idle;
        m_terminal->clearFocus();
        plugin_editor->unhighlightLine(i_documentId);
    }

    if (e_state==Idle) {
//        m_variablesWebObject->reset(0);
    }

}

void KumirProgram::handleLineChanged(int lineNo)
{
    if (lineNo!=-1) {
        if (plugin_bytecodeRun->error().isEmpty())
            plugin_editor->highlightLineGreen(i_documentId, lineNo);
        else
            plugin_editor->highlightLineRed(i_documentId, lineNo);
    }
    else {
        plugin_editor->unhighlightLine(i_documentId);
    }
}


void KumirProgram::switchGlobalState(GlobalState prev, GlobalState cur)
{
    if (cur==GS_Unlocked || cur==GS_Observe) {
        if (a_fastRun)
            a_fastRun->setEnabled(true);
        a_blindRun->setEnabled(true);
        a_regularRun->setEnabled(true);
        a_testingRun->setEnabled(true);
        a_stepRun->setEnabled(true);
//        a_stepRun->setText(tr("Step run"));
//        a_stepRun->setIcon(QIcon::fromTheme("media-skip-forward", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/media-skip-forward.png")));
        a_stepIn->setEnabled(true);
        a_stepOut->setEnabled(false);
        a_stop->setEnabled(false);

    }
    if (cur==GS_Running || cur==GS_Input) {
        if (a_fastRun)
            a_fastRun->setEnabled(false);
        a_blindRun->setEnabled(false);
        a_regularRun->setEnabled(false);
        a_testingRun->setEnabled(false);
        a_stepRun->setEnabled(false);
        a_stepIn->setEnabled(false);
        a_stepOut->setEnabled(false);
        a_stop->setEnabled(true);
    }
    if (cur==GS_Pause) {
        if (a_fastRun)
            a_fastRun->setEnabled(false);
        a_blindRun->setEnabled(true);
        a_regularRun->setEnabled(true);
        a_stepRun->setEnabled(true);
        a_testingRun->setEnabled(false);
//        a_stepRun->setText(tr("Step over"));
//        a_stepRun->setIcon(QIcon::fromTheme("debug-step-over", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-over.png")));
        a_stepIn->setEnabled(true);
        a_stepOut->setEnabled(true);
        a_stop->setEnabled(true);
    }

    if (prev==GS_Unlocked && cur==GS_Running) {
        m_variablesWebObject->reset(plugin_bytecodeRun);
    }
    if (cur==GS_Unlocked) {
        m_variablesWebObject->reset(0);
    }


}

void KumirProgram::handleActorCommand(
    const QString &actorName,
    const QString &command,
    const QVariantList &arguments)
{
    Q_ASSERT(m_actors.contains(actorName));
    ActorInterface * a = m_actors[actorName];
    Q_CHECK_PTR(a);
    a->runFunct(command, arguments);
}

void KumirProgram::handleActorCommandFinished()
{
    QObject * o = sender();
    Q_CHECK_PTR(o);
    ActorInterface * a = qobject_cast<ActorInterface*>(o);
    Q_CHECK_PTR(a);
    const QString error = a->errorText();
    const QVariant result = a->result();
    const QVariantList res = a->algOptResults();
    if (e_state==FastRun || e_state==Idle) {
        QVariantList message;
        message << error;
        message << result;
        message += res;
        m_connector->sendReply(message);
    }
    if (e_state==RegularRun || e_state==StepRun) {
        Q_CHECK_PTR(plugin_bytecodeRun);
        plugin_bytecodeRun->finishExternalFunctionCall(error, result, res);
    }
}

void KumirProgram::handleActorResetRequest(const QString & actorName)
{
    Q_ASSERT(m_actors.contains(actorName));
    ActorInterface * a = m_actors[actorName];
    Q_CHECK_PTR(a);
    a->reset();
    Q_ASSERT(m_actorWindows.contains(actorName));
    if (m_actorWindows[actorName]) {
        m_actorWindows[actorName]->show();
    }
}

void KumirProgram::timerEvent(QTimerEvent *e)
{
    if (e_state==RegularRun && !b_blind) {
        m_variablesWebObject->refreshRoot();
    }
    e->accept();
}

KumirProgram::~KumirProgram()
{
    if (m_process && m_process->state()!=QProcess::NotRunning) {
        m_process->kill();
    }
    killTimer(i_timerId);
}

} // namespace CoreGui
