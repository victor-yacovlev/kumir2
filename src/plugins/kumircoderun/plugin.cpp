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
    , d(new Run(this))
    , common_(nullptr)
    , console_(nullptr)
    , gui_(nullptr)
{
    connect (this, SIGNAL(finishInput(QVariantList)), d, SIGNAL(finishInput(QVariantList)));

    done_ = true;
    connect (d, SIGNAL(output(QString)), this, SIGNAL(outputRequest(QString)));
    connect (d, SIGNAL(input(QString)), this, SIGNAL(inputRequest(QString)));
    connect (d, SIGNAL(finished()), this, SLOT(handleThreadFinished()));
    connect (d, SIGNAL(lineChanged(int)), this, SIGNAL(lineChanged(int)));
    connect (d, SIGNAL(marginText(int,QString)), this, SIGNAL(marginText(int,QString)));
    connect (d, SIGNAL(clearMarginRequest(int,int)), this, SIGNAL(clearMargin(int,int)));
    onlyOneTryToInput_ = false;


    connect(d, SIGNAL(signal_debuggerReset()), this, SIGNAL(debuggerReset()), Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerPopContext()), this, SIGNAL(debuggerPopContext()), Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerPushContext(QString,QStringList,QStringList,QList<int>)),
            this, SIGNAL(debuggerPushContext(QString,QStringList,QStringList,QList<int>)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerSetGlobals(QString,QStringList,QStringList,QList<int>)),
            this, SIGNAL(debuggerSetGlobals(QString,QStringList,QStringList,QList<int>)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerUpdateLocalVariable(QString,QString)),
            this, SIGNAL(debuggerUpdateLocalVariable(QString,QString)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerUpdateGlobalVariable(QString,QString,QString)),
            this, SIGNAL(debuggerUpdateGlobalVariable(QString,QString,QString)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerUpdateLocalTableBounds(QString,QList<int>)),
            this, SIGNAL(debuggerUpdateLocalTableBounds(QString,QList<int>)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerUpdateGlobalTableBounds(QString,QString,QList<int>)),
            this, SIGNAL(debuggerUpdateGlobalTableBounds(QString,QString,QList<int>)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerSetLocalReference(QString,QString,QList<int>,int,QString)),
            this, SIGNAL(debuggerSetLocalReference(QString,QString,QList<int>,int,QString)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerForceUpdateValues()),
            this, SIGNAL(debuggerForceUpdateValues()),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerUpdateLocalTableValue(QString,QList<int>)),
            this, SIGNAL(debuggerUpdateLocalTableValue(QString,QList<int>)),
            Qt::DirectConnection);
    connect(d, SIGNAL(signal_debuggerUpdateGlobalTableValue(QString,QString,QList<int>)),
            this, SIGNAL(debuggerUpdateGlobalTableValue(QString,QString,QList<int>)),
            Qt::DirectConnection);
}

int Plugin::currentLineNo() const
{
    return d->effectiveLineNo();
}

bool Plugin::loadProgram(const QString & filename, const QByteArray & source, Shared::ProgramFormat format)
{
    bool ok = false;
    if (format==Shared::FormatBinary) {
        std::list<char> buffer;
        for (int i=0; i<source.size(); i++)
            buffer.push_back(source[i]);
        ok = d->loadProgramFromBinaryBuffer(buffer, filename.toStdWString());
    }
    else {
        const std::string str(source.constData());
        d->loadProgramFromTextBuffer(str, filename.toStdWString());
    }
    const QString programDirName = QFileInfo(filename).absoluteDir().absolutePath();
    d->setProgramDirectory(programDirName);
    d->programLoaded = ok;
    return ok;
}

QString Plugin::error() const
{
    return d->error();
}

QMap<QString,QVariant> Plugin::getScalarLocalValues(int frameNo) const
{
    d->lockVMMutex();
    QMap<QString,QVariant> result;
    const std::vector<Variable> & locals = d->vm->getLocals(frameNo);
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
    d->unlockVMMutex();
    return result;
}

QMap<QString,QVariant> Plugin::getScalarGlobalValues(const QString & moduleName) const
{
    d->lockVMMutex();
    QMap<QString,QVariant> result;
    const std::vector<Variable> & locals = d->vm->getGlobals(moduleName.toStdWString());
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
    d->unlockVMMutex();
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
    d->lockVMMutex();
    const std::vector<Variable> & locals = d->vm->getLocals(frameNo);
    for (int i=0; i<locals.size(); i++) {
        const Variable & var = locals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            complete = true;
            result = getTableValues(var, var.dimension(), ranges, counter, maxCount, complete);
            break;
        }
    }
    d->unlockVMMutex();
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
    d->lockVMMutex();
    const std::vector<Variable> & locals = d->vm->getLocals(frameNo);
    for (int i=0; i<locals.size(); i++) {
        const Variable & var = locals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            if (var.hasValue(kIndeces))
                result = QString::fromStdWString(var.toString(kIndeces));
            break;
        }
    }
    d->unlockVMMutex();
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
    d->lockVMMutex();
    const std::vector<Variable> & globals = d->vm->getGlobals(moduleName.toStdWString());
    for (int i=0; i<globals.size(); i++) {
        const Variable & var = globals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            complete = true;
            result = getTableValues(var, var.dimension(), ranges, counter, maxCount, complete);
            break;
        }
    }
    d->unlockVMMutex();
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
    d->lockVMMutex();
    const std::vector<Variable> & globals = d->vm->getGlobals(moduleName.toStdWString());
    for (int i=0; i<globals.size(); i++) {
        const Variable & var = globals.at(i);
        if (var.dimension()>0 && var.myName()==name.toStdWString()) {
            if (var.hasValue(kIndeces))
                result = QString::fromStdWString(var.toString(kIndeces));
            break;
        }
    }
    d->unlockVMMutex();
    return result;
}



void Plugin::runContinuous()
{
    if (done_) {
        d->setEntryPointToMain();
        d->reset();
        done_ = false;
    }
    d->runContinuous();
}

void Plugin::runBlind()
{
    if (done_) {
        d->setEntryPointToMain();
        d->reset();
        done_ = false;
    }
    d->runBlind();
}

void Plugin::runStepInto()
{
    d->runStepIn();
}

void Plugin::runStepOut()
{
    d->runStepOut();
}

void Plugin::runStepOver()
{
    if (done_) {
        d->setEntryPointToMain();
        d->reset();
        done_ = false;
    }
    d->runStepOver();
}

void Plugin::runTesting()
{
    if (done_) {
        d->setEntryPointToTest();
        d->reset();
    }
    d->runBlind();
}

void Plugin::terminate()
{
    d->stop();
}


void Plugin::handleThreadFinished()
{
    if (d->error().length()>0) {
        emit stopped(Shared::RunInterface::SR_Error);
        done_ = true;
    }
    else if (d->hasMoreInstructions() && d->stopped()) {
        emit stopped(Shared::RunInterface::SR_UserTerminated);
        done_ = true;
    }
    else if (d->hasMoreInstructions()) {
        emit stopped(Shared::RunInterface::SR_UserInteraction);
    }
    else {
        emit stopped(Shared::RunInterface::SR_Done);
        done_ = true;
    }
}

void Plugin::handleLineChanged(int lineNo)
{
    emit lineChanged(lineNo);
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
};

Plugin::~Plugin()
{
    if (d->isRunning()) {
        d->stop();
        d->wait();
    }
    delete d;
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
    d->vm->setFunctor(&common_->reset);
    d->vm->setFunctor(&common_->call);
    d->vm->setFunctor(&common_->toString);
    d->vm->setFunctor(&common_->fromString);
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

    d->vm->setFunctor(&console_->load);    
    d->vm->setFunctor(&console_->input);
    d->vm->setFunctor(&console_->output);
    d->vm->setFunctor(&console_->getMainArgument);
    d->vm->setFunctor(&console_->returnMainValue);

}

void Plugin::prepareGuiRun()
{
    if (! common_)
        prepareCommonRun();

    gui_ = new GuiFunctors;

    gui_->input.setRunnerInstance(d);
    gui_->output.setRunnerInstance(d);
    gui_->getMainArgument.setRunnerInstance(d);
    gui_->returnMainValue.setRunnerInstance(d);

    gui_->input.setCustomTypeFromStringFunctor(&common_->fromString);
    gui_->output.setCustomTypeToStringFunctor(&common_->toString);
    gui_->getMainArgument.setCustomTypeFromStringFunctor(&common_->fromString);
    gui_->returnMainValue.setCustomTypeToStringFunctor(&common_->toString);

    d->vm->setFunctor(&gui_->load);
    d->vm->setFunctor(&gui_->input);
    d->vm->setFunctor(&gui_->output);
    d->vm->setFunctor(&gui_->getMainArgument);
    d->vm->setFunctor(&gui_->returnMainValue);


}


QString Plugin::initialize(const QStringList &)
{
    d->programLoaded = false;

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
    }
    else {
        prepareGuiRun();
    }

    return "";
}

void Plugin::timerEvent(QTimerEvent *event) {
    killTimer(event->timerId());
    event->accept();
    d->reset();
    QList<KPlugin*> actors = ExtensionSystem::PluginManager::instance()
            ->loadedPlugins("Actor*");
    foreach (KPlugin * plugin, actors) {
        Shared::ActorInterface * actor =
                qobject_cast<Shared::ActorInterface*>(plugin);
        if (actor)
            actor->setAnimationEnabled(true);
        if (actor && actor->mainWidget()) {
            actor->mainWidget()->show();
            connect(d, SIGNAL(finished()), actor->mainWidget(), SLOT(close()));
        }
    }
    d->start();
}

void Plugin::start()
{
    if (d->programLoaded) {
        if (!ExtensionSystem::PluginManager::instance()->isGuiRequired()) {
            d->reset();
            d->start();
            d->wait();
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
    return d->hasMoreInstructions();
}

bool Plugin::canStepOut() const
{
    return d->canStepOut();
}


} // namespace KumirCodeRun

Q_EXPORT_PLUGIN(KumirCodeRun::Plugin)
