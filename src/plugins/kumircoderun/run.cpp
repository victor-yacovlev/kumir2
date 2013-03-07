#include <cstdlib>
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm.hpp"
#include "run.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"

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
    externalModuleResetFunctor_ = new ExternalModuleResetFunctor();
    externalModuleLoadFunctor_ = new ExternalModuleLoadFunctor();
    externalModuleCallFunctor_ = new ExternalModuleCallFunctor(this);
    customTypeFromStringFunctor_ = new CustomTypeFromStringFunctor();
    customTypeToStringFunctor_ = new CustomTypeToStringFunctor();
    inputFunctor_ = new InputFunctor(this, customTypeFromStringFunctor_);
    outputFunctor_ = new OutputFunctor(this, customTypeToStringFunctor_);
    getMainArgumentFunctor_ = new GetMainArgumentFunctor(this, customTypeFromStringFunctor_);
    returnMainValueFunctor_ = new ReturnMainValueFunctor(this, customTypeToStringFunctor_);

    vm->setFunctor(externalModuleLoadFunctor_);
    vm->setFunctor(externalModuleResetFunctor_);
    vm->setFunctor(externalModuleCallFunctor_);
    vm->setFunctor(inputFunctor_);
    vm->setFunctor(outputFunctor_);
    vm->setFunctor(customTypeFromStringFunctor_);
    vm->setFunctor(customTypeToStringFunctor_);
    vm->setFunctor(getMainArgumentFunctor_);
    vm->setFunctor(returnMainValueFunctor_);

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
        emit lineChanged(-1);
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
    emit lineChanged(-1);
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


QVariant VariableToQVariant(const Variable & var)
{
    QVariant result;
    if (var.dimension()==0) {
        if (!var.isValid())
            result = QVariant::Invalid;
        else if (var.baseType()==VT_int)
            result = QVariant(var.toInt());
        else if (var.baseType()==VT_real)
            result = QVariant(var.toReal());
        else if (var.baseType()==VT_bool)
            result = QVariant(var.toBool());
        else if (var.baseType()==VT_char)
            result = QVariant(QChar(var.toChar()));
        else if (var.baseType()==VT_string)
            result = QVariant(QString::fromStdWString(var.toString()));
        else if (var.baseType()==VT_record) {
            QVariantList recData;
            const AnyValue & val = var.value();
            const Record & record = val.toRecord();
            for (int j=0; j<record.fields.size(); j++) {
                const VM::AnyValue & field = record.fields[j];
                if (field.type()==VT_int)
                    recData << QVariant(field.toInt());
                else if (field.type()==VT_real)
                    recData << QVariant(field.toReal());
                else if (field.type()==VT_bool)
                    recData << QVariant(field.toBool());
                else if (field.type()==VT_char)
                    recData << QVariant(QChar(field.toChar()));
                else if (field.type()==VT_string)
                    recData << QVariant(QString::fromStdWString(field.toString()));
            }
            result = recData;
        }
    }
    else {
        // TODO implement me
    }
    return result;
}

VM::AnyValue QVariantToValue(const QVariant & var, int dim)
{
    AnyValue aval;
    if (!var.isValid())
        return aval;
    if (dim==0) {
        if (var.type()==QVariant::Int)
            aval = AnyValue(var.toInt());
        else if (var.type()==QVariant::Double)
            aval = AnyValue(var.toDouble());
        else if (var.type()==QVariant::Bool)
            aval = AnyValue(var.toBool());
        else if (var.type()==QVariant::Char)
            aval = AnyValue(var.toChar().unicode());
        else if (var.type()==QVariant::String)
            aval = AnyValue(var.toString().toStdWString());
        else if (var.type()==QVariant::List) {
            Record record;
            const QVariantList & recvals = var.toList();
            for (int i=0; i<recvals.size(); i++) {
                AnyValue fieldVal;
                const QVariant & rval = recvals.at(i);
                if (rval.type()==QVariant::Int)
                    fieldVal = AnyValue(rval.toInt());
                else if (rval.type()==QVariant::Double)
                    fieldVal = AnyValue(rval.toDouble());
                else if (rval.type()==QVariant::Bool)
                    fieldVal = AnyValue(rval.toBool());
                else if (rval.type()==QVariant::Char)
                    fieldVal = AnyValue(rval.toChar().unicode());
                else if (rval.type()==QVariant::String)
                    fieldVal = AnyValue(rval.toString().toStdWString());
                record.fields.push_back(fieldVal);
            }
            aval = AnyValue(record);
        }
    }
    else {
        // TODO implement me
    }
    return aval;
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

bool Run::noticeOnValueChange(int l, const String & s)
{
    emit marginText(l, QString::fromStdWString(s));
    return true;
}

bool Run::noticeOnLineNoChanged(int lineNo)
{
    stepDoneMutex_->lock();
    stepDoneFlag_ = true;
    stepDoneMutex_->unlock();
    if (mustStop())
        emit lineChanged(lineNo);
    else
        emit lineChanged(-1);
    return true;
}

bool Run::clearMargin(int from, int to)
{
    emit clearMarginRequest(from, to);
    return true;
}

bool Run::mustStop() const
{
    QMutexLocker l1(stoppingMutex_); l1;
    QMutexLocker l2(stepDoneMutex_); l2;

    if (vm->error().length()>0)
        return true;

    if (stoppingFlag_)
        return true;

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

void Run::handleAlgorhitmDone(int lineNo)
{
    algDoneMutex_->lock();
    algDoneFlag_ = true;
    algDoneMutex_->unlock();
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
        if (vm->error().length()>0) {
            int lineNo = vm->effectiveLineNo();
            emit lineChanged(lineNo);
            emit error(QString::fromStdWString(vm->error()));
            break;
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

void Run::loadProgramFromBinaryBuffer(std::list<char> &stream, const String & filename)
{
    String error;
    if (!vm->loadProgramFromBinaryBuffer(stream, true, filename, error)) {
        std::string msg;
#if defined(WIN32) || defined(_WIN32)
        msg = Kumir::Coder::encode(Kumir::CP866, error);
#else
        msg = Kumir::Coder::encode(Kumir::UTF8, error);
#endif
        std::cerr << msg << std::endl;
    }
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

// --------------------- FUNCTORS IMPLEMENTATION
// TODO move to separate files



void ExternalModuleResetFunctor::operator ()(const String & moduleName)
{
    const QString qModuleName = QString::fromStdWString(moduleName);
    QList<ExtensionSystem::KPlugin*> plugins =
            ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
    Shared::ActorInterface * actor = 0;
    for (int i=0; i<plugins.size(); i++) {
        actor = qobject_cast<Shared::ActorInterface*>(plugins[i]);
        if (actor) {
            if (actor->name()==qModuleName)
                break;
            else
                actor = 0;
        }
    }
    if (actor) {
        actor->reset();
    }
    else {
        const QString qErrorMessage =
                QString::fromUtf8("Нет такого исполнителя: \"%1\"")
                .arg(qModuleName);
        const String errorMessage =
                qErrorMessage.toStdWString();
        throw errorMessage;
    }
}

ExternalModuleCallFunctor::ExternalModuleCallFunctor(QObject *parent)
    : QObject(parent)
    , finishedFlag_(false)
    , finishedMutex_(new QMutex)
{
}

ExternalModuleCallFunctor::~ExternalModuleCallFunctor()
{
    delete finishedMutex_;
}

AnyValue ExternalModuleCallFunctor::operator ()
(
    const String & moduleName,
    const uint16_t algKey,
    VariableReferencesList alist
)
{
    // Clear state
    finishedFlag_ = false;

    // Convert STL+Kumir into Qt value types
    const QString qModuleName = QString::fromStdWString(moduleName);
    const quint16 qAlgKey = quint16(algKey);
    QVariantList arguments;
    for (auto it=alist.begin(); it!=alist.end(); ++it) {
        const QVariant qVal = VariableToQVariant(*it);
        arguments.push_back(qVal);
    }

    // Find an actor (or throw)
    Shared::ActorInterface * actor = findActor(qModuleName);

    if (actor->evaluate(qAlgKey, arguments)==Shared::ES_Async) {
        // Wait for actor thread to finish
        forever {
            bool done = false;
            finishedMutex_->lock();
            done = finishedFlag_;
            finishedMutex_->unlock();
            if (!done) {
                SleepFunctions::msleep(1);
            }
            else {
                break;
            }
        }
    }

    // Collect actor result
    const QString errorMessage = actor->errorText();
    const QVariant returnValue = actor->result();
    const QVariantList argumentReturnValues = actor->algOptResults();

    // Check for runtime error
    if (errorMessage.length()>0) {
        const String message = errorMessage.toStdWString();
        throw message;
    }

    // Get result
    AnyValue result = QVariantToValue(returnValue, 0);

    // Check for out and in/out arguments and store them
    for (size_t i=0; i<alist.size(); i++) {
        Variable var = alist.at(i);
        const QVariant & qval = argumentReturnValues.at(i);
        if (var.isReference() && qval.isValid()) {
            quint8 dim = quint8(var.dimension());
            const AnyValue kval = QVariantToValue(qval, dim);
            var.setValue(kval);
        }
    }

    // Disconnect an actor
    releaseActor(actor);

    // Return a result
    return result;
}

void ExternalModuleCallFunctor::handleActorSync()
{
    finishedMutex_->lock();
    finishedFlag_ = true;
    finishedMutex_->unlock();
}

Shared::ActorInterface * ExternalModuleCallFunctor::findActor(
        const QString &moduleName)
{
    Shared::ActorInterface * actor = nullptr;
    QObject * actorObject = nullptr;
    QList<ExtensionSystem::KPlugin*> plugins =
            ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
    for (int i=0; i<plugins.size(); i++) {
        actor = qobject_cast<Shared::ActorInterface*>(plugins[i]);
        actorObject = plugins[i];
        if (actor) {
            if (actor->name()==moduleName)
                break;
            else
                actor = 0;
        }
    }
    if (!actor) {
        throw
        QString::fromUtf8("Нет такого исполнителя: %1")
                .arg(moduleName)
                .toStdWString();
    }
    else {
        connect(actorObject, SIGNAL(sync()), this, SLOT(handleActorSync()));
    }
    return actor;
}

void ExternalModuleCallFunctor::releaseActor(
        Shared::ActorInterface *actor)
{
    QObject * actorObject = dynamic_cast<QObject*>(actor);
    disconnect(actorObject, SIGNAL(sync()), this, SLOT(handleActorSync()));
}

InputFunctor::InputFunctor(
        class Run * parent,
        VM::CustomTypeFromStringFunctor * converter
        )
    : QObject(parent)
    , finishedFlag_(false)
    , finishedMutex_(new QMutex)
    , inputValues_(QVariantList())
    , converter_(converter)
    , runner_(parent)
{
    connect(parent, SIGNAL(finishInput(QVariantList)),
            this, SLOT(handleInputDone(QVariantList)));
    connect(this, SIGNAL(requestInput(QString)),
            parent, SIGNAL(input(QString)));
}

InputFunctor::~InputFunctor()
{
    delete finishedMutex_;
}

void InputFunctor::operator ()(VariableReferencesList references)
{
    // Clear state
    finishedFlag_ = false;
    inputValues_.clear();

    // Prepare input format for GUI
    String format;
    for (int i=0; i<references.size(); i++) {
        if (references[i].baseType()==VT_int) {
            format.push_back('i');
        }
        else if (references[i].baseType()==VT_real) {
            format.push_back('r');
        }
        else if (references[i].baseType()==VT_bool) {
            format.push_back('b');
        }
        else if (references[i].baseType()==VT_char &&
                 references[i].isConstant() &&
                 references[i].value().toChar() == Char('\n'))
        {
            format.push_back('n');
        }
        else if (references[i].baseType()==VT_char) {
            format.push_back('c');
        }
        else if (references[i].baseType()==VT_string) {
            format.push_back('s');
        }
        else if (references[i].baseType()==VT_record) {
            const Variable & variable = references[i];
            const String typeFullName =
                    variable.recordModuleName()+
                    Kumir::Core::fromAscii("::")+
                    variable.recordClassName();
            format.append(typeFullName);
        }
        if (i<references.size()-1) format.push_back(';');
    }

    const QString qFormat = QString::fromStdWString(format);

    // Request input action and wait for response
    emit requestInput(qFormat);
    forever {
        bool done = false;
        finishedMutex_->lock();
        done = finishedFlag_;
        finishedMutex_->unlock();
        if (runner_->mustStop()) {
            break;
        }
        else if (!done) {
            SleepFunctions::msleep(1);
        }
        else {
            break;
        }
    }

    if (runner_->mustStop())
        return; // Do nothing on exit

    // Store input values
    Q_ASSERT(inputValues_.size()==references.size());
    for (int i=0; i<inputValues_.size(); i++) {
        const AnyValue val = QVariantToValue(inputValues_.at(i), 0);
        references[i].setValue(val);
    }
}

void InputFunctor::handleInputDone(const QVariantList & values)
{
    finishedMutex_->lock();
    finishedFlag_ = true;
    inputValues_ = values;
    finishedMutex_->unlock();
}

OutputFunctor::OutputFunctor(QObject *parent, VM::CustomTypeToStringFunctor * converter)
    : QObject(parent)
    , converter_(converter)
{
    connect(this, SIGNAL(requestOutput(QString)),
            parent, SIGNAL(output(QString)));
}

void OutputFunctor::operator ()(
        VariableReferencesList values,
        FormatsList formats
        )
{
    Kumir::IO::OutputStream os;
    for (int i=0; i<formats.size(); i++) {
        std::pair<int,int> format = formats[i];
        if (values[i].baseType()==VT_int) {
            Kumir::IO::writeInteger(os, values[i].toInt(), format.first);
        }
        else if (values[i].baseType()==VT_real) {
            Kumir::IO::writeReal(os, values[i].toDouble(), format.first, format.second);
        }
        else if (values[i].baseType()==VT_bool) {
            Kumir::IO::writeBool(os, values[i].toBool(), format.first);
        }
        else if (values[i].baseType()==VT_char) {
            Kumir::IO::writeChar(os, values[i].toChar(), format.first);
        }
        else if (values[i].baseType()==VT_string) {
            Kumir::IO::writeString(os, values[i].toString(), format.first);
        }
        else if (values[i].baseType()==VT_record) {
            String svalue;
            if (converter_) {
                svalue = (*converter_)(values[i]);
                Kumir::IO::writeString(os, svalue, format.first);
            }
        }
    }
    QString data = QString::fromStdWString(os.getBuffer());
    emit requestOutput(data);
    SleepFunctions::usleep(100);
}

GetMainArgumentFunctor::GetMainArgumentFunctor(
        Run * parent,
        VM::CustomTypeFromStringFunctor * converter
        )
    : QObject(parent)
    , finishedFlag_(false)
    , finishedMutex_(new QMutex)
    , inputValues_(QVariantList())
    , converter_(converter)
    , runner_(parent)
{
    connect(parent, SIGNAL(finishInput(QVariantList)),
            this, SLOT(handleInputDone(QVariantList)));
    connect(this, SIGNAL(requestInput(QString)),
            parent, SIGNAL(input(QString)));
    connect(this, SIGNAL(requestOutput(QString)),
            parent, SIGNAL(outputRequest(QString)));
}

GetMainArgumentFunctor::~GetMainArgumentFunctor()
{
    delete finishedMutex_;
}

void GetMainArgumentFunctor::operator ()(Variable & reference)
{
    QString format;
    if (reference.baseType()==VT_int)
        format.push_back('i');
    else if (reference.baseType()==VT_real)
        format.push_back('r');
    else if (reference.baseType()==VT_bool)
        format.push_back('b');
    else if (reference.baseType()==VT_char)
        format.push_back('c');
    else if (reference.baseType()==VT_string)
        format.push_back('s');

    if (reference.dimension()==0) {
        AnyValue val;
        if (inputScalarArgument(QString::fromUtf8("Введите %1: ").arg(QString::fromStdWString(reference.name())),format,val))
            reference.setValue(val);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            AnyValue val;
            if (inputScalarArgument(QString::fromUtf8("Введите %1[%2]: ").arg(QString::fromStdWString(reference.name())).arg(x),format,val))
                reference.setValue(x,val);
            else
                return;
        }
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                AnyValue val;
                if (inputScalarArgument(QString::fromUtf8("Введите %1[%2,%3]: ").arg(QString::fromStdWString(reference.name())).arg(y).arg(x),format,val))
                    reference.setValue(y,x,val);
                else
                    return;
            }
        }
    }
    else if (reference.dimension()==3) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int z=bounds[0]; z<=bounds[1]; z++) {
            for (int y=bounds[2]; y<=bounds[3]; y++) {
                for (int x=bounds[4]; x<=bounds[5]; x++) {
                    AnyValue val;
                    if (inputScalarArgument(QString::fromUtf8("Введите %1[%2,%3,%4]: ").arg(QString::fromStdWString(reference.name())).arg(z).arg(y).arg(x),format,val))
                        reference.setValue(z,y,x,val);
                    else
                        return;
                }
            }
        }
    }
}

void GetMainArgumentFunctor::handleInputDone(const QVariantList &values)
{
    QMutexLocker lock(finishedMutex_); lock;
    finishedFlag_ = true;
    inputValues_ = values;
}


ReturnMainValueFunctor::ReturnMainValueFunctor(
        QObject *parent,
        VM::CustomTypeToStringFunctor * converter
        )
    : QObject(parent)
    , converter_(converter)
{
    connect(this, SIGNAL(requestOutput(QString)),
            parent, SIGNAL(outputRequest(QString)));
}

bool GetMainArgumentFunctor::inputScalarArgument(
        const QString &message,
        const QString & format,
        AnyValue &val
        )
{
    // Show message
    emit requestOutput(message);

    // Clear values
    inputValues_.clear();
    finishedFlag_ = false;

    emit requestInput(format);
    forever {
        bool done = false;
        finishedMutex_->lock();
        done = finishedFlag_;
        finishedMutex_->unlock();
        if (runner_->mustStop())
            break;
        else if (!done)
            SleepFunctions::msleep(1);
        else
            break;
    }
    if (runner_->mustStop())
        return false;

    if      (format[0]=='i')
        val = inputValues_[0].toInt();
    else if (format[0]=='r')
        val = inputValues_[0].toDouble();
    else if (format[0]=='b')
        val = inputValues_[0].toBool();
    else if (format[0]=='c')
        val = inputValues_[0].toChar().unicode();
    else if (format[0]=='s')
        val = inputValues_[0].toString().toStdWString();
    return true;
}

void ReturnMainValueFunctor::operator()(const Variable & reference)
{
    if (!reference.isValid())
        return;
    QString repr;
    emit requestOutput(QString::fromStdWString(reference.name())+" = ");
    if (reference.dimension()==0) {
        if (reference.hasValue()) {
            repr = QString::fromStdWString(reference.value().toString());
            if (reference.baseType()==Bytecode::VT_string)
                repr = "\"" + repr + "\"";
            else if (reference.baseType()==Bytecode::VT_char)
                repr = "'" + repr + "'";
        }
        emit requestOutput(repr);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        emit requestOutput("{ ");
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            repr = "";
            if (reference.hasValue(x)) {
                repr = QString::fromStdWString(reference.value(x).toString());
                if (reference.baseType()==Bytecode::VT_string)
                    repr = "\"" + repr + "\"";
                else if (reference.baseType()==Bytecode::VT_char)
                    repr = "'" + repr + "'";
            }
            emit requestOutput(repr);
            if (x<bounds[1]) {
                emit requestOutput(", ");
            }
        }
        emit requestOutput(" }");
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        emit requestOutput("{ ");
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            emit requestOutput("{ ");
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                repr = "";
                if (reference.hasValue(y,x)) {
                    repr = QString::fromStdWString(reference.value(y,x).toString());
                    if (reference.baseType()==Bytecode::VT_string)
                        repr = "\"" + repr + "\"";
                    else if (reference.baseType()==Bytecode::VT_char)
                        repr = "'" + repr + "'";
                }
                emit requestOutput(repr);
                if (x<bounds[1]) {
                    emit requestOutput(", ");
                }
            }
            emit requestOutput(" }");
            if (y<bounds[1]) {
                emit requestOutput(", ");
            }
        }
        emit requestOutput(" }");
    }
    else if (reference.dimension()==3) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        emit requestOutput("{ ");
        for (int z=bounds[0]; z<=bounds[1]; z++) {
            emit requestOutput("{ ");
            for (int y=bounds[2]; y<=bounds[3]; y++) {
                emit requestOutput("{ ");
                for (int x=bounds[4]; x<=bounds[5]; x++) {
                    repr = "";
                    if (reference.hasValue(z,y,x)) {
                        repr = QString::fromStdWString(reference.value(z,y,x).toString());
                        if (reference.baseType()==Bytecode::VT_string)
                            repr = "\"" + repr + "\"";
                        else if (reference.baseType()==Bytecode::VT_char)
                            repr = "'" + repr + "'";
                    }
                    emit requestOutput(repr);
                    if (x<bounds[1]) {
                        emit requestOutput(", ");
                    }
                }
                emit requestOutput(" }");
                if (y<bounds[1]) {
                    emit requestOutput(", ");
                }
            }
            emit requestOutput(" }");
            if (z<bounds[1]) {
                emit requestOutput(", ");
            }
        }
        emit requestOutput(" }");
    }
    emit requestOutput("\n");
}

String CustomTypeToStringFunctor::operator ()(const Variable & variable)
{
    const QString modName =
            QString::fromStdWString(variable.recordModuleName());
    const QString className =
            QString::fromStdWString(variable.recordClassName());

    QList<ExtensionSystem::KPlugin*> plugins =
            ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
    Shared::ActorInterface * actor = 0;
    for (int i=0; i<plugins.size(); i++) {
        actor = qobject_cast<Shared::ActorInterface*>(plugins[i]);
        if (actor) {
            if (actor->name()==modName)
                break;
            else
                actor = 0;
        }
    }
    String result;
    if (actor) {
        Shared::ActorInterface::CustomType ct;
        ct.first = className;
        QVariant value = VariableToQVariant(variable);
        const QString qString = actor->customValueToString(ct, value);
        result = qString.toStdWString();
    }
    else {
        throw QString::fromUtf8("Нет такого исполнителя: %1").arg(modName)
                .toStdWString();
    }
    return result;
}

AnyValue CustomTypeFromStringFunctor::operator ()(
        const String & source,
        const String & moduleName,
        const String & typeName
        )
{
    AnyValue result;
    QString errorMessage;
    const QString modName =
            QString::fromStdWString(moduleName);
    const QString className =
            QString::fromStdWString(typeName);
    const QString qString = QString::fromStdWString(source);

    QList<ExtensionSystem::KPlugin*> plugins =
            ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
    Shared::ActorInterface * actor = 0;
    for (int i=0; i<plugins.size(); i++) {
        actor = qobject_cast<Shared::ActorInterface*>(plugins[i]);
        if (actor) {
            if (actor->name()==modName)
                break;
            else
                actor = 0;
        }
    }
    if (actor) {
        Shared::ActorInterface::CustomType ct;
        ct.first = className;
        const QVariant value = actor->customValueFromString(ct, qString);
        if (value.isValid()) {
            result = QVariantToValue(value, 0);
        }
        else {
            errorMessage = QString::fromUtf8("Ошибка ввода значения типа \"%1\"")
                    .arg(className);
        }
    }
    else {
        errorMessage = QString::fromUtf8("Исполнитель не доступен: %1")
                .arg(modName);
    }
    if (errorMessage.length()>0)
        throw errorMessage.toStdWString();
    return result;
}

} // namespace KumirCodeRun


