#include "plugin.h"
#include "variant.h"
#include "run.h"
#include "extensionsystem/pluginmanager.h"
#include "stdlib/genericinputoutput.h"
#include <iostream>
#include <locale.h>

namespace KumirCodeRun {

Plugin::Plugin() :
    ExtensionSystem::KPlugin()
{
    d = new Run(this);
    b_done = true;
    connect (d, SIGNAL(output(QString)), this, SLOT(handleOutput(QString)));
    connect (d, SIGNAL(input(QString)), this, SLOT(handleInput(QString)));
    connect (d, SIGNAL(externalFunctionCall(QString,QString,QVariantList)),
             this, SIGNAL(externalRequest(QString,QString,QVariantList)));
    connect (d, SIGNAL(finished()), this, SLOT(handleThreadFinished()));
    connect (d, SIGNAL(lineChanged(int)), this, SLOT(handleLineChanged(int)));
    connect (d->vm, SIGNAL(valueChangeNotice(int,QString)),
             this, SIGNAL(marginText(int,QString)));
    connect (d->vm, SIGNAL(clearMargin(int,int)),
             this, SIGNAL(clearMargin(int,int)));
    connect (d->vm, SIGNAL(resetModuleRequest(QString)), this, SIGNAL(resetModule(QString)));
}

bool Plugin::isGuiRequired() const
{
    QStringList actors = d->vm->usedActors();
    const QList<const KPlugin *> modules = loadedPlugins();
    for (int i=0; i<actors.size(); i++) {
        const QString actorName = actors[i];
        for (int j=0; j<modules.size(); j++) {
            if (modules[j]->pluginSpec().name==actorName) {
                if (modules[j]->isGuiRequired())
                    return true;
            }
        }
    }
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
    return d->vm->currentLineNo();
}

bool Plugin::loadProgram(QIODevice *source, Shared::ProgramFormat format)
{
    Bytecode::Data data;
    if (format==Shared::FormatBinary) {
        QDataStream ds(source);
        ds >> data;
    }
    else {
        QTextStream ts(source);
        ts >> data;
    }
    d->vm->loadProgram(data);
    dt_loadedVersion = data.lastModified;
    d->programLoaded = true;
    return true;
}

QString Plugin::error() const
{
    return d->vm->error();
}

QVariantList Plugin::remainingValues() const
{
    return d->vm->remainingValues();
}

void Plugin::runContinuous()
{
    if (b_done) {
        d->vm->setEntryPoint(VM::EP_Main);
        d->vm->reset();
        b_done = false;
    }
    d->runContinuous();
}

void Plugin::runBlind()
{
    if (b_done) {
        d->vm->setEntryPoint(VM::EP_Main);
        d->vm->reset();
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
        d->vm->setEntryPoint(VM::EP_Main);
        d->vm->reset();
        b_done = false;
    }
    d->runStepOver();
}

void Plugin::runTesting()
{
    if (b_done) {
        d->vm->setEntryPoint(VM::EP_Testing);
        d->vm->reset();
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
    if (!d->vm->error().isEmpty()) {
        emit stopped(Shared::RunInterface::Error);
        b_done = true;
    }
    else if (d->vm->hasMoreInstructions() && d->stopped()) {
        emit stopped(Shared::RunInterface::UserTerminated);
        b_done = true;
    }
    else if (d->vm->hasMoreInstructions()) {
        emit stopped(Shared::RunInterface::UserInteraction);
    }
    else {
        emit stopped(Shared::RunInterface::Done);
        b_done = true;
    }
}

void Plugin::handleLineChanged(int lineNo)
{
    emit lineChanged(lineNo);
}

void Plugin::handleInput(const QString &format)
{
    if (ExtensionSystem::PluginManager::instance()->startupModule()==this) {
        QList<QVariant> result;
        StdLib::GenericInputOutput * inp = StdLib::GenericInputOutput::instance();
        inp->doInput(format);
        wchar_t buffer[4096];
        wchar_t err[256];
        QString error;
        bool ok;
        do {
            wscanf(L"%ls", &buffer);
            QSet<QPair<int,int> > errpos;
            ok = inp->tryFinishInput(QString::fromWCharArray(buffer), result, errpos, false, error);
            if (!ok) {
                err[error.toWCharArray(err)] = L'\0';
                fwprintf(stderr, L"%ls\n", err);
            }
        } while (!ok);
        d->finishInput(result);
    }
    else {
        emit stopped(Shared::RunInterface::UserInteraction);
        emit inputRequest(format);
    }
}

QString Plugin::initialize(const QStringList &)
{
    d->programLoaded = false;
    qRegisterMetaType<Variant>("KumirCodeRun::Variant");
    qRegisterMetaType<VariantList>("KumirCodeRun::VariantList");
    qRegisterMetaType<Shared::RunInterface::StopReason>("Shared::RunInterface::StopReason");

    const QList<KPlugin*> plugins = loadedPlugins();
    QList<ActorInterface*> availableActors;
    foreach (KPlugin * plugin , plugins) {
        ActorInterface * a = qobject_cast<ActorInterface*>(plugin);
        if (a) {
            availableActors << a;
        }
    }

    d->vm->setAvailableActors(availableActors);

    if (ExtensionSystem::PluginManager::instance()->startupModule()==this) {
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
            if (f.open(QIODevice::ReadOnly))
                loadProgram(&f, fileName.endsWith(".ks")? Shared::FormatText : Shared::FormatBinary);
        }
    }

    return "";
}

void Plugin::start()
{
    if (d->programLoaded) {
        d->vm->reset();
        QList<KPlugin*> plugins = loadedPlugins();
        foreach (KPlugin * p, plugins) {
            ActorInterface * a = qobject_cast<ActorInterface*>(p);
            if (a) {
                a->reset();
            }
        }

        while (d->vm->hasMoreInstructions()) {
            d->vm->evaluateNextInstruction();
            if (!error().isEmpty())
                break;
        }
        if (error().isEmpty()) {
            qApp->setProperty("returnCode", 0);
        }
        else {
            QString message;
            if (d->vm->currentLineNo()==-1) {
                message = tr("RUNTIME ERROR: ");
            }
            else {
                message = tr("RUNTIME ERROR AT LINE ");
                message += QString::number(d->vm->currentLineNo()+1)+": ";
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
    return d->vm->hasMoreInstructions();
}

bool Plugin::canStepOut() const
{
    return d->vm->topStackType()==EL_FUNCTION;
}





QVariant Plugin::value(int moduleId, int algorhitmId, int variableId) const
{
    return d->vm->value(moduleId, algorhitmId, variableId);
}

QList<int> Plugin::bounds(int moduleId, int algorhitmId, int variableId) const
{
    return d->vm->bounds(moduleId, algorhitmId, variableId);
}

QList<int> Plugin::reference(int moduleId, int algorhitmId, int variableId) const
{
    return d->vm->reference(moduleId, algorhitmId, variableId);
}

} // namespace KumirCodeRun

Q_EXPORT_PLUGIN(KumirCodeRun::Plugin)
