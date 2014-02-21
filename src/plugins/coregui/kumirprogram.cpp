#include "kumirprogram.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "dataformats/ast_algorhitm.h"
#include "interfaces/coursesinterface.h"

namespace CoreGUI {

using Shared::ActorInterface;
using namespace ExtensionSystem;

KumirProgram::KumirProgram(QObject *parent)
    : QObject(parent)
    , state_(Idle)
    , terminal_(0)
    , editor_(0)
    , mainWidget_(0)

    , blindRunAction_(0)
    , regularRunAction_(0)
    , testingRunAction_(0)
    , stepRunAction_(0)
    , stepInAction_(0)
    , stepOutAction_(0)
    , stopAction_(0)
    , actions_(0)

    , courseManagerRequest_(false)
{
    createActions();
    createConnections();
}


void KumirProgram::createActions()
{
    regularRunAction_ = new QAction(tr("Regular run"), this);
    const QString qtcreatorIconsPath = ExtensionSystem::PluginManager::instance()->sharePath()
            + "/icons/from_qtcreator/";
    regularRunAction_->setIcon(QIcon(qtcreatorIconsPath+"debugger_start.png"));
    connect(regularRunAction_, SIGNAL(triggered()), this, SLOT(regularRun()));
#ifndef Q_OS_MAC
    regularRunAction_->setShortcut(QKeySequence("F9"));
#else
    regularRunAction_->setShortcut(QKeySequence("F5"));
#endif
    regularRunAction_->setToolTip(regularRunAction_->text()+" <b>"+regularRunAction_->shortcut().toString()+"</b>");

    testingRunAction_ = new QAction(tr("Testing run"), this);
    connect(testingRunAction_, SIGNAL(triggered()), this, SLOT(testingRun()));
    testingRunAction_->setShortcut(QKeySequence("Ctrl+T"));
    testingRunAction_->setToolTip(testingRunAction_->text()+" <b>"+testingRunAction_->shortcut().toString()+"</b>");

    stepRunAction_ = new QAction(tr("Step over"), this);
    stepRunAction_->setIcon(QIcon(qtcreatorIconsPath+"debugger_steponeproc_small.png"));
    connect(stepRunAction_, SIGNAL(triggered()), this, SLOT(stepRun()));
#ifndef Q_OS_MAC
    stepRunAction_->setShortcut(QKeySequence("F8"));
#else
    stepRunAction_->setShortcut(QKeySequence("F6"));
#endif
    stepRunAction_->setToolTip(tr("Do big step")+" <b>"+stepRunAction_->shortcut().toString()+"</b>");

    stepInAction_ = new QAction(tr("Step in"), this);
    stepInAction_->setIcon(QIcon(qtcreatorIconsPath+"debugger_stepinto_small.png"));
    connect(stepInAction_, SIGNAL(triggered()), this, SLOT(stepIn()));
#ifndef Q_OS_MAC
    stepInAction_->setShortcut(QKeySequence("F7"));
#else
    stepInAction_->setShortcut(QKeySequence("F7"));
#endif
    stepInAction_->setToolTip(tr("Do small step")+" <b>"+stepInAction_->shortcut().toString()+"</b>");

    stepOutAction_ = new QAction(tr("Step to end"), this);
    stepOutAction_->setIcon(QIcon(qtcreatorIconsPath+"debugger_stepoverproc_small.png"));
    connect(stepOutAction_, SIGNAL(triggered()), this, SLOT(stepOut()));
#ifndef Q_OS_MAC
    stepOutAction_->setShortcut(QKeySequence("Shift+F8"));
#else
    stepOutAction_->setShortcut(QKeySequence("Shift+F8"));
#endif
    stepOutAction_->setToolTip(tr("Run to end of algorhitm")+" <b>"+stepOutAction_->shortcut().toString()+"</b>");

    stopAction_ = new QAction(tr("Stop"), this);
    stopAction_->setIcon(QIcon(qtcreatorIconsPath+"stop.png"));
    connect(stopAction_, SIGNAL(triggered()), this, SLOT(stop()));
#ifndef Q_OS_MAC
    stopAction_->setShortcut(QKeySequence("Esc"));
#else
    stopAction_->setShortcut(QKeySequence("Esc"));
#endif
    stopAction_->setToolTip(stopAction_->text()+" <b>"+stopAction_->shortcut().toString()+"</b>");

    stepInAction_->setEnabled(false);
    stepOutAction_->setEnabled(false);
    stopAction_->setEnabled(false);

    QAction * s1 = new QAction(this);
    s1->setSeparator(true);

    QAction * s2 = new QAction(this);
    s2->setSeparator(true);


    blindRunAction_ = new QAction(tr("Blind run"), this);
    blindRunAction_->setIcon(QIcon(qtcreatorIconsPath+"run.png"));
    connect(blindRunAction_, SIGNAL(triggered()), this, SLOT(blindRun()));
#ifndef Q_OS_MAC
    blindRunAction_->setShortcut(QKeySequence("Shift+F9"));

#else
    blindRunAction_->setShortcut(QKeySequence("Ctrl+R"));
#endif
    blindRunAction_->setToolTip(blindRunAction_->text()+" <b>"+blindRunAction_->shortcut().toString()+"</b>");


    actions_ = new QActionGroup(this);
    actions_->addAction(blindRunAction_);
    actions_->addAction(regularRunAction_);
    actions_->addAction(testingRunAction_);
    actions_->addAction(stopAction_);
    actions_->addAction(s1);
    actions_->addAction(stepRunAction_);
    actions_->addAction(stepInAction_);
    actions_->addAction(stepOutAction_);
}

Shared::RunInterface * KumirProgram::runner()
{
    using namespace ExtensionSystem;
    using namespace Shared;

    static RunInterface * RUNNER = nullptr;

    if (!RUNNER) {
        RUNNER = PluginManager::instance()->findPlugin<RunInterface>();
    }

    return RUNNER;
}

Shared::GeneratorInterface * KumirProgram::generator()
{
    using namespace ExtensionSystem;
    using namespace Shared;

    static GeneratorInterface * GENERATOR = nullptr;

    if (!GENERATOR) {
        GENERATOR = PluginManager::instance()->findPlugin<GeneratorInterface>();
    }

    return GENERATOR;
}



void KumirProgram::createConnections()
{
    using namespace ExtensionSystem;
    using namespace Shared;

    KPlugin * run =
            PluginManager::instance()->findKPlugin<RunInterface>();

    connect(run, SIGNAL(stopped(int)),
            this, SLOT(handleRunnerStopped(int)));
    connect(run, SIGNAL(lineChanged(int,quint32,quint32)), this, SLOT(handleLineChanged(int,quint32,quint32)));
    connect(run, SIGNAL(marginText(int,QString)), this, SLOT(handleMarginTextRequest(int,QString)));
    connect(run, SIGNAL(clearMargin(int,int)), this, SLOT(handleMarginClearRequest(int,int)));
    connect(run, SIGNAL(replaceMarginText(int,QString, bool)),
            this, SLOT(handleMarginTextReplace(int,QString,bool)));
}

void KumirProgram::handleMarginTextReplace(int lineNo, const QString &text, bool redFgColor)
{
    if (lineNo!=-1 && !text.isEmpty())
        editor_->setMarginText(lineNo, text, redFgColor ? QColor("red") : QColor("black"));
}

void KumirProgram::handleMarginTextRequest(int lineNo, const QString &text)
{
    if (lineNo!=-1 && !text.isEmpty())
        editor_->appendMarginText(lineNo, text);
}

void KumirProgram::handleMarginClearRequest(int fromLine, int toLine)
{
    editor_->clearMarginText(fromLine, toLine);
}

void KumirProgram::setTerminal(Term *t, QDockWidget * w)
{
    using namespace ExtensionSystem;
    using namespace Shared;
    terminal_ = t;

    KPlugin * plugin_bytecodeRunObject =
            PluginManager::instance()->findKPlugin<RunInterface>();

    connect(terminal_, SIGNAL(inputFinished(QVariantList)),
            plugin_bytecodeRunObject, SIGNAL(finishInput(QVariantList)));
    connect(plugin_bytecodeRunObject, SIGNAL(inputRequest(QString)),
            terminal_, SLOT(input(QString)));
    connect(plugin_bytecodeRunObject, SIGNAL(outputRequest(QString)),
            terminal_, SLOT(output(QString)));
    connect(plugin_bytecodeRunObject, SIGNAL(errorOutputRequest(QString)),
            terminal_, SLOT(outputErrorStream(QString)));
}



void KumirProgram::blindRun()
{
    using namespace Shared;
    endStatus_ = "";
    if (state_==Idle) {
        emit giveMeAProgram();
        QString message = prepareKumirRunner(GeneratorInterface::LinesOnly,
                                             Analizer::RegularRun);
        if (!message.isEmpty()) {
            QMessageBox::information(mainWidget_,
                                     tr("Can't run"),
                                     message);
            return;
        }
    }
    state_ = BlindRun;
    PluginManager::instance()->switchGlobalState(PluginInterface::GS_Running);
    setAllActorsAnimationFlag(false);
    runner()->runBlind();
}

void KumirProgram::testingRun()
{
    using namespace Shared;
    endStatus_ = "";
    if (state_==Idle) {
        emit giveMeAProgram();
        QString message = prepareKumirRunner(GeneratorInterface::LinesOnly,
                                             Analizer::TestingRun);
        if (!message.isEmpty()) {
            QMessageBox::information(mainWidget_,
                                     tr("Can't run"),
                                     message);
            return;
        }
        if (!runner()->hasTestingEntryPoint()) {
            QMessageBox::information(mainWidget_, testingRunAction_->text(),
                                  tr("This program does not have testing algorithm")
                                  );
            return;
        }
    }
    state_ = TestingRun;
    PluginManager::instance()->switchGlobalState(PluginInterface::GS_Running);
    setAllActorsAnimationFlag(false);
    runner()->runTesting();
}

void KumirProgram::regularRun()
{
    using namespace Shared;
    endStatus_ = "";
    if (state_==Idle) {
        emit giveMeAProgram();
        QString message = prepareKumirRunner(GeneratorInterface::LinesAndVariables,
                                             Analizer::RegularRun);
        if (!message.isEmpty()) {
            QMessageBox::information(mainWidget_,
                                     tr("Can't run"),
                                     message);
            return;
        }
    }
    state_ = RegularRun;
    PluginManager::instance()->switchGlobalState(PluginInterface::GS_Running);
    setAllActorsAnimationFlag(true);
    runner()->runContinuous();
}

QString KumirProgram::prepareKumirRunner(Shared::GeneratorInterface::DebugLevel debugLevel,
                                      Shared::Analizer::RunTarget target)
{
    bool ok = false;
    QString sourceProgramPath;
    QString errorMessage;
    using namespace Shared::Analizer;
    using Shared::RunInterface;
    InstanceInterface* analizer = editor_->analizer();
    ASTCompilerInterface* astCompiler = analizer->compiler();
    ExternalExecutableCompilerInterface* exeCompiler = analizer->externalExecutableCompiler();    
    RunInterface::SourceInfo sourceInfo;
    if (analizer) {
        Q_ASSERT(astCompiler!=0 || exeCompiler!=0);
        editor_->ensureAnalized();
        if (astCompiler) {
            const AST::DataPtr ast = editor_->analizer()->compiler()->abstractSyntaxTree();
            sourceProgramPath = editor_->documentContents().sourceUrl.toLocalFile();
            QByteArray bufArray;
            generator()->setOutputToText(false);
            generator()->setDebugLevel(debugLevel);
            QString fileNameSuffix, mimeType;
            generator()->generateExecuable(ast, bufArray, mimeType, fileNameSuffix);
            runner()->loadProgram(sourceProgramPath, bufArray, sourceInfo);
        }
        else if (exeCompiler) {
            errorMessage = editor_->analizer()->externalExecutableCompiler()->prepareToRun(target);
            sourceInfo.sourceFileName = exeCompiler->debuggableSourceFileName();
            ok = errorMessage.isEmpty();
            if (ok) {
                ok = runner()->loadProgram(
                            editor_->analizer()->externalExecutableCompiler()->
                            executableFilePath(), QByteArray(), sourceInfo
                            );
                if (!ok) {
                    errorMessage = runner()->error();
                }
            }
        }
    }
    else {
        const KumFile::Data source = editor_->documentContents();
        sourceProgramPath = source.sourceUrl.toLocalFile();
        const QString sourceText = source.hasHiddenText
                ? source.visibleText + "\n" + source.hiddenText
                : source.visibleText;
        const QByteArray sourceData = sourceText.toUtf8();
        ok = runner()->loadProgram(sourceProgramPath, sourceData, sourceInfo);
        if (!ok) {
            if (!ok) {
                errorMessage = runner()->error();
            }
        }
    }
    if (ok) {
        const QString newCwd = QFileInfo(sourceProgramPath).absoluteDir().absolutePath();
        QDir::setCurrent(newCwd);
        terminal_->start(sourceProgramPath);
    }
    return errorMessage;
}

void KumirProgram::stepRun()
{
    using namespace Shared;
    endStatus_ = "";
    if (state_==Idle) {
        emit giveMeAProgram();
        QString message = prepareKumirRunner(GeneratorInterface::LinesAndVariables,
                                             Analizer::RegularRun);
        if (!message.isEmpty()) {
            QMessageBox::information(mainWidget_,
                                     tr("Can't run"),
                                     message);
            return;
        }
    }
    state_ = StepRun;
//    stepRunAction_->setIcon(QIcon::fromTheme("debug-step-over",  QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-over.png")));
    PluginManager::instance()->switchGlobalState(PluginInterface::GS_Running);
    setAllActorsAnimationFlag(true);
    runner()->runStepOver();
}

void KumirProgram::stepIn()
{
    if (state_!=StepRun) {
        stepRun();
    }
    else {
        setAllActorsAnimationFlag(true);
        runner()->runStepInto();
    }
}

void KumirProgram::stepOut()
{
    if (state_!=StepRun)
        return;
    setAllActorsAnimationFlag(true);
    runner()->runToEnd();
}

void KumirProgram::stop()
{
    if (state_==StepRun || state_==RegularRun || state_==TestingRun || state_==BlindRun) {
        runner()->terminate();
    }    
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
    using namespace Shared;
    const State previousState = state_;
    RunInterface::StopReason reason = RunInterface::StopReason (rr);
    if (reason==RunInterface::SR_InputRequest) {
        PluginManager::instance()->switchGlobalState(PluginInterface::GS_Input);
    }
    else if (reason==RunInterface::SR_UserInteraction) {
        PluginManager::instance()->switchGlobalState(PluginInterface::GS_Pause);
//        a_stepIn->setEnabled(plugin_bytecodeRun->canStepInto());
//        a_stepOut->setEnabled(plugin_bytecodeRun->canStepOut());
    }
    else if (reason==RunInterface::SR_UserTerminated) {
        endStatus_ = tr("Evaluation terminated");
        terminal_->finish();
        PluginManager::instance()->switchGlobalState(PluginInterface::GS_Observe);
        state_ = Idle;
        terminal_->clearFocus();
        editor_->unhighlightLine();
    }
    else if (reason==RunInterface::SR_Error) {
        endStatus_ = tr("Evaluation error");
        terminal_->error(runner()->error());
        editor_->highlightLineRed(runner()->currentLineNo(), runner()->currentColumn().first, runner()->currentColumn().second);
        PluginManager::instance()->switchGlobalState(PluginInterface::GS_Observe);
        state_ = Idle;
        terminal_->clearFocus();
    }
    else if (reason==RunInterface::SR_Done) {
        endStatus_ = tr("Evaluation finished");
        terminal_->finish();
        PluginManager::instance()->switchGlobalState(PluginInterface::GS_Observe);
        state_ = Idle;
        terminal_->clearFocus();
        editor_->unhighlightLine();
    }

    typedef CoursesInterface CI;
    CI * courseManager =
            ExtensionSystem::PluginManager::instance()->findPlugin<CI>();
    typedef RunInterface RI;
    RI * runner =
            ExtensionSystem::PluginManager::instance()->findPlugin<RI>();
    if (courseManager && runner->isTestingRun() && courseManagerRequest_) {
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
    if (lineNo!=-1) {
        if (runner()->error().isEmpty())
            editor_->highlightLineGreen(lineNo, colStart, colEnd);
        else
            editor_->highlightLineRed(lineNo, colStart, colEnd);
    }
    else {
        editor_->unhighlightLine();
    }
}


void KumirProgram::switchGlobalState(GlobalState prev, GlobalState cur)
{
    using Shared::PluginInterface;
    if (cur==PluginInterface::GS_Unlocked || cur==PluginInterface::GS_Observe) {

        blindRunAction_->setEnabled(true);
        regularRunAction_->setEnabled(true);
        testingRunAction_->setEnabled(true);
        stepRunAction_->setEnabled(true);
//        a_stepRun->setText(tr("Step run"));
//        a_stepRun->setIcon(QIcon::fromTheme("media-skip-forward", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/media-skip-forward.png")));
        stepInAction_->setEnabled(true);
        stepOutAction_->setEnabled(false);
        stopAction_->setEnabled(false);

    }
    if (cur==PluginInterface::GS_Running || cur==PluginInterface::GS_Input) {

        blindRunAction_->setEnabled(false);
        regularRunAction_->setEnabled(false);
        testingRunAction_->setEnabled(false);
        stepRunAction_->setEnabled(false);
        stepInAction_->setEnabled(false);
        stepOutAction_->setEnabled(false);
        stopAction_->setEnabled(true);
    }
    if (cur==PluginInterface::GS_Pause) {

        blindRunAction_->setEnabled(true);
        regularRunAction_->setEnabled(true);
        stepRunAction_->setEnabled(true);
        testingRunAction_->setEnabled(false);
//        a_stepRun->setText(tr("Step over"));
//        a_stepRun->setIcon(QIcon::fromTheme("debug-step-over", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/debug-step-over.png")));
        stepInAction_->setEnabled(true);
        stepOutAction_->setEnabled(true);
        stopAction_->setEnabled(true);
    }
}

} // namespace CoreGui
