#include <QtCore> // include it before STL to avoid MSVC-specific errors
extern "C" {
#include <stdlib.h>
#include <string.h>
}
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm.hpp"
#include "run.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"
#include "util.h"

namespace KumirCodeRun {

using namespace VM;
using namespace Kumir;

Run::Run(QObject *parent) :
    QThread(parent)
{
    programLoaded = false;
    vm.reset(new KumirVM);
    VMMutex_.reset(new Mutex);
    vm->setMutex(VMMutex_);
    variablesModel_ = new KumVariablesModel(vm, VMMutex_, this);

    originFunctionDeep_ = 0;
    interactDoneFlag_ = stoppingFlag_ = stepDoneFlag_ = algDoneFlag_ = false;
    stoppingMutex_ = new QMutex;
    stepDoneMutex_ = new QMutex;
    algDoneMutex_ = new QMutex;
    interactDoneMutex_ = new QMutex;
    breakHitMutex_ = new QMutex;
    breakHitFlag_ = false;
    ignoreLineChangeFlag_ = false;
    runMode_ = RM_ToEnd;
    stdInBuffer_ = 0;
    supportBreakpoints_ = true;

    vm->setDebuggingHandler(this);

}

void Run::lockVMMutex()
{
    VMMutex_->lock();
}

void Run::unlockVMMutex()
{
    VMMutex_->unlock();
}

void Run::stop()
{
    QMutexLocker l(stoppingMutex_);
    stoppingFlag_ = true;
    if (!isRunning()) {
        emit lineChanged(-1, 0u, 0u);
        emit userTerminated();
    }

    using namespace Shared;
    using namespace ExtensionSystem;

    QList<ActorInterface*> actors = PluginManager::instance()->findPlugins<ActorInterface>();
    Q_FOREACH(ActorInterface * actor, actors) {
        actor->terminateEvaluation();
    }
}

void Run::runStepOver()
{
    stepDoneFlag_ = false;
    stoppingFlag_ = false;
    breakHitFlag_ = false;
    runMode_ = RM_StepOver;
    vm->setNextCallStepOver();
    start();
}

void Run::runStepIn()
{
    stepDoneFlag_ = false;
    breakHitFlag_ = false;
    runMode_ = RM_StepIn;
    vm->setNextCallInto();
    start();
}

void Run::runToEnd()
{
    stepDoneFlag_ = false;
    algDoneFlag_ = false;
    breakHitFlag_ = false;
    ignoreLineChangeFlag_ = false;
    emit lineChanged(-1, 0u, 0u);
    runMode_ = RM_StepOut;
    vm->setNextCallToEndOfContext();
    start();
}

void Run::runBlind()
{
    stoppingFlag_ = false;
    breakHitFlag_ = false;
    ignoreLineChangeFlag_ = false;
    runMode_ = RM_ToEnd;
    vm->setDebugOff(true);
    vm->setNextCallToEnd();
    start();
}

void Run::runContinuous()
{
    runMode_ = RM_ToEnd;
    stoppingFlag_ = false;
    breakHitFlag_ = false;
    ignoreLineChangeFlag_ = false;
    vm->setNextCallToEnd();
    start();
}


void Run::debuggerReset()
{
    variablesModel_->clear();
}

void Run::debuggerNoticeBeforePopContext()
{
    int index = variablesModel_->rowCount(QModelIndex()) - 1;
    variablesModel_->beginRemoveRows(QModelIndex(), index, index);
}

void Run::debuggerNoticeAfterPopContext()
{
    variablesModel_->endRemoveRows();
}

void Run::debuggerNoticeBeforePushContext()
{
    int index = variablesModel_->rowCount(QModelIndex());
    variablesModel_->beginInsertRows(QModelIndex(), index, index);
}

void Run::debuggerNoticeAfterPushContext()
{
    variablesModel_->endInsertRows();    
}

void Run::debuggerNoticeBeforeArrayInitialize(const Variable & variable,
                                              const int bounds[7])
{
    int firstStart = bounds[0];
    int firstEnd = bounds[1];
    int count = firstEnd - firstStart + 1;
    QModelIndex variableIndex;
    KumVariableItem * item = nullptr;
    for (int i=0; i<variablesModel_->cache_.size(); i++) {
        KumVariableItem * it = variablesModel_->cache_[i];
        if (it->itemType() == KumVariableItem::Variable) {
            if (it->variable() == &variable) {
                item = it;
                break;
            }
        }
    }
    if (item && variablesModel_->modelIndeces_.contains(item)) {
        variableIndex = variablesModel_->modelIndeces_[item];
    }
    variablesModel_->beginInsertRows(variableIndex, 0, count - 1);
}

void Run::debuggerNoticeAfterArrayInitialize(const Variable & variable)
{
    variablesModel_->endInsertRows();
    variablesModel_->emitValueChanged(variable, QVector<int>());
}

void Run::debuggerNoticeOnValueChanged(const Variable & variable, const int * indeces)
{
    QVector<int> ind = indeces==nullptr
            ? QVector<int>()
            : QVector<int>(indeces[3]);
    if (ind.size() > 0)
        ::memcpy(ind.data(), indeces, indeces[3] * sizeof(int));
    variablesModel_->emitValueChanged(variable, ind);
}

void Run::debuggerNoticeOnBreakpointHit(const String &filename, const quint32 lineNo)
{
    stepDoneMutex_->lock();
    stepDoneFlag_ = true;
    stepDoneMutex_->unlock();
    breakHitMutex_->lock();
    breakHitFlag_ = true;
    ignoreLineChangeFlag_ = true;
    breakHitMutex_->unlock();
    runMode_ = RM_StepOver;
    vm->setNextCallStepOver();
    emit breakpointHit(QString::fromStdWString(filename), lineNo);
}

bool Run::appendTextToMargin(int l, const String & s)
{
    emit marginText(l, QString::fromStdWString(s));
    return true;
}

bool Run::setTextToMargin(int lineNo, const String &s, bool red)
{
    emit marginTextReplace(lineNo, QString::fromStdWString(s), red);
    return true;
}

bool Run::noticeOnLineChanged(int lineNo, quint32 colStart, quint32 colEnd)
{
    if (ignoreLineChangeFlag_) {
        ignoreLineChangeFlag_ = false;
        return true;
    }
    stepDoneMutex_->lock();
    stepDoneFlag_ = true;
    stepDoneMutex_->unlock();
    if (mustStop())
        emit lineChanged(lineNo, colStart, colEnd);
    else
        emit lineChanged(-1, 0u, 0u);
    return true;
}

bool Run::noticeOnFunctionReturn()
{
    algDoneMutex_->lock();
    algDoneFlag_ = true;
    algDoneMutex_->unlock();
    emit lineChanged(vm->effectiveLineNo(), vm->effectiveColumn().first, vm->effectiveColumn().second);
    return true;
}

bool Run::noticeOnStepsChanged(quint64 stepsDone)
{
    emit updateStepsCounter(stepsDone);
    return true;
}

bool Run::clearMargin(int from, int to)
{
    emit clearMarginRequest(from, to);
    return true;
}

bool Run::mustStop() const
{
    QMutexLocker l1(stoppingMutex_);
    QMutexLocker l2(stepDoneMutex_);
    QMutexLocker l3(breakHitMutex_);


    if (vm->error().length()>0) {        
        return true;
    }

    if (stoppingFlag_) {
        return true;
    }

    if (breakHitFlag_) {
        return true;
    }

    if (runMode_==RM_StepOut) {
        return algDoneFlag_;
    }
    else if (runMode_!=RM_ToEnd) {
        return stepDoneFlag_;
    }
    else {
        return false;
    }
}

void Run::handleAlgorhitmDone(int lineNo, quint32 colStart, quint32 colEnd)
{
    algDoneMutex_->lock();
    algDoneFlag_ = true;
    algDoneMutex_->unlock();
    if (mustStop())
        emit lineChanged(lineNo, colStart, colEnd);
    else
        emit lineChanged(-1, 0u, 0u);
}

void Run::handlePauseRequest()
{
    runMode_ = RM_StepOver;
    vm->setNextCallStepOver();
    vm->setDebugOff(false);
}

void Run::removeAllBreakpoints()
{
    vm->removeAllBreakpoints();
}

void Run::insertOrChangeBreakpoint(bool enabled, const QString &fileName, quint32 lineNo, quint32 ignoreCount, const QString &condition)
{
    const String wFileName = fileName.toStdWString();
    const String wCondition = condition.toStdWString();
    vm->insertOrChangeBreakpoint(enabled, wFileName, lineNo, ignoreCount, wCondition);
}

void Run::insertSingleHitBreakpoint(const QString &fileName, quint32 lineNo)
{
    const String wFileName = fileName.toStdWString();
    vm->insertSingleHitBreakpoint(wFileName, lineNo);
}

void Run::removeBreakpoint(const QString &fileName, quint32 lineNo)
{
    const String wFileName = fileName.toStdWString();
    vm->removeBreakpoint(wFileName, lineNo);
}

void Run::run()
{    
    while (vm->hasMoreInstructions()) {
        if (mustStop()) {
            break;
        }
        vm->evaluateNextInstruction();        
        if (vm->error().length()>0 && !stoppingFlag_) {
            int lineNo = vm->effectiveLineNo();
            std::pair<quint32,quint32> colNo =
                    vm->effectiveColumn();
            emit lineChanged(lineNo, colNo.first, colNo.second);
            emit error(QString::fromStdWString(vm->error()));
            break;
        }
    }
    if (vm->error().length() == 0 && !stoppingFlag_ &&
            vm->entryPoint() == KumirVM::EP_Testing)
    {
        qApp->setProperty("returnCode", vm->returnCode());
    }
//    bool wasError = vm->error().length()>0;
    // Unclosed files is an error only if program reached end
    bool unclosedFilesIsNotError = stoppingFlag_ || vm->hasMoreInstructions();
    // Must close all files if program reached end or user terminated
    bool programFinished = stoppingFlag_ || !vm->hasMoreInstructions() || vm->error().length();
//    __check_for_unclosed_files__st_funct(unclosedFilesIsNotError, closeUnclosedFiles);
//    vm->updateStFunctError();
//    if (!wasError && vm->error().length()>0) {
//        emit lineChanged(vm->effectiveLineNo());
//        emit error(QString::fromStdWString(vm->error()));
//    }
    if (programFinished)
        Kumir::finalizeStandardLibrary();
    emit aboutToStop();
}

int Run::effectiveLineNo() const
{
    return vm->effectiveLineNo();
}

bool Run::loadProgramFromBinaryBuffer(std::list<char> &stream, const String & filename)
{
    breakpoints_.clear();
    Kumir::EncodingError encodingError;
    String errorMessage;
    bool ok = vm->loadProgramFromBinaryBuffer(stream, true, filename, errorMessage);
    if (!ok) {
        std::string msg;
#if defined(WIN32) || defined(_WIN32)
        msg = Kumir::Coder::encode(Kumir::CP866, errorMessage, encodingError);
#else
        msg = Kumir::Coder::encode(Kumir::UTF8, errorMessage, encodingError);
#endif
        std::cerr << msg << std::endl;
        programLoadError_ = QString::fromUtf8("Ошибка загрузки программы: %1")
                .arg(QString::fromStdWString(errorMessage));
        emit error(programLoadError_);

    }
    return ok;
}

void Run::setEntryPointToMain()
{
    vm->setEntryPoint(KumirVM::EP_Main);
}

void Run::setEntryPointToTest()
{
    vm->setEntryPoint(KumirVM::EP_Testing);
}

bool Run::isTestingRun() const
{
    return vm->entryPoint() == KumirVM::EP_Testing;
}

QString Run::error() const
{
    return programLoadError_.length() > 0
            ? programLoadError_ : QString::fromStdWString(vm->error());
}

bool Run::hasTestingAlgorithm() const
{
    return vm->hasTestingAlgorithm();
}


void Run::reset()
{
    breakHitFlag_ = false;
    ignoreLineChangeFlag_ = false;
    vm->reset();
}

bool Run::hasMoreInstructions() const
{
    return vm->hasMoreInstructions();
}

void Run::evaluateNextInstruction()
{
    vm->evaluateNextInstruction();
}

bool Run::canStepOut() const
{
    return vm->canStepOut();
}

QVariant Run::valueStackTopItem() const
{
    VMMutex_->lock();
    AnyValue value;
    value = vm->returnCode();
    VMMutex_->unlock();
    QVariant result;
    if (value.isValid()) {
        if (value.type() == VT_int) {
            result = QVariant(value.toInt());
        }
        else if (value.type() == VT_real) {
            result = QVariant(value.toReal());
        }
        else if (value.type() == VT_bool) {
            result = QVariant(value.toBool());
        }
        else if (value.type() == VT_char) {
            result = QVariant(QChar(value.toChar()));
        }
        else if (value.type() == VT_string) {
            result = QVariant(QString::fromStdWString(value.toString()));
        }
    }
    return result;
}




} // namespace KumirCodeRun


