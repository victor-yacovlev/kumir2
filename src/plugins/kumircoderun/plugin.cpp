#include "plugin.h"
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

Plugin::Plugin()
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


    connect(pRun_, SIGNAL(signal_debuggerReset()), this, SIGNAL(debuggerReset()), Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerPopContext()), this, SIGNAL(debuggerPopContext()), Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerPushContext(QString,QStringList,QStringList,QList<int>)),
            this, SIGNAL(debuggerPushContext(QString,QStringList,QStringList,QList<int>)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerSetGlobals(QString,QStringList,QStringList,QList<int>)),
            this, SIGNAL(debuggerSetGlobals(QString,QStringList,QStringList,QList<int>)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerUpdateLocalVariable(QString,QString)),
            this, SIGNAL(debuggerUpdateLocalVariable(QString,QString)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerUpdateGlobalVariable(QString,QString,QString)),
            this, SIGNAL(debuggerUpdateGlobalVariable(QString,QString,QString)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerUpdateLocalTableBounds(QString,QList<int>)),
            this, SIGNAL(debuggerUpdateLocalTableBounds(QString,QList<int>)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerUpdateGlobalTableBounds(QString,QString,QList<int>)),
            this, SIGNAL(debuggerUpdateGlobalTableBounds(QString,QString,QList<int>)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerSetLocalReference(QString,QString,QList<int>,int,QString)),
            this, SIGNAL(debuggerSetLocalReference(QString,QString,QList<int>,int,QString)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerForceUpdateValues()),
            this, SIGNAL(debuggerForceUpdateValues()),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerUpdateLocalTableValue(QString,QList<int>)),
            this, SIGNAL(debuggerUpdateLocalTableValue(QString,QList<int>)),
            Qt::DirectConnection);
    connect(pRun_, SIGNAL(signal_debuggerUpdateGlobalTableValue(QString,QString,QList<int>)),
            this, SIGNAL(debuggerUpdateGlobalTableValue(QString,QString,QList<int>)),
            Qt::DirectConnection);
}

unsigned long int Plugin::stepsCounted() const
{
    return pRun_->vm->stepsDone();
}

int Plugin::currentLineNo() const
{
    return pRun_->effectiveLineNo();
}

QPair<quint32,quint32> Plugin::currentColumn() const
{
    return QPair<quint32,quint32>(pRun_->vm->effectiveColumn().first, pRun_->vm->effectiveColumn().second);
}

bool Plugin::loadProgram(const QString & filename, const QByteArray & source, Shared::ProgramFormat format)
{
    bool ok = false;
    if (format==Shared::FormatBinary) {
        std::list<char> buffer;
        for (int i=0; i<source.size(); i++)
            buffer.push_back(source[i]);
        ok = pRun_->loadProgramFromBinaryBuffer(buffer, filename.toStdWString());
    }
    else {
        const std::string str(source.constData());
        pRun_->loadProgramFromTextBuffer(str, filename.toStdWString());
    }
    const QString programDirName = QFileInfo(filename).absoluteDir().absolutePath();
    pRun_->setProgramDirectory(programDirName);
    pRun_->programLoaded = ok;
    return ok;
}

QString Plugin::error() const
{
    return pRun_->error();
}

QMap<QString,QVariant> Plugin::getScalarLocalValues(int frameNo) const
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

QVariant Plugin::valueStackTopItem() const
{
    return pRun_->valueStackTopItem();
}

QMap<QString,QVariant> Plugin::getScalarGlobalValues(const QString & moduleName) const
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

QVariantList Plugin::getLocalTableValues(
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

QVariant Plugin::getLocalTableValue(
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

QVariantList Plugin::getGlobalTableValues(
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

QVariant Plugin::getGlobalTableValue(
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



void Plugin::runContinuous()
{
    if (done_) {
        pRun_->setEntryPointToMain();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runContinuous();
}

void Plugin::runBlind()
{
    if (done_) {
        pRun_->setEntryPointToMain();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runBlind();
}

void Plugin::runStepInto()
{
    pRun_->runStepIn();
}

void Plugin::runStepOut()
{
    pRun_->runStepOut();
}

void Plugin::runStepOver()
{
    if (done_) {
        pRun_->setEntryPointToMain();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runStepOver();
}

void Plugin::runTesting()
{
    if (done_) {
        pRun_->setEntryPointToTest();
        pRun_->reset();
        done_ = false;
    }
    pRun_->runBlind();
}

bool Plugin::isTestingRun() const
{
    return pRun_->isTestingRun();
}

void Plugin::terminate()
{
    pRun_->stop();
}


void Plugin::handleThreadFinished()
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

void Plugin::handleLineChanged(int lineNo, quint32 colStart, quint32 colEnd)
{
    emit lineChanged(lineNo, colStart, colEnd);
}


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
};

Plugin::~Plugin()
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


void Plugin::prepareCommonRun()
{
    common_ = new CommonFunctors;
    common_->reset.setCallFunctor(&common_->call);
    pRun_->vm->setFunctor(&common_->reset);
    pRun_->vm->setFunctor(&common_->call);
    pRun_->vm->setFunctor(&common_->toString);
    pRun_->vm->setFunctor(&common_->fromString);
}

void Plugin::prepareConsoleRun()
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

void Plugin::prepareGuiRun()
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
    pRun_->vm->setConsoleInputBuffer(&gui_->input);
    pRun_->vm->setConsoleOutputBuffer(&gui_->output);
}


QString Plugin::initialize(const QStringList &)
{
    pRun_->programLoaded = false;

    qRegisterMetaType<QVariant::Type>("QVariant::Type");
    qRegisterMetaType< QList<QVariant::Type> >("QList<QVariant::Type>");
    qRegisterMetaType<Shared::RunInterface::StopReason>("Shared::RunInterface::StopReason");

    if (ExtensionSystem::PluginManager::instance()->startupModule()==this) {

        prepareConsoleRun();

#ifndef Q_OS_WIN32
        setlocale(LC_CTYPE, "ru_RU.UTF-8");
#else
        setlocale(LC_CTYPE, ".1251");
#endif
        onlyOneTryToInput_ = qApp->arguments().contains("-p");
        QString fileName;
        QStringList programArguments;
        for (int i=1; i<qApp->arguments().size(); i++) {
            const QString arg = qApp->arguments()[i];
            if (fileName.isEmpty()) {
                if (!arg.startsWith("-")) {
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
                    loadProgram(fileName, data, fileName.endsWith(".ks")? Shared::FormatText : Shared::FormatBinary);
                }
                catch (const std::wstring & message) {
                    return QString::fromStdWString(message);
                }
            }
        }        
        if (qApp->arguments().contains("-t") || qApp->arguments().contains("--teacher"))
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

void Plugin::timerEvent(QTimerEvent *event) {
    killTimer(event->timerId());
    event->accept();
    pRun_->reset();
    QList<KPlugin*> actors = ExtensionSystem::PluginManager::instance()
            ->loadedPlugins("Actor*");
    foreach (KPlugin * plugin, actors) {
        Shared::ActorInterface * actor =
                qobject_cast<Shared::ActorInterface*>(plugin);
        if (actor)
            actor->setAnimationEnabled(true);
        if (actor && actor->mainWidget()) {
            actor->mainWidget()->show();
            connect(pRun_, SIGNAL(finished()), actor->mainWidget(), SLOT(close()));
        }
    }
    pRun_->start();
}

void Plugin::start()
{
    if (pRun_->programLoaded) {
        if (!ExtensionSystem::PluginManager::instance()->isGuiRequired()) {
            pRun_->reset();
            pRun_->start();
            pRun_->wait();
        }
        else {
            startTimer(0); // start thread after event loop started
        }
    }
    else {
        std::cerr << "Usage:\n\t" << qApp->arguments().at(0).toLocal8Bit().data() << " PROGRAM.kod|PROGRAM.ks [PROGRAM_ARGUMENTS]\n\n";
        std::cerr << "\tPROGRAM.kod or PROGRAM.ks  Kumir portable bytecode file name\n";
        std::cerr << "\tPROGRAM_ARGUMENTS          Optional parameters passed to kumir program\n";
        qApp->setProperty("returnCode", 127);
    }
}

bool Plugin::hasMoreInstructions() const
{
    return pRun_->hasMoreInstructions();
}

bool Plugin::canStepOut() const
{
    return pRun_->canStepOut();
}

bool Plugin::hasTestingEntryPoint() const
{
    return pRun_->vm->hasTestingAlgorithm();
}

} // namespace KumirCodeRun

Q_EXPORT_PLUGIN(KumirCodeRun::Plugin)
