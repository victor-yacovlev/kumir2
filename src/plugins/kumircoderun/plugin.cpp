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

    b_done = true;
    connect (d, SIGNAL(output(QString)), this, SIGNAL(outputRequest(QString)));
    connect (d, SIGNAL(input(QString)), this, SIGNAL(inputRequest(QString)));
    connect (d, SIGNAL(externalFunctionCall(QString,QString,QVariantList)),
             this, SIGNAL(externalRequest(QString,QString,QVariantList)));
    connect (d, SIGNAL(finished()), this, SLOT(handleThreadFinished()));
    connect (d, SIGNAL(lineChanged(int)), this, SIGNAL(lineChanged(int)));
//    connect (d->vm, SIGNAL(valueChangeNotice(int,QString)),
//             this, SIGNAL(marginText(int,QString)));
    connect (d, SIGNAL(marginText(int,QString)), this, SIGNAL(marginText(int,QString)));
//    connect (d->vm, SIGNAL(clearMargin(int,int)),
//             this, SIGNAL(clearMargin(int,int)));
    connect (d, SIGNAL(clearMarginRequest(int,int)), this, SIGNAL(clearMargin(int,int)));
//    connect (d->vm, SIGNAL(resetModuleRequest(QString)), this, SIGNAL(resetModule(QString)));
    b_onlyOneTryToInput = false;
}

bool Plugin::isGuiRequired() const
{
//    QStringList actors = d->vm->usedActors();
//    const QList<const KPlugin *> modules = loadedPlugins();
//    for (int i=0; i<actors.size(); i++) {
//        const QString actorName = actors[i];
//        for (int j=0; j<modules.size(); j++) {
//            if (modules[j]->pluginSpec().name==actorName) {
//                if (modules[j]->isGuiRequired())
//                    return true;
//            }
//        }
//    }
    return false;
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

bool Plugin::loadProgram(const QByteArray & source, Shared::ProgramFormat format)
{
    if (format==Shared::FormatBinary) {
        std::list<char> buffer;
        for (int i=0; i<source.size(); i++)
            buffer.push_back(source[i]);
        d->loadProgramFromBinaryBuffer(buffer);
    }
    else {
        const std::string str(source.constData());
        d->loadProgramFromTextBuffer(str);
    }
    d->programLoaded = true;
    return true;
}

QString Plugin::error() const
{
    return d->error();
}

QVariantList Plugin::remainingValues() const
{
    return d->remainingValues();
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

void Plugin::finishInput(const QVariantList &message)
{
    d->finishInput(message);
}

void Plugin::finishExternalFunctionCall(const QString & error,
                                        const QVariant &retval,
                                        const QVariantList &results)
{
    d->finishExternalFunctionCall(error, retval, results);
}

void Plugin::handleOutput(const QString &text)
{
    if (ExtensionSystem::PluginManager::instance()->startupModule()==this) {
        wchar_t * buffer = (wchar_t*)calloc(text.length()+1, sizeof(wchar_t));
        text.toWCharArray(buffer);
        buffer[text.length()] = L'\0';
        wprintf(L"%ls", buffer);
        free(buffer);
    }
    else {
        emit outputRequest(text);
    }
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

void Plugin::handleInput(const QString &format)
{
//    if (ExtensionSystem::PluginManager::instance()->startupModule()==this) {
//        QList<QVariant> result;
//        StdLib::GenericInputOutput * inp = StdLib::GenericInputOutput::instance();
//        inp->doInput(format);
//        wchar_t buffer[4096];
//        wchar_t err[256];
//        QString error;
//        bool ok;
//        do {
//            wscanf(L"%ls", &buffer);
//            QSet<QPair<int,int> > errpos;
//            ok = inp->tryFinishInput(QString::fromWCharArray(buffer), result, errpos, false, error);
//            if (!ok) {
//                if (!b_onlyOneTryToInput) {
//                    err[error.toWCharArray(err)] = L'\0';
//                    fwprintf(stderr, L"%ls\n", err);
//                }
//                else {
//                    d->vm->s_error = error.toStdWString();
//                    break;
//                }
//            }
//        } while (!ok);

//        d->finishInput(result);

//    }
//    else {
//        emit stopped(Shared::RunInterface::SR_UserInteraction);
//        emit inputRequest(format);
//    }
}

QString Plugin::initialize(const QStringList &)
{
    d->programLoaded = false;
//    d->setGuiMode(ExtensionSystem::PluginManager::instance()->startupModule()!=this);
//    qRegisterMetaType<Variant>("KumirCodeRun::Variant");
//    qRegisterMetaType<VariantList>("KumirCodeRun::VariantList");
    qRegisterMetaType<QVariant::Type>("QVariant::Type");
    qRegisterMetaType< QList<QVariant::Type> >("QList<QVariant::Type>");
    qRegisterMetaType<Shared::RunInterface::StopReason>("Shared::RunInterface::StopReason");

    const QList<KPlugin*> plugins = loadedPlugins();
//    QList<ActorInterface*> availableActors;
//    foreach (KPlugin * plugin , plugins) {
//        ActorInterface * a = qobject_cast<ActorInterface*>(plugin);
//        if (a) {
//            availableActors << a;
//        }
//    }

//    d->vm->setAvailableActors(availableActors);

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
                loadProgram(data, fileName.endsWith(".ks")? Shared::FormatText : Shared::FormatBinary);
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





QVariant Plugin::value(int moduleId, int algorhitmId, int variableId) const
{
    return d->value(moduleId, algorhitmId, variableId);
}

QList<int> Plugin::bounds(int moduleId, int algorhitmId, int variableId) const
{
    return d->bounds(moduleId, algorhitmId, variableId);
}

QList<int> Plugin::reference(int moduleId, int algorhitmId, int variableId) const
{
    return d->reference(moduleId, algorhitmId, variableId);
}

} // namespace KumirCodeRun

Q_EXPORT_PLUGIN(KumirCodeRun::Plugin)
