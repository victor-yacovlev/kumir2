#include "runplugin.h"
#include "run.h"
#include "extensionsystem/pluginmanager.h"
#include <iostream>
#include <sstream>
#include <locale.h>
#include "util.h"
#include "commonrun.h"
#include "consolerun.h"
#include "guirun.h"
#include "vm/vm_console_handlers.hpp"

#include <QtGui>

namespace KumirCodeRun {

struct CommonFunctors {
    Common::ExternalModuleResetFunctor reset;
    Common::ExternalModuleCallFunctor call;
    Common::CustomTypeFromStringFunctor fromString;
    Common::CustomTypeToStringFunctor toString;
};

struct ConsoleFunctors {
    Console::ExternalModuleLoadFunctor load;
    VM::Console::InputFunctor input;
    VM::Console::OutputFunctor output;
    VM::Console::GetMainArgumentFunctor getMainArgument;
    VM::Console::ReturnMainValueFunctor returnMainValue;
};

struct GuiFunctors {
    Gui::ExternalModuleLoadFunctor load;
    Gui::InputFunctor input;
    Gui::OutputFunctor output;
    Gui::GetMainArgumentFunctor getMainArgument;
    Gui::ReturnMainValueFunctor returnMainValue;
    Gui::PauseFunctor pause;
    Gui::DelayFunctor delay;
};


KumirRunPlugin::KumirRunPlugin()
    : ExtensionSystem::KPlugin()
    , pRun_(new Run(this))
    , common_(nullptr)
    , console_(nullptr)
    , gui_(nullptr)
{
    connect (this, SIGNAL(finishInput(QVariantList)), pRun_, SIGNAL(finishInput(QVariantList)));

    done_ = true;
    connect (pRun_, SIGNAL(output(QString)), this, SIGNAL(outputRequest(QString)));
    connect (pRun_, SIGNAL(input(QString)), this, SIGNAL(inputRequest(QString)));
    connect (pRun_, SIGNAL(finished()), this, SLOT(handleThreadFinished()));
    connect (pRun_, SIGNAL(lineChanged(int,quint32,quint32)), this, SIGNAL(lineChanged(int,quint32,quint32)));
    connect (pRun_, SIGNAL(updateStepsCounter(quint64)), this, SIGNAL(updateStepsCounter(quint64)));
    connect (pRun_, SIGNAL(marginText(int,QString)), this, SIGNAL(marginText(int,QString)));
    connect (pRun_, SIGNAL(clearMarginRequest(int,int)), this, SIGNAL(clearMargin(int,int)));
    connect (pRun_, SIGNAL(marginTextReplace(int,QString,bool)),
             this, SIGNAL(replaceMarginText(int,QString,bool)));
    onlyOneTryToInput_ = false;
}

unsigned long int KumirRunPlugin::stepsCounted() const
{
    return pRun_->vm->stepsDone();
}

int KumirRunPlugin::currentLineNo() const
{
    return pRun_->effectiveLineNo();
}

QPair<quint32,quint32> KumirRunPlugin::currentColumn() const
{
    return QPair<quint32,quint32>(pRun_->vm->effectiveColumn().first, pRun_->vm->effectiveColumn().second);
}

bool KumirRunPlugin::loadProgram(const QString & filename, const QByteArray & source)
{
    bool ok = false;
    std::list<char> buffer;
    for (int i=0; i<source.size(); i++)
        buffer.push_back(source[i]);
    ok = pRun_->loadProgramFromBinaryBuffer(buffer, filename.toStdWString());
    if (!ok) {
        return ok;
    }
    const QString programDirName = QFileInfo(filename).absoluteDir().absolutePath();
    pRun_->setProgramDirectory(programDirName);
    pRun_->programLoaded = ok;
    return ok;
}

QString KumirRunPlugin::error() const
{
    return pRun_->error();
}

QMap<QString,QVariant> KumirRunPlugin::getScalarLocalValues(int frameNo) const
{
    pRun_->lockVMMutex();
    QMap<QString,QVariant> result;
    const std::vector<Variable> & locals = pRun_->vm->getLocals(frameNo);
    for (int i=0; i<locals.size(); i++) {
        const Variable & var = locals.at(i);
        if (var.dimension()==0) {
            const QString varName = QString::fromStdWString(var.myName());
            QVariant value;
            if (var.isValid()) {
                value = QString::fromStdWString(var.toString());
            }
            else {
                value = QVariant::Invalid;
            }
            result.insert(varName, value);
        }
    }
    pRun_->unlockVMMutex();
    return result;
}

QVariant KumirRunPlugin::valueStackTopItem() const
{
    return pRun_->valueStackTopItem();
}

QMap<QString,QVariant> KumirRunPlugin::getScalarGlobalValues(const QString & moduleName) const
{
    pRun_->lockVMMutex();
    QMap<QString,QVariant> result;
    const std::vector<Variable> & locals = pRun_->vm->getGlobals(moduleName.toStdWString());
    for (int i=0; i<locals.size(); i++) {
        const Variable & var = locals.at(i);
        if (var.dimension()==0) {
            const QString varName = QString::fromStdWString(var.myName());
            QVariant value;
            if (var.isValid()) {
                value = QString::fromStdWString(var.toString());
            }
            else {
                value = QVariant::Invalid;
            }
            result.insert(varName, value);
        }
    }
    pRun_->unlockVMMutex();
    return result;
}

QVariantList getTableValues(
        const Variable & var,
        int dimension,
        QList< QPair<int,int> > ranges,
        int & counter,
        const int maxCount,
        bool & complete
        )
{
    int bounds[7];
    var.getEffectiveBounds(bounds);
    QVariantList result;
    if (ranges.size()<dimension) {
        QPair<int,int> maxRange;
        maxRange.first  = bounds[2*(3-dimension)];
        maxRange.second = bounds[2*(3-dimension)+1];
        ranges.push_back(maxRange);
    }
    if (dimension==1) {
        const QPair<int,int> xrange = ranges[0];
        QVariantList & X = result;
        const int xs = xrange.first;
        const int xe = xrange.second;
        for (int x=xs; x<=xe; x++) {
            const QVariant Xvalue = var.hasValue(x)
                    ? QVariant(QString::fromStdWString(var.value(x).toString()))
                    : QVariant(QVariant::Invalid);
            X.push_back(Xvalue);
            counter += 1;
            if (maxCount!=-1 && counter>=maxCount) {
                complete = complete && x==xe;
                break;
            }
        }
    }
    else if (dimension==2) {
        const QPair<int,int> yrange = ranges[0];
        const QPair<int,int> xrange = ranges[1];
        QVariantList & Y = result;
        const int ys = yrange.first;
        const int ye = yrange.second;
        for (int y=ys; y<=ye; y++) {
            QVariantList Yvalue;
            QVariantList & X = Yvalue;
            const int xs = xrange.first;
            const int xe = xrange.second;
            for (int x=xs; x<=xe; x++) {
                const QVariant Xvalue = var.hasValue(y,x)
                        ? QVariant(QString::fromStdWString(var.value(y,x).toString()))
                        : QVariant(QVariant::Invalid);
                X.push_back(Xvalue);
                counter += 1;
                if (maxCount!=-1 && counter>=maxCount) {
                    complete = complete && x==xe;
                    break;
                }
            }
            Y.push_back(Yvalue);
            if (maxCount!=-1 && counter>=maxCount) {
                complete = complete && y==ye;
                break;
            }
        }
    }
    else if (dimension==3) {
        const QPair<int,int> zrange = ranges[0];
        const QPair<int,int> yrange = ranges[1];
        const QPair<int,int> xrange = ranges[2];
        QVariantList & Z = result;
        const int zs = zrange.first;
        const int ze = zrange.second;
        for (int z=zs; z<=ze; z++) {
            QVariantList Zvalue;
            QVariantList & Y = Zvalue;
            const int ys = yrange.first;
            const int ye = yrange.second;
            for (int y=ys; y<=ye; y++) {
                QVariantList Yvalue;
                QVariantList & X = Yvalue;
                const int xs = xrange.first;
                const int xe = xrange.second;
                for (int x=xs; x<=xe; x++) {
                    const QVariant Xvalue = var.hasValue(z,y,x)
                            ? QVariant(QString::fromStdWString(var.value(z,y,x).toString()))
                            : QVariant(QVariant::Invalid);
                    X.push_back(Xvalue);
                    counter += 1;
                    if (maxCount!=-1 && counter>=maxCount) {
                        complete = complete && x==xe;
                        break;
                    }
                }
                Y.push_back(Yvalue);
                if (maxCount!=-1 && counter>=maxCount) {
                    complete = complete && y==ye;
                    break;
                }
            }
            Z.push_back(Zvalue);
            if (maxCount!=-1 && counter>=maxCount) {
                complete = complete && z==ze;
                break;
            }
        }
    }
    return result;
}

QVariantList KumirRunPlugin::getLocalTableValues(
        int frameNo,
        int maxCount,
        const QString &name,
        const QList<QPair<int, int> > &ranges,
        bool & complete
        ) const
{
    QVariantList result;
    int counter = 0;
    pRun_->lockVMMutex();
    const std::vector<Variable> & locals = pRun_->vm->getLocals(frameNo);
    for (int i=0; i<locals.size(); i++) {
        const Variable & var = locals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            complete = true;
            result = getTableValues(var, var.dimension(), ranges, counter, maxCount, complete);
            break;
        }
    }
    pRun_->unlockVMMutex();
    return result;
}

QVariant KumirRunPlugin::getLocalTableValue(
        int frameNo,
        const QString &name,
        const QList<int> &indeces
        ) const
{
    QVariant result;
    int kIndeces[4];
    kIndeces[3] = indeces.size();
    for (int i=0; i<indeces.size(); i++) {
        kIndeces[i] = indeces[i];
    }
    pRun_->lockVMMutex();
    const std::vector<Variable> & locals = pRun_->vm->getLocals(frameNo);
    for (int i=0; i<locals.size(); i++) {
        const Variable & var = locals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            if (var.hasValue(kIndeces))
                result = QString::fromStdWString(var.toString(kIndeces));
            break;
        }
    }
    pRun_->unlockVMMutex();
    return result;
}

QVariantList KumirRunPlugin::getGlobalTableValues(
        const QString & moduleName,
        int maxCount,
        const QString &name,
        const QList<QPair<int, int> > &ranges,
        bool & complete
        ) const
{
    QVariantList result;
    int counter = 0;
    pRun_->lockVMMutex();
    const std::vector<Variable> & globals = pRun_->vm->getGlobals(moduleName.toStdWString());
    for (int i=0; i<globals.size(); i++) {
        const Variable & var = globals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            complete = true;
            result = getTableValues(var, var.dimension(), ranges, counter, maxCount, complete);
            break;
        }
    }
    pRun_->unlockVMMutex();
    return result;
}

QVariant KumirRunPlugin::getGlobalTableValue(
        const QString & moduleName,
        const QString &name,
        const QList<int> &indeces
        ) const
{
    QVariant result;
    int kIndeces[4];
    kIndeces[3] = indeces.size();
    for (int i=0; i<indeces.size(); i++) {
        kIndeces[i] = indeces[i];
    }
    pRun_->lockVMMutex();
    const std::vector<Variable> & globals = pRun_->vm->getGlobals(moduleName.toStdWString());
    for (int i=0; i<globals.size(); i++) {
        const Variable & var = globals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            if (var.hasValue(kIndeces))
                result = QString::fromStdWString(var.toString(kIndeces));
            break;
        }
    }
    pRun_->unlockVMMutex();
    return result;
}



void KumirRunPlugin::runContinuous()
{
    if (done_) {
        pRun_->setEntryPointToMain();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runContinuous();
}

void KumirRunPlugin::runBlind()
{
    if (done_) {
        pRun_->setEntryPointToMain();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runBlind();
}

void KumirRunPlugin::runStepInto()
{
    pRun_->runStepIn();
}

void KumirRunPlugin::runToEnd()
{
    pRun_->runToEnd();
}

void KumirRunPlugin::runStepOver()
{
    if (done_) {
        pRun_->setEntryPointToMain();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runStepOver();
}

void KumirRunPlugin::runTesting()
{
    if (done_) {
        pRun_->setEntryPointToTest();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runBlind();
}

bool KumirRunPlugin::isTestingRun() const
{
    return pRun_->isTestingRun();
}

void KumirRunPlugin::terminate()
{
    if (gui_)
        gui_->delay.stop();
    pRun_->stop();
}


void KumirRunPlugin::handleThreadFinished()
{
    if (pRun_->error().length()>0) {
        done_ = true;
        emit stopped(Shared::RunInterface::SR_Error);
    }
    else if (pRun_->hasMoreInstructions() && pRun_->stopped()) {
        done_ = true;
        emit stopped(Shared::RunInterface::SR_UserTerminated);
    }
    else if (pRun_->hasMoreInstructions()) {
        emit stopped(Shared::RunInterface::SR_UserInteraction);
    }
    else {
        done_ = true;
        emit stopped(Shared::RunInterface::SR_Done);
    }
}

void KumirRunPlugin::handleLineChanged(int lineNo, quint32 colStart, quint32 colEnd)
{
    emit lineChanged(lineNo, colStart, colEnd);
}



KumirRunPlugin::~KumirRunPlugin()
{
    if (pRun_->isRunning()) {
        pRun_->stop();
        pRun_->wait();
    }
    delete pRun_;
    if (gui_)
        delete gui_;
    if (console_)
        delete console_;
    if (common_)
        delete common_;
}


void KumirRunPlugin::prepareCommonRun()
{
    common_ = new CommonFunctors;
    common_->reset.setCallFunctor(&common_->call);
    pRun_->vm->setFunctor(&common_->reset);
    pRun_->vm->setFunctor(&common_->call);
    pRun_->vm->setFunctor(&common_->toString);
    pRun_->vm->setFunctor(&common_->fromString);
}

void KumirRunPlugin::prepareConsoleRun()
{
    if (! common_)
        prepareCommonRun();

    console_ = new ConsoleFunctors;

    const Kumir::Encoding localeEncoding =
        #ifdef Q_OS_WIN32
            Kumir::CP1251;
#else
            Kumir::UTF8;
#endif

    std::deque<std::string> arguments;
    foreach (const QString & arg, qApp->arguments())
        arguments.push_back(arg.toStdString());

    console_->input.setLocale(localeEncoding);
    console_->output.setLocale(localeEncoding);
    console_->getMainArgument.setLocale(localeEncoding);
    console_->returnMainValue.setLocale(localeEncoding);

    console_->input.setCustomTypeFromStringFunctor(&common_->fromString);
    console_->getMainArgument.setCustomTypeFromStringFunctor(&common_->fromString);
    console_->output.setCustomTypeToStringFunctor(&common_->toString);
    console_->returnMainValue.setCustomTypeToStringFunctor(&common_->toString);

    console_->getMainArgument.init(arguments);

    pRun_->vm->setFunctor(&console_->load);
    pRun_->vm->setFunctor(&console_->input);
    pRun_->vm->setFunctor(&console_->output);
    pRun_->vm->setFunctor(&console_->getMainArgument);
    pRun_->vm->setFunctor(&console_->returnMainValue);
    pRun_->vm->setConsoleInputBuffer(&console_->input);
    pRun_->vm->setConsoleOutputBuffer(&console_->output);

}

void KumirRunPlugin::prepareGuiRun()
{
    if (! common_)
        prepareCommonRun();

    gui_ = new GuiFunctors;

    gui_->input.setRunnerInstance(pRun_);
    gui_->output.setRunnerInstance(pRun_);
    gui_->getMainArgument.setRunnerInstance(pRun_);
    gui_->returnMainValue.setRunnerInstance(pRun_);

    gui_->input.setCustomTypeFromStringFunctor(&common_->fromString);
    gui_->output.setCustomTypeToStringFunctor(&common_->toString);
    gui_->getMainArgument.setCustomTypeFromStringFunctor(&common_->fromString);
    gui_->returnMainValue.setCustomTypeToStringFunctor(&common_->toString);

    connect(&gui_->pause, SIGNAL(requestPause()),
            pRun_, SLOT(handlePauseRequest()),
            Qt::DirectConnection
            );

    pRun_->vm->setFunctor(&gui_->load);
    pRun_->vm->setFunctor(&gui_->input);
    pRun_->vm->setFunctor(&gui_->output);
    pRun_->vm->setFunctor(&gui_->getMainArgument);
    pRun_->vm->setFunctor(&gui_->returnMainValue);
    pRun_->vm->setFunctor(&gui_->pause);
    pRun_->vm->setFunctor(&gui_->delay);
    pRun_->vm->setConsoleInputBuffer(&gui_->input);
    pRun_->vm->setConsoleOutputBuffer(&gui_->output);
}

QList<ExtensionSystem::CommandLineParameter>
KumirRunPlugin::acceptableCommandLineParameters() const
{
    using ExtensionSystem::CommandLineParameter;
    QList<CommandLineParameter> result;

    result << CommandLineParameter(
                  false,
                  'p', "pipe",
                  tr("Run in pipeline batch mode without user interaction"));

    result << CommandLineParameter(
                  false,
                  't', "testing",
                  tr("Use testing entry point instead of main"));

    // Startup parameters
    result << CommandLineParameter(
                  false,
                  tr("PROGRAM.kod"),
                  tr("Executable kumir bytecode"),
                  QVariant::String,
                  true
                  );

    result << CommandLineParameter(
                  false,
                  tr("PROGRAM_ARG_%1"),
                  tr("Program argument"),
                  QVariant::String,
                  false
                  );

    return result;
}

QString KumirRunPlugin::initialize(const QStringList &,
                                   const ExtensionSystem::CommandLine & runtimeArguments)
{
    pRun_->programLoaded = false;

    qRegisterMetaType<QVariant::Type>("QVariant::Type");
    qRegisterMetaType< QList<QVariant::Type> >("QList<QVariant::Type>");
    qRegisterMetaType<Shared::RunInterface::StopReason>("Shared::RunInterface::StopReason");

    if (ExtensionSystem::PluginManager::instance()->startupModule()==this) {

        prepareConsoleRun();

        if (runtimeArguments.hasFlag(QChar('p'))) {
            console_->returnMainValue.setQuietMode(true);
            console_->getMainArgument.setQuietMode(true);
        }

        onlyOneTryToInput_ = runtimeArguments.hasFlag(QChar('p'));

#ifndef Q_OS_WIN32
        setlocale(LC_CTYPE, "ru_RU.UTF-8");
#else
        setlocale(LC_CTYPE, ".1251");
#endif

        QString fileName;
        QStringList programArguments;
        for (int i=1; i<qApp->arguments().size(); i++) {
            const QString arg = qApp->arguments()[i];
            if (fileName.isEmpty()) {
                if (!arg.startsWith("-") && !arg.startsWith("[")) {
                    fileName = arg;
                }
            }
            else {
                programArguments << arg;
            }
        }
        if (!fileName.isEmpty()) {
            QFile f(fileName);
            if (f.open(QIODevice::ReadOnly)) {
                const QByteArray data = f.readAll();
                try {
                    loadProgram(fileName, data);
                }
                catch (const std::wstring & message) {
                    return QString::fromStdWString(message);
                }
            }
            if (pRun_->error().length() > 0) {
                return pRun_->error();
            }
        }
        else {
            qApp->setProperty("returnCode", 127);
            return tr("Error: file name not specified.\nRun with --help parameter for more details");
        }
        if (runtimeArguments.hasFlag('t'))
        {
            if (!pRun_->hasTestingAlgorithm()) {
                qApp->setProperty("returnCode", 125);
                return QString::fromUtf8("В ПРОГРАММЕ НЕТ ТЕСТОВОГО АЛГОРИТМА");
            }
            else {
                pRun_->setEntryPointToTest();
            }
        }
    }
    else {
        prepareGuiRun();
    }

    return "";
}

void KumirRunPlugin::timerEvent(QTimerEvent *event) {
    killTimer(event->timerId());
    event->accept();
    pRun_->reset();
    QList<KPlugin*> actors = ExtensionSystem::PluginManager::instance()
            ->loadedPlugins("Actor*");
    foreach (KPlugin * plugin, actors) {
        Shared::ActorInterface * actor =
                qobject_cast<Shared::ActorInterface*>(plugin);
        if (actor)
            actor->setAnimationEnabled(false);
        if (actor && actor->mainWidget()) {
//            actor->mainWidget()->show();
            connect(pRun_, SIGNAL(finished()), actor->mainWidget(), SLOT(close()));
        }
    }
    connect(pRun_, SIGNAL(finished()),
            this, SLOT(checkForErrorInConsole()));
    pRun_->start();
}

void KumirRunPlugin::checkForErrorInConsole()
{
    if (pRun_->error().length() > 0) {
        const QString message = pRun_->effectiveLineNo() != -1
                ?
                    tr("RUNTIME ERROR AT LINE %1: %2")
                    .arg(pRun_->effectiveLineNo())
                    .arg(pRun_->error())
                  : tr("RUNTIME ERROR: %1").arg(pRun_->error());
        std::cerr << message.toLocal8Bit().constData() << std::endl;
    }
}

void KumirRunPlugin::start()
{
    if (pRun_->programLoaded) {
        if (!ExtensionSystem::PluginManager::instance()->isGuiRequired()) {
            pRun_->reset();
            pRun_->start();
            pRun_->wait();
            checkForErrorInConsole();
        }
        else {
            startTimer(0); // start thread after event loop started
        }
    }
}

bool KumirRunPlugin::hasMoreInstructions() const
{
    return pRun_->hasMoreInstructions();
}

bool KumirRunPlugin::canStepOut() const
{
    return pRun_->canStepOut();
}

bool KumirRunPlugin::hasTestingEntryPoint() const
{
    return pRun_->vm->hasTestingAlgorithm();
}

QAbstractItemModel * KumirRunPlugin::debuggerVariablesViewModel() const
{
    return pRun_->variablesModel();
}

} // namespace KumirCodeRun



Q_EXPORT_PLUGIN(KumirCodeRun::KumirRunPlugin)
