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
    runMode_ = RM_ToEnd;

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
//        emit finished();
    }
}

void Run::runStepOver()
{
    stepDoneFlag_ = false;
    stoppingFlag_ = false;
    runMode_ = RM_StepOver;
    vm->setNextCallStepOver();
    start();
}

void Run::runStepIn()
{
    stepDoneFlag_ = false;
    runMode_ = RM_StepIn;
    vm->setNextCallInto();
    start();
}

void Run::runToEnd()
{
    stepDoneFlag_ = false;
    algDoneFlag_ = false;
    emit lineChanged(-1, 0u, 0u);
    runMode_ = RM_StepOut;
    vm->setNextCallToEndOfContext();
    start();
}

void Run::runBlind()
{
    stoppingFlag_ = false;
    runMode_ = RM_ToEnd;
    vm->setDebugOff(true);
    vm->setNextCallToEnd();
    start();
}

void Run::runContinuous()
{
    runMode_ = RM_ToEnd;
    stoppingFlag_ = false;
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
    stoppingMutex_->lock();
    stepDoneMutex_->lock();

    if (vm->error().length()>0) {
        stoppingMutex_->unlock();
        stepDoneMutex_->unlock();
        return true;
    }

    if (stoppingFlag_) {
        stoppingMutex_->unlock();
        stepDoneMutex_->unlock();
        return true;
    }

    if (runMode_==RM_StepOut) {
        stoppingMutex_->unlock();
        stepDoneMutex_->unlock();
        return algDoneFlag_;
    }
    else if (runMode_!=RM_ToEnd) {
        stoppingMutex_->unlock();
        stepDoneMutex_->unlock();
        return stepDoneFlag_;
    }
    else {
        stoppingMutex_->unlock();
        stepDoneMutex_->unlock();
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
        bool hasRetval = false;
        try {
            VM::AnyValue retval = vm->topLevelStackValue();
            hasRetval = retval.isValid();
        }
        catch (...) {
            hasRetval = false;
        }
        if (hasRetval) {
            qApp->setProperty("returnCode", vm->topLevelStackValue().toInt());
        }
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
    String errorMessage;
    bool ok = vm->loadProgramFromBinaryBuffer(stream, true, filename, errorMessage);
    if (!ok) {
        std::string msg;
#if defined(WIN32) || defined(_WIN32)
        msg = Kumir::Coder::encode(Kumir::CP866, errorMessage);
#else
        msg = Kumir::Coder::encode(Kumir::UTF8, errorMessage);
#endif
        std::cerr << msg << std::endl;
        programLoadError_ = QString::fromUtf8("Ошибка загрузки программы: %1")
                .arg(QString::fromStdWString(errorMessage));
        emit error(programLoadError_);

    }
    return ok;
}

void Run::loadProgramFromTextBuffer(const std::string &stream, const String & filename)
{
    String error;
    if (!vm->loadProgramFromTextBuffer(stream, true, filename, error)) {
        std::string msg;
#if defined(WIN32) || defined(_WIN32)
        msg = Kumir::Coder::encode(Kumir::CP866, error);
#else
        msg = Kumir::Coder::encode(Kumir::UTF8, error);
#endif
        std::cerr << msg << std::endl;
    }
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
    try {
        value = vm->topLevelStackValue();
    }
    catch (std::string & e) {
        qDebug() << e.c_str();
    }
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


