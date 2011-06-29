#include "run.h"

namespace KumirCodeRun {

Run::Run(QObject *parent) :
    QThread(parent)
{
    vm = new VM(this);
    i_functionDeep = i_originFunctionDeep = 0;
    b_inputDone = b_stopping = b_stepDone = false;
    mutex_stopping = new QMutex;
    mutex_stepDone = new QMutex;
    mutex_functionDeep = new QMutex;
    mutex_inputDone = new QMutex;
    e_runMode = RM_ToEnd;

    connect(vm, SIGNAL(functionEntered()), this, SLOT(handleDeepChanged()), Qt::DirectConnection);
    connect(vm, SIGNAL(functionLeaved()), this, SLOT(handleDeepChanged()), Qt::DirectConnection);
    connect(vm, SIGNAL(lineNoChanged(int)), this, SLOT(handleLineChanged(int)), Qt::DirectConnection);

    connect(vm, SIGNAL(inputRequest(QString,QList<quintptr>)),
            this, SLOT(handleInputRequest(QString,QList<quintptr>)), Qt::DirectConnection);
    connect(vm, SIGNAL(outputRequest(QString)), this, SLOT(handleOutputRequest(QString)));

}

void Run::stop()
{
    QMutexLocker l(mutex_stopping);
    b_stopping = true;
    if (!isRunning()) {
        emit lineChanged(-1);
        emit finished();
    }
}

void Run::runStepOver()
{
    b_stepDone = false;
    i_functionDeep = i_originFunctionDeep = vm->deep();
    e_runMode = RM_StepOver;
    start();
}

void Run::runStepIn()
{
    b_stepDone = false;
    i_functionDeep = i_originFunctionDeep = vm->deep();
    e_runMode = RM_StepIn;
    start();
}

void Run::runStepOut()
{
    b_stepDone = false;
    i_functionDeep = i_originFunctionDeep = vm->deep();
    e_runMode = RM_StepOut;
    start();
}

void Run::runContinuous()
{
    e_runMode = RM_ToEnd;
    start();
}


void Run::handleInputRequest(const QString & format, const QList<quintptr> & references)
{
    mutex_inputDone->lock();
    b_inputDone = false;
    list_inputResult.clear();
    mutex_inputDone->unlock();
    QVariantList result;
    emit input(format);
    forever {
        mutex_inputDone->lock();
        result = list_inputResult;
        mutex_inputDone->unlock();
        if (result.isEmpty()) {
            msleep(1);
        }
        else {
            break;
        }
        if (mustStop())
            break;
    }
    if (mustStop())
        return;
    Q_ASSERT(result.size()==references.size());
    vm->setResults(references, result);
}

void Run::handleOutputRequest(const QString & out )
{
    emit output(out);
}

bool Run::mustStop()
{
    QMutexLocker l1(mutex_stopping);
    QMutexLocker l2(mutex_stepDone);
    QMutexLocker l3(mutex_functionDeep);

    if (b_stopping)
        return true;

    if (e_runMode==RM_StepOver) {
        return b_stepDone && i_functionDeep <= i_originFunctionDeep;
    }
    else if (e_runMode==RM_StepIn) {
        return b_stepDone && i_functionDeep >= i_originFunctionDeep;
    }
    else if (e_runMode==RM_StepOut) {
        return b_stepDone && i_functionDeep < i_originFunctionDeep;
    }
    else {
        return false;
    }
}

void Run::handleDeepChanged()
{
    QMutexLocker l(mutex_functionDeep);
    i_functionDeep = vm->deep();
}

void Run::handleLineChanged(int lineNo)
{
    mutex_stepDone->lock();
    b_stepDone = true;
    mutex_stepDone->unlock();
    i_lineNo = lineNo;
    if (mustStop())
        emit lineChanged(lineNo);
    else
        emit lineChanged(-1);

}

void Run::run()
{
    while (vm->hasMoreInstructions()) {
        if (mustStop()) {
            break;
        }
        vm->evaluateNextInstruction();
        if (!vm->error().isEmpty()) {
            emit error(vm->error());
            break;
        }
    }
}

} // namespace KumirCodeRun
