#include "plugin.h"
#include "variant.h"
#include "run.h"

namespace KumirCodeRun {

Plugin::Plugin() :
    ExtensionSystem::KPlugin()
{
    d = new Run(this);
    b_done = false;
    connect (d, SIGNAL(output(QString)), this, SLOT(handleOutput(QString)));
    connect (d, SIGNAL(input(QString)), this, SLOT(handleInput(QString)));
    connect (d, SIGNAL(finished()), this, SLOT(handleThreadFinished()));
    connect (d, SIGNAL(lineChanged(int)), this, SLOT(handleLineChanged(int)));
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
    return d->lineNo();
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
    return true;
}

QString Plugin::error() const
{
    return d->vm->error();
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
    d->runContinuous();
}

void Plugin::terminate()
{
    d->stop();
}

void Plugin::finishInput(const QVariantList &message)
{
    d->finishInput(message);
}

void Plugin::handleOutput(const QString &text)
{
    emit outputRequest(text);
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
    emit stopped(Shared::RunInterface::UserInteraction);
    emit inputRequest(format);
}

QString Plugin::initialize(const QStringList &)
{
    qRegisterMetaType<Variant>("KumirCodeRun::Variant");
    qRegisterMetaType<VariantList>("KumirCodeRun::VariantList");
    qRegisterMetaType<Shared::RunInterface::StopReason>("Shared::RunInterface::StopReason");
    return "";
}

bool Plugin::hasMoreInstructions() const
{
    return d->vm->canStepInto();
}

bool Plugin::canStepOut() const
{
    return d->vm->topStackType()==EL_FUNCTION;
}

bool Plugin::canStepInto() const
{
    return false;
}

} // namespace KumirCodeRun


Q_EXPORT_PLUGIN(KumirCodeRun::Plugin)
