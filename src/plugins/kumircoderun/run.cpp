#include "run.h"

namespace KumirCodeRun {

Run::Run(QObject *parent) :
    QThread(parent)
{
    programLoaded = false;
    vm = new VM(this);
    i_originFunctionDeep = 0;
    b_interactDone = b_stopping = b_stepDone = false;
    mutex_stopping = new QMutex;
    mutex_stepDone = new QMutex;
    mutex_interactDone = new QMutex;
    e_runMode = RM_ToEnd;

    connect(vm, SIGNAL(lineNoChanged(int)), this, SLOT(handleLineChanged(int)), Qt::DirectConnection);

    connect(vm, SIGNAL(inputRequest(QString,QList<quintptr>,QList<int>)),
            this, SLOT(handleInputRequest(QString,QList<quintptr>,QList<int>)), Qt::DirectConnection);
    connect(vm, SIGNAL(inputArgumentRequest(int,QString,QString,QList<int>)),
            this, SLOT(handleInputArgumentRequest(int,QString,QString,QList<int>)), Qt::DirectConnection);
    connect(vm, SIGNAL(outputRequest(QString)), this, SLOT(handleOutputRequest(QString)));
    connect(vm, SIGNAL(invokeExternalFunction(QString,QString,QVariantList,QList<quintptr>,QList<int>)),
            this, SLOT(handleExternalRequest(QString,QString,QVariantList,QList<quintptr>,QList<int>)),
            Qt::DirectConnection);
    connect(vm, SIGNAL(resetModuleRequest(QString)), this, SIGNAL(resetModule(QString)));

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
    b_stopping = false;
//    i_originFunctionDeep = vm->deep();
    e_runMode = RM_StepOver;
    vm->setNextCallStepOver();
    start();
}

void Run::runStepIn()
{
    b_stepDone = false;
//    i_originFunctionDeep = vm->deep();
    e_runMode = RM_StepIn;
    vm->setNextCallInto();
    start();
}

void Run::runStepOut()
{
    b_stepDone = false;
//    i_originFunctionDeep = vm->deep();
    emit lineChanged(-1);
    e_runMode = RM_StepOut;
    vm->setNextCallOut();
    start();
}

void Run::runBlind()
{
    e_runMode = RM_ToEnd;
    vm->setBlindMode(true);
    vm->setNextCallToEnd();
    start();
}

void Run::runContinuous()
{
    e_runMode = RM_ToEnd;
    b_stopping = false;
    vm->setNextCallToEnd();
    start();
}


void Run::handleInputRequest(const QString & format, const QList<quintptr> & references, const QList<int> & indeces)
{
    mutex_interactDone->lock();
    b_interactDone = false;
    list_inputResult.clear();
    mutex_interactDone->unlock();
    QVariantList result;
    emit input(format);
    forever {
        mutex_interactDone->lock();
        result = list_inputResult;
        mutex_interactDone->unlock();
        if (result.isEmpty()) {
            msleep(50);
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
    vm->setResults("", references, indeces, result);
}

void Run::handleInputArgumentRequest(int localId,
                                     const QString &varName,
                                     const QString &baseFormat,
                                     const QList<int> &bounds)
{
    QVariantList result;
    QVariantList localResult;
    int totalItems = 1;
    int currentIndex = 0;
    int dimension = 0;
    bool inputNext = true;
    int z = 0;
    int y = 0;
    int x = 0;
    int size0 = 0;
    int size1 = 0;
    int size2 = 0;
    if (bounds.size()==2) {
        dimension = 1;
        size0 = bounds[1]-bounds[0]+1;
        x = bounds[0];
        totalItems = size0;
    }
    else if (bounds.size()==4) {
        dimension = 2;
        size0 = bounds[3]-bounds[2]+1;
        size1 = bounds[1]-bounds[0]+1;
        x = bounds[0];
        y = bounds[2];
        totalItems = size0 * size1;
    }
    else if (bounds.size()==6) {
        dimension = 3;
        size0 = bounds[5]-bounds[4]+1;
        size1 = bounds[3]-bounds[2]+1;
        size2 = bounds[1]-bounds[0]+1;
        x = bounds[0];
        y = bounds[2];
        z = bounds[4];
        totalItems = size0 * size1 * size2;
    }
    forever {
        if (inputNext) {
            inputNext = false;
            mutex_interactDone->lock();
            b_interactDone = false;
            list_inputResult.clear();
            mutex_interactDone->unlock();
            QString varNameAndIndeces = varName;
            if (dimension>0) {
                QStringList indeces;
                indeces.prepend(QString::number(x+currentIndex%size0));
                if (dimension>=2)
                    indeces.prepend(QString::number(y+currentIndex%(size0*size1)));
                if (dimension==3)
                    indeces.prepend(QString::number(z+currentIndex%(size0*size1*size2)));
                varNameAndIndeces += "["+indeces.join(",")+"]";
            }
            QString greeting = tr("Please enter %1: ").arg(varNameAndIndeces);
            emit output(greeting);
            emit input(baseFormat);
        }
        mutex_interactDone->lock();
        localResult = list_inputResult;
        mutex_interactDone->unlock();
        if (localResult.isEmpty()) {
            msleep(50);
        }
        else {
            result << localResult.at(0);
            currentIndex++;
            inputNext = true;
            if (totalItems==currentIndex) {
                break;
            }
        }
        if (mustStop())
            break;
    }
    if (mustStop())
        return;
    if (dimension==0) {
        vm->setLocalVariableValue(localId, result.first());
    }
    else {
        vm->setLocalVariableValue(localId, QVariant(result));
    }
}

void Run::handleExternalRequest(const QString &pluginName,
                                const QString &functionName,
                                const QVariantList &arguments,
                                const QList<quintptr> &references,
                                const QList<int> &indeces)
{
    mutex_interactDone->lock();
    b_interactDone = false;
    list_funcResults.clear();
    v_funcResult = QVariant::Invalid;
    s_funcError = "";
    mutex_interactDone->unlock();
    QVariantList result;
    emit externalFunctionCall(pluginName, functionName, arguments);
    bool done = false;
    forever {
        mutex_interactDone->lock();
        done = b_interactDone;
        mutex_interactDone->unlock();
        if (!done) {
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
    vm->pushValueToStack(v_funcResult);
    vm->setResults(s_funcError, references, indeces, list_funcResults);
}

void Run::finishInput(const QVariantList &data)
{
    QMutexLocker l(mutex_interactDone);
    b_interactDone = true;
    list_inputResult = data;
}

void Run::finishExternalFunctionCall(const QString & error,
                                     const QVariant &retval,
                                     const QVariantList &results)
{
    QMutexLocker l(mutex_interactDone);
    b_interactDone = true;
    list_funcResults = results;
    v_funcResult = retval;
    s_funcError = error;
}

void Run::handleOutputRequest(const QString & out )
{
    emit output(out);
}

bool Run::mustStop()
{
    QMutexLocker l1(mutex_stopping);
    QMutexLocker l2(mutex_stepDone);


    if (b_stopping)
        return true;

    if (e_runMode!=RM_ToEnd) {
        return b_stepDone;
    }
    else {
        return false;
    }
}


void Run::handleLineChanged(int lineNo)
{
    mutex_stepDone->lock();
    b_stepDone = true;
    mutex_stepDone->unlock();
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
