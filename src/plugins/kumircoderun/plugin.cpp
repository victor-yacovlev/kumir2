#include "plugin.h"
#include "run.h"
#include "extensionsystem/pluginmanager.h"
#include <iostream>
#include <sstream>
#include <locale.h>


namespace KumirCodeRun {

Plugin::Plugin() :
    ExtensionSystem::KPlugin()
{
    d = new Run(this);

    connect (this, SIGNAL(finishInput(QVariantList)), d, SIGNAL(finishInput(QVariantList)));

    b_done = true;
    connect (d, SIGNAL(output(QString)), this, SIGNAL(outputRequest(QString)));
    connect (d, SIGNAL(input(QString)), this, SIGNAL(inputRequest(QString)));
    connect (d, SIGNAL(finished()), this, SLOT(handleThreadFinished()));
    connect (d, SIGNAL(lineChanged(int)), this, SIGNAL(lineChanged(int)));
    connect (d, SIGNAL(marginText(int,QString)), this, SIGNAL(marginText(int,QString)));
    connect (d, SIGNAL(clearMarginRequest(int,int)), this, SIGNAL(clearMargin(int,int)));
    b_onlyOneTryToInput = false;


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

Plugin::~Plugin()
{
    if (d->isRunning()) {
        d->stop();
        d->wait();
    }
    delete d;
}

int Plugin::currentLineNo() const
{
    return d->effectiveLineNo();
}

bool Plugin::loadProgram(const QString & filename, const QByteArray & source, Shared::ProgramFormat format)
{
    if (format==Shared::FormatBinary) {
        std::list<char> buffer;
        for (int i=0; i<source.size(); i++)
            buffer.push_back(source[i]);
        d->loadProgramFromBinaryBuffer(buffer, filename.toStdWString());
    }
    else {
        const std::string str(source.constData());
        d->loadProgramFromTextBuffer(str, filename.toStdWString());
    }
    const QString programDirName = QFileInfo(filename).absoluteDir().absolutePath();
    d->setProgramDirectory(programDirName);
    d->programLoaded = true;
    return true;
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
    if (b_done) {
        d->setEntryPointToMain();
        d->reset();
        b_done = false;
    }
    d->runContinuous();
}

void Plugin::runBlind()
{
    if (b_done) {
        d->setEntryPointToMain();
        d->reset();
        b_done = false;
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
    if (b_done) {
        d->setEntryPointToMain();
        d->reset();
        b_done = false;
    }
    d->runStepOver();
}

void Plugin::runTesting()
{
    if (b_done) {
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
        b_done = true;
    }
    else if (d->hasMoreInstructions() && d->stopped()) {
        emit stopped(Shared::RunInterface::SR_UserTerminated);
        b_done = true;
    }
    else if (d->hasMoreInstructions()) {
        emit stopped(Shared::RunInterface::SR_UserInteraction);
    }
    else {
        emit stopped(Shared::RunInterface::SR_Done);
        b_done = true;
    }
}

void Plugin::handleLineChanged(int lineNo)
{
    emit lineChanged(lineNo);
}

QString Plugin::initialize(const QStringList &)
{
    d->programLoaded = false;

    qRegisterMetaType<QVariant::Type>("QVariant::Type");
    qRegisterMetaType< QList<QVariant::Type> >("QList<QVariant::Type>");
    qRegisterMetaType<Shared::RunInterface::StopReason>("Shared::RunInterface::StopReason");

    if (ExtensionSystem::PluginManager::instance()->startupModule()==this) {
#ifndef Q_OS_WIN32
        setlocale(LC_CTYPE, "ru_RU.UTF-8");
#else
        setlocale(LC_CTYPE, ".1251");
#endif
        b_onlyOneTryToInput = qApp->arguments().contains("-p");
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
                loadProgram(fileName, data, fileName.endsWith(".ks")? Shared::FormatText : Shared::FormatBinary);
            }
        }
    }

    return "";
}

void Plugin::start()
{
    if (d->programLoaded) {
        d->reset();
        QList<KPlugin*> plugins = loadedPlugins();
        foreach (KPlugin * p, plugins) {
//            ActorInterface * a = qobject_cast<ActorInterface*>(p);
//            if (a) {
//                a->reset();
//            }
        }

        while (d->hasMoreInstructions()) {
            d->evaluateNextInstruction();
            if (!error().isEmpty())
                break;
        }
        if (error().isEmpty()) {
            qApp->setProperty("returnCode", 0);
        }
        else {
            QString message;
            if (d->effectiveLineNo()==-1) {
                message = tr("RUNTIME ERROR: ");
            }
            else {
                message = tr("RUNTIME ERROR AT LINE ");
                message += QString::number(d->effectiveLineNo()+1)+": ";
            }
            message += error()+"\n";
            wchar_t * buffer = (wchar_t*)calloc(message.length()+1, sizeof(wchar_t));
            message.toWCharArray(buffer);
            buffer[message.length()] = L'\0';
            fwprintf(stderr, L"%ls", buffer);
            free(buffer);
            qApp->setProperty("returnCode", 1);
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
