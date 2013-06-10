#include "kumirprogram.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "dataformats/ast_algorhitm.h"
#include "debuggerwindow.h"
#include "interfaces/coursesinterface.h"

namespace CoreGUI {

using Shared::ActorInterface;
using namespace ExtensionSystem;

KumirProgram::KumirProgram(QObject *parent)
    : QObject(parent)
    , e_state(Idle)
    , m_ast(0)
    , m_process(0)
    , m_terminal(0)
    , plugin_bytcodeGenerator(0)
    , plugin_bytecodeRun(0)
    , plugin_editor(0)
    , a_blindRun(0)
    , a_regularRun(0)
    , a_testingRun(0)
    , a_stepRun(0)
    , a_stepIn(0)
    , a_stepOut(0)
    , a_stop(0)
    , gr_actions(0)
    , w_mainWidget(0)
    , w_debuggerWindow(0)
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
    gr_actions->addAction(a_blindRun);
    gr_actions->addAction(a_regularRun);
    gr_actions->addAction(a_testingRun);
    gr_actions->addAction(s1);
    gr_actions->addAction(a_stepRun);
    gr_actions->addAction(a_stepIn);
    gr_actions->addAction(a_stepOut);
    gr_actions->addAction(s2);
    gr_actions->addAction(a_stop);

    i_timerId = startTimer(1000);
}

void KumirProgram::setAST(const AST::DataPtr ast)
{
    m_ast = ast;
}

void KumirProgram::handleMarginTextReplace(int lineNo, const QString &text, bool redFgColor)
{
    if (lineNo!=-1 && !text.isEmpty())
        plugin_editor->setMarginText(
                    documentId_,
                    lineNo,
                    text,
                    redFgColor ? QColor("red") : QColor("black")
                    );
}

void KumirProgram::handleMarginTextRequest(int lineNo, const QString &text)
{
    if (lineNo!=-1 && !text.isEmpty())
        plugin_editor->appendMarginText(documentId_, lineNo, text);
}

void KumirProgram::handleMarginClearRequest(int fromLine, int toLine)
{
    plugin_editor->clearMargin(documentId_, fromLine, toLine);
}

void KumirProgram::setTerminal(Term *t, QDockWidget * w)
{
    using namespace ExtensionSystem;
    using namespace Shared;
    m_terminal = t;
    m_terminalWindow = w;

    KPlugin * plugin_bytecodeRunObject =
            PluginManager::instance()->findKPlugin<RunInterface>();

    connect(m_terminal, SIGNAL(inputFinished(QVariantList)),
            plugin_bytecodeRunObject, SIGNAL(finishInput(QVariantList)));
    connect(plugin_bytecodeRunObject, SIGNAL(inputRequest(QString)),
            m_terminal, SLOT(input(QString)));
    connect(plugin_bytecodeRunObject, SIGNAL(outputRequest(QString)),
            m_terminal, SLOT(output(QString)));
    connect(plugin_bytecodeRunObject, SIGNAL(errorOutputRequest(QString)),
            m_terminal, SLOT(outputErrorStream(QString)));
}


void KumirProgram::setBytecodeRun(KPlugin *run)
{
    plugin_bytecodeRun = qobject_cast<RunInterface*>(run);

    connect(run, SIGNAL(stopped(int)),
            this, SLOT(handleRunnerStopped(int)));
    connect(run, SIGNAL(lineChanged(int,quint32,quint32)), this, SLOT(handleLineChanged(int,quint32,quint32)));
    connect(run, SIGNAL(marginText(int,QString)), this, SLOT(handleMarginTextRequest(int,QString)));
    connect(run, SIGNAL(clearMargin(int,int)), this, SLOT(handleMarginClearRequest(int,int)));
    connect(run, SIGNAL(replaceMarginText(int,QString, bool)),
            this, SLOT(handleMarginTextReplace(int,QString,bool)));
}


void KumirProgram::fastRun()
{
    if (w_debuggerWindow) w_debuggerWindow->reset();
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

//        QPair<QString,QString> res = plugin_nativeGenerator->generateExecuable(m_ast, &exeFile);
//        if (!res.first.isEmpty()) {
//            qDebug() << "Error generating execuable: " << res.first;
//        }
//        else {
//            ok = true;
//        }
        exeFile.close();
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
        setAllActorsAnimationFlag(false);
    }
}

void KumirProgram::blindRun()
{
    if (w_debuggerWindow) w_debuggerWindow->reset();
    if (e_state==FastRun)
        return;
    b_blind = true;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        prepareKumirRunner(Shared::GeneratorInterface::LinesOnly);
    }
    e_state = RegularRun;
    PluginManager::instance()->switchGlobalState(GS_Running);
    setAllActorsAnimationFlag(false);
    plugin_bytecodeRun->runBlind();
}

void KumirProgram::testingRun()
{
    if (w_debuggerWindow) w_debuggerWindow->reset();
    if (e_state==FastRun)
        return;

    using namespace ExtensionSystem;
    using namespace Shared;
    RunInterface * runner =
            PluginManager::instance()->findPlugin<RunInterface>();

    b_blind = true;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        prepareKumirRunner(Shared::GeneratorInterface::LinesOnly);        
        if (!runner->hasTestingEntryPoint()) {
            QMessageBox::information(w_mainWidget, a_testingRun->text(),
                                  tr("This program does not have testing algorithm")
                                  );
            return;
        }
    }
    e_state = TestingRun;
    PluginManager::instance()->switchGlobalState(GS_Running);
    setAllActorsAnimationFlag(false);
    runner->runTesting();
}

void KumirProgram::regularRun()
{
    if (w_debuggerWindow) w_debuggerWindow->reset();
    if (e_state==FastRun)
        return;
    b_blind = false;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        prepareKumirRunner(Shared::GeneratorInterface::LinesAndVariables);
    }
    e_state = RegularRun;
    PluginManager::instance()->switchGlobalState(GS_Running);
    setAllActorsAnimationFlag(true);
    plugin_bytecodeRun->runContinuous();
}

void KumirProgram::prepareKumirRunner(Shared::GeneratorInterface::DebugLevel debugLevel)
{
    using namespace ExtensionSystem;
    using namespace Shared;
    AnalizerInterface * analizer =
            PluginManager::instance()->findPlugin<AnalizerInterface>();
    RunInterface * runner =
            PluginManager::instance()->findPlugin<RunInterface>();
    bool ok = false;
    QString exeFileName;
    if (analizer->resultType() == AnalizerInterface::RT_AST) {
        if (w_debuggerWindow) w_debuggerWindow->reset();
        ok = true;
        bool mustRegenerate = !m_ast->lastModified.isValid() ||
                !runner->loadedProgramVersion().isValid() ||
                m_ast->lastModified > runner->loadedProgramVersion();
        if (mustRegenerate) {
            QByteArray bufArray;
            QPair<QString,QString> res = plugin_bytcodeGenerator->generateExecuable(m_ast, bufArray, debugLevel);
            if (!res.first.isEmpty()) {
                qDebug() << "Error generating execuable: " << res.first;
                ok = false;
            }
            else {
                ok = runner->loadProgram(s_sourceFileName, bufArray, Shared::FormatBinary);

            }
        }
        exeFileName =
                s_sourceFileName.mid(0, s_sourceFileName.length()-4) +
                analizer->defaultDocumentFileNameSuffix();
    }
    else if (analizer->resultType() == AnalizerInterface::RT_Source) {
        EditorInterface * editor =
                PluginManager::instance()->findPlugin<EditorInterface>();
        const KumFile::Data source = editor->documentContent(documentId());
        const QString sourceText = source.hasHiddenText
                ? source.visibleText + "\n" + source.hiddenText
                : source.visibleText;
        const QByteArray sourceData = sourceText.toUtf8();
        exeFileName = s_sourceFileName;
        ok = runner->loadProgram(s_sourceFileName, sourceData, Shared::FormatText);
    }
    else if (analizer->resultType() == AnalizerInterface::RT_Binary) {
        ok = false;  // not implemented
    }
    if (ok) {
        m_terminal->start(exeFileName);
    }
    else {
        handleRunnerStopped(Shared::RunInterface::SR_Error);
    }
}

void KumirProgram::stepRun()
{
    if (e_state==FastRun)
        return;
    s_endStatus = "";
    if (e_state==Idle) {
        emit giveMeAProgram();
        prepareKumirRunner(Shared::GeneratorInterface::LinesAndVariables);
    }
    e_state = StepRun;
    a_stepRun->setIcon(QIcon::fromTheme("debug-step-over",  QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-over.png")));
    PluginManager::instance()->switchGlobalState(GS_Running);
    setAllActorsAnimationFlag(true);
    plugin_bytecodeRun->runStepOver();
}

void KumirProgram::stepIn()
{
    if (e_state!=StepRun) {
        stepRun();
    }
    else {
        setAllActorsAnimationFlag(true);
        plugin_bytecodeRun->runStepInto();
    }
}

void KumirProgram::stepOut()
{
    if (e_state!=StepRun)
        return;
    setAllActorsAnimationFlag(true);
    plugin_bytecodeRun->runStepOut();
}

void KumirProgram::stop()
{
    if (e_state==StepRun || e_state==RegularRun || e_state==TestingRun) {
        plugin_bytecodeRun->terminate();
    }
    else if (e_state==FastRun) {
        b_processUserTerminated = true;
        m_process->kill();
    }
    if (w_debuggerWindow) w_debuggerWindow->reset();
}


void KumirProgram::setAllActorsAnimationFlag(bool animationEnabled)
{
    QList<KPlugin*> actorPlugins = ExtensionSystem::PluginManager::instance()
            ->loadedPlugins("Actor*");
    foreach (KPlugin * plugin , actorPlugins) {
        Shared::ActorInterface * actor =
                qobject_cast<Shared::ActorInterface*>(plugin);
        if (actor) {
            actor->setAnimationEnabled(animationEnabled);
        }
    }
}

void KumirProgram::handleRunnerStopped(int rr)
{
    const State previousState = e_state;
    Shared::RunInterface::StopReason reason = Shared::RunInterface::StopReason (rr);
    if (reason==Shared::RunInterface::SR_InputRequest) {
        PluginManager::instance()->switchGlobalState(GS_Input);
    }
    else if (reason==Shared::RunInterface::SR_UserInteraction) {
        PluginManager::instance()->switchGlobalState(GS_Pause);
//        a_stepIn->setEnabled(plugin_bytecodeRun->canStepInto());
//        a_stepOut->setEnabled(plugin_bytecodeRun->canStepOut());
    }
    else if (reason==Shared::RunInterface::SR_UserTerminated) {
        s_endStatus = tr("Evaluation terminated");
        m_terminal->finish();
        PluginManager::instance()->switchGlobalState(GS_Observe);
        e_state = Idle;
        m_terminal->clearFocus();

        plugin_editor->unhighlightLine(documentId_);
    }
    else if (reason==Shared::RunInterface::SR_Error) {
        s_endStatus = tr("Evaluation error");
        m_terminal->error(plugin_bytecodeRun->error());
        plugin_editor->highlightLineRed(documentId_, plugin_bytecodeRun->currentLineNo(), plugin_bytecodeRun->currentColumn().first, plugin_bytecodeRun->currentColumn().second);
        PluginManager::instance()->switchGlobalState(GS_Observe);
        e_state = Idle;
        m_terminal->clearFocus();
    }
    else if (reason==Shared::RunInterface::SR_Done) {
        s_endStatus = tr("Evaluation finished");
        m_terminal->finish();
        PluginManager::instance()->switchGlobalState(GS_Observe);
        e_state = Idle;
        m_terminal->clearFocus();
        plugin_editor->unhighlightLine(documentId_);
        if (w_debuggerWindow) w_debuggerWindow->reset();
    }

    typedef Shared::CoursesInterface CI;
    CI * courseManager =
            ExtensionSystem::PluginManager::instance()->findPlugin<CI>();
    typedef Shared::RunInterface RI;
    RI * runner =
            ExtensionSystem::PluginManager::instance()->findPlugin<RI>();
    if (courseManager && previousState == TestingRun) {
        if (reason == Shared::RunInterface::SR_UserTerminated) {
            courseManager->setTestingResult(CI::UserTerminated, 0);
        }
        else if (reason == Shared::RunInterface::SR_Done) {
            courseManager->setTestingResult(CI::SuccessfullyFinished,
                                            runner->valueStackTopItem().toInt());
        }
        else if (reason == Shared::RunInterface::SR_Error) {
            courseManager->setTestingResult(CI::AbortedOnError, 0);
        }
    }

}

void KumirProgram::handleLineChanged(int lineNo, quint32 colStart, quint32 colEnd)
{
    emit activateDocumentTab(documentId_);
    if (lineNo!=-1) {
        if (plugin_bytecodeRun->error().isEmpty())
            plugin_editor->highlightLineGreen(documentId_, lineNo, colStart, colEnd);
        else
            plugin_editor->highlightLineRed(documentId_, lineNo, colStart, colEnd);
    }
    else {
        plugin_editor->unhighlightLine(documentId_);
    }
}


void KumirProgram::switchGlobalState(GlobalState prev, GlobalState cur)
{
    if (cur==GS_Unlocked || cur==GS_Observe) {

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

        a_blindRun->setEnabled(false);
        a_regularRun->setEnabled(false);
        a_testingRun->setEnabled(false);
        a_stepRun->setEnabled(false);
        a_stepIn->setEnabled(false);
        a_stepOut->setEnabled(false);
        a_stop->setEnabled(true);
    }
    if (cur==GS_Pause) {

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
}


void KumirProgram::timerEvent(QTimerEvent *e)
{
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
