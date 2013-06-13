#include <cstdlib>
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

class Mutex: public CriticalSectionLocker
{
public:
    inline Mutex() { m = new QMutex; }
    inline void lock() { m->lock(); }
    inline void unlock() { m->unlock(); }
    inline ~Mutex() { delete m; }
private:
    QMutex * m;
};


Run::Run(QObject *parent) :
    QThread(parent)
{
    programLoaded = false;
    vm = new KumirVM();
    VMMutex_ = new Mutex;
    vm->setMutex(VMMutex_);

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
        emit finished();
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

void Run::runStepOut()
{
    stepDoneFlag_ = false;
    algDoneFlag_ = false;
    emit lineChanged(-1, 0u, 0u);
    runMode_ = RM_StepOut;
    vm->setNextCallOut();
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


bool Run::debuggerReset()
{
    emit signal_debuggerReset();
    return true;
}

bool Run::debuggerPopContext()
{
    emit signal_debuggerPopContext();
    return true;
}

bool Run::debuggerPushContext(
        const Kumir::String & contextName,
        const std::deque<Kumir::String> & names,
        const std::deque<Kumir::String> & baseTypes,
        const std::deque<uint8_t> & dimensions)
{
    const QString qContextName = QString::fromStdWString(contextName);
    QStringList qNames, qBaseTypes;
    QList<int> qDimensions;
    for (size_t i=0; i<names.size(); i++) {
        qNames.push_back(QString::fromStdWString(names.at(i)));
        qBaseTypes.push_back(QString::fromStdWString(baseTypes.at(i)));
        qDimensions.push_back(static_cast<int>(dimensions.at(i)));
    }
    emit signal_debuggerPushContext(qContextName,
                                    qNames,
                                    qBaseTypes,
                                    qDimensions);
    return true;
}

bool Run::debuggerSetGlobals(
        const Kumir::String & moduleName,
        const std::deque<Kumir::String> & names,
        const std::deque<Kumir::String> & baseTypes,
        const std::deque<uint8_t> & dimensions)
{
    const QString qModuleName = QString::fromStdWString(moduleName);
    QStringList qNames, qBaseTypes;
    QList<int> qDimensions;
    for (size_t i=0; i<names.size(); i++) {
        qNames.push_back(QString::fromStdWString(names.at(i)));
        qBaseTypes.push_back(QString::fromStdWString(baseTypes.at(i)));
        qDimensions.push_back(static_cast<int>(dimensions.at(i)));
    }
    emit signal_debuggerSetGlobals(qModuleName,
                                   qNames,
                                   qBaseTypes,
                                   qDimensions);
    return true;
}

bool Run::debuggerUpdateLocalVariable(
        const Kumir::String & name,
        const Kumir::String & value)
{
    const QString qName = QString::fromStdWString(name);
    const QString qValue = QString::fromStdWString(value);
    emit signal_debuggerUpdateLocalVariable(qName, qValue);
    return true;
}

bool Run::debuggerUpdateGlobalVariable(
        const Kumir::String & moduleName,
        const Kumir::String & name,
        const Kumir::String & value)
{
    const QString qModuleName = QString::fromStdWString(moduleName);
    const QString qName = QString::fromStdWString(name);
    const QString qValue = QString::fromStdWString(value);
    emit signal_debuggerUpdateGlobalVariable(qModuleName, qName, qValue);
    return true;
}

bool Run::debuggerUpdateLocalTableBounds(
        const Kumir::String & name,
        const int bounds[7])
{
    const QString qName = QString::fromStdWString(name);
    QList<int> qBounds;
    int count = bounds[6];
    for (int i=0; i<count; i++) {
        qBounds.push_back(bounds[i]);
    }
    emit signal_debuggerUpdateLocalTableBounds(qName, qBounds);
    return true;
}

bool Run::debuggerUpdateGlobalTableBounds(
        const Kumir::String & moduleName,
        const Kumir::String & name,
        const int bounds[7])
{
    const QString qModuleName = QString::fromStdWString(moduleName);
    const QString qName = QString::fromStdWString(name);
    QList<int> qBounds;
    int count = bounds[6];
    for (int i=0; i<count; i++) {
        qBounds.push_back(bounds[i]);
    }
    emit signal_debuggerUpdateGlobalTableBounds(qModuleName, qName, qBounds);
    return true;
}

bool Run::debuggerSetLocalReference(
        const Kumir::String & name,
        const Kumir::String & targetName,
        const int tableIndeces[4],
        const int stackStepsBackward,
        const Kumir::String & moduleName
        )
{
    const QString qName = QString::fromStdWString(name);
    const QString qTargetName = QString::fromStdWString(targetName);
    const QString qModuleName = QString::fromStdWString(moduleName);
    QList<int> qIndeces;
    for (int i=0; i<tableIndeces[3]; i++) {
        qIndeces.push_back(tableIndeces[i]);
    }
    emit signal_debuggerSetLocalReference(
                qName,
                qTargetName,
                qIndeces,
                stackStepsBackward,
                qModuleName
                );
    return true;
}

bool Run::debuggerForceUpdateValues()
{
    emit signal_debuggerForceUpdateValues();
    return true;
}

bool Run::debuggerUpdateLocalTableValue(const Kumir::String &name,
                                        const int indeces[4]
                                        )
{
    const QString qName = QString::fromStdWString(name);
    QList<int> qIndeces;
    for (int i=0; i<indeces[3]; i++) {
        qIndeces.push_back(indeces[i]);
    }
    emit signal_debuggerUpdateLocalTableValue(qName, qIndeces);
    return true;
}


bool Run::debuggerUpdateGlobalTableValue(
        const Kumir::String &moduleName,
        const Kumir::String &name,
        const int indeces[4]
        )
{
    const QString qModuleName = QString::fromStdWString(moduleName);
    const QString qName = QString::fromStdWString(name);
    QList<int> qIndeces;
    for (int i=0; i<indeces[3]; i++) {
        qIndeces.push_back(indeces[i]);
    }
    emit signal_debuggerUpdateGlobalTableValue(qModuleName, qName, qIndeces);
    return true;
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
        emit error(QString::fromUtf8("Ошибка загрузки программы: %1")
                    .arg(QString::fromStdWString(errorMessage)));
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

QString Run::error() const
{
    return QString::fromStdWString(vm->error());
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


