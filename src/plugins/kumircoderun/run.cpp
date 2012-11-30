#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm.hpp"
#include "run.h"


namespace KumirCodeRun {

using Kumir::String;
using Kumir::real;

using namespace VM;

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
    vm->setMutex(new Mutex);

    i_originFunctionDeep = 0;
    b_interactDone = b_stopping = b_stepDone = b_algDone = false;
    mutex_stopping = new QMutex;
    mutex_stepDone = new QMutex;
    mutex_algDone = new QMutex;
    mutex_interactDone = new QMutex;
    e_runMode = RM_ToEnd;

    vm->setExternalHandler(this);
//    connect(vm, SIGNAL(pauseRequest()), this, SLOT(handlePauseRequest()), Qt::DirectConnection);

//    connect(vm, SIGNAL(lineNoChanged(int)), this, SLOT(handleLineChanged(int)), Qt::DirectConnection);
//    connect(vm, SIGNAL(retInstruction(int)), this, SLOT(handleAlgorhitmDone(int)), Qt::DirectConnection);

//    connect(vm, SIGNAL(inputRequest(QString,QList<quintptr>,QList<int>)),
//            this, SLOT(handleInputRequest(QString,QList<quintptr>,QList<int>)), Qt::DirectConnection);
//    connect(vm, SIGNAL(inputArgumentRequest(int,QString,QString,QList<int>)),
//            this, SLOT(handleInputArgumentRequest(int,QString,QString,QList<int>)), Qt::DirectConnection);
//    connect(vm, SIGNAL(outputArgumentRequest(QVariant,QString,QList<int>)),
//            this, SLOT(handleOutputArgumentRequest(QVariant,QString,QList<int>)),
//            Qt::DirectConnection);
//    connect(vm, SIGNAL(outputRequest(QStringList, QList<QVariant::Type>, QVariantList)),
//                       this, SLOT(handleOutputRequest(QStringList, QList<QVariant::Type>, QVariantList)), Qt::DirectConnection);
//    connect(vm, SIGNAL(invokeExternalFunction(QList<quintptr>)),
//            this, SLOT(handleExternalRequest(QList<quintptr>)),
//            Qt::DirectConnection);
//    connect(vm, SIGNAL(beforeExternalFunction()),
//            this, SLOT(prepareExternalCall()), Qt::DirectConnection);
//    connect(vm, SIGNAL(resetModuleRequest(QString)), this, SIGNAL(resetModule(QString)));

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
    e_runMode = RM_StepOver;
    vm->setNextCallStepOver();
    start();
}

void Run::runStepIn()
{
    b_stepDone = false;
    e_runMode = RM_StepIn;
    vm->setNextCallInto();
    start();
}

void Run::runStepOut()
{
    b_stepDone = false;
    b_algDone = false;
    emit lineChanged(-1);
    e_runMode = RM_StepOut;
    vm->setNextCallOut();
    start();
}

void Run::runBlind()
{
    e_runMode = RM_ToEnd;
    vm->setDebugOff(true);
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


bool Run::makeInput(const std::deque<String> & formats, std::deque<Variable> & references)
{
    mutex_interactDone->lock();
    b_interactDone = false;
    list_inputResult.clear();
    mutex_interactDone->unlock();
    QVariantList result;
    String format;
    for (int i=0; i<formats.size(); i++) {
        if (references[i].baseType()==VT_int) {
            Kumir::IO::parseIntFormat(formats[i]);
            format.push_back('i');
        }
        else if (references[i].baseType()==VT_real) {
            Kumir::IO::parseRealFormat(formats[i]);
            format.push_back('r');
        }
        else if (references[i].baseType()==VT_bool) {
            Kumir::IO::parseBoolFormat(formats[i]);
            format.push_back('b');
        }
        else if (references[i].baseType()==VT_char &&
                 references[i].isConstant() &&
                 references[i].value().toChar() == Char('\n'))
        {
            format.push_back('n');
        }
        else if (references[i].baseType()==VT_char) {
            Kumir::IO::parseCharFormat(formats[i]);
            format.push_back('c');
        }
        else if (references[i].baseType()==VT_string) {
            Kumir::IO::parseStringFormat(formats[i]);
            format.push_back('s');
        }
        format += formats[i];
        if (i<formats.size()) format.push_back(';');
        if (Kumir::Core::getError().length()>0)
            return true;
    }
    emit input(QString::fromStdWString(format));
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
    if (mustStop()) {
        return true;
    }
    else {
        Q_ASSERT(result.size()==references.size());
        for (int i=0; i<result.size(); i++) {
            AnyValue val;
            if (references[i].baseType()==VT_int)
                val = result[i].toInt();
            else if (references[i].baseType()==VT_real)
                val = result[i].toDouble();
            else if (references[i].baseType()==VT_bool)
                val = result[i].toBool();
            else if (references[i].baseType()==VT_char)
                val = result[i].toChar().unicode();
            else if (references[i].baseType()==VT_string)
                val = result[i].toString().toStdWString();
            references[i].setValue(val);
        }
    }
    return true;
}

void Run::handleInputArgumentRequest(int localId,
                                     const QString &varName,
                                     const QString &baseFormat,
                                     const QList<int> &bounds)
{
//    QVariantList result;
//    QVariantList localResult;
//    int totalItems = 1;
//    int currentIndex = 0;
//    int dimension = 0;
//    bool inputNext = true;
//    int z = 0;
//    int y = 0;
//    int x = 0;
//    int size0 = 0;
//    int size1 = 0;
//    int size2 = 0;
//    if (bounds.size()==2) {
//        dimension = 1;
//        size0 = bounds[1]-bounds[0]+1;
//        x = bounds[0];
//        totalItems = size0;
//    }
//    else if (bounds.size()==4) {
//        dimension = 2;
//        size0 = bounds[3]-bounds[2]+1;
//        size1 = bounds[1]-bounds[0]+1;
//        x = bounds[0];
//        y = bounds[2];
//        totalItems = size0 * size1;
//    }
//    else if (bounds.size()==6) {
//        dimension = 3;
//        size0 = bounds[5]-bounds[4]+1;
//        size1 = bounds[3]-bounds[2]+1;
//        size2 = bounds[1]-bounds[0]+1;
//        x = bounds[0];
//        y = bounds[2];
//        z = bounds[4];
//        totalItems = size0 * size1 * size2;
//    }
//    forever {
//        if (inputNext) {
//            inputNext = false;
//            mutex_interactDone->lock();
//            b_interactDone = false;
//            list_inputResult.clear();
//            mutex_interactDone->unlock();
//            QString varNameAndIndeces = varName;
//            if (dimension>0) {
//                QStringList indeces;
//                indeces.prepend(QString::number(x+currentIndex%size0));
//                if (dimension>=2)
//                    indeces.prepend(QString::number(y+currentIndex%(size0*size1)));
//                if (dimension==3)
//                    indeces.prepend(QString::number(z+currentIndex%(size0*size1*size2)));
//                varNameAndIndeces += "["+indeces.join(",")+"]";
//            }
//            QString greeting = tr("Please enter %1: ").arg(varNameAndIndeces);
//            emit output(greeting);
//            emit input(baseFormat);
//        }
//        mutex_interactDone->lock();
//        localResult = list_inputResult;
//        mutex_interactDone->unlock();
//        if (localResult.isEmpty()) {
//            msleep(50);
//        }
//        else {
//            result << localResult.at(0);
//            currentIndex++;
//            inputNext = true;
//            if (totalItems==currentIndex) {
//                break;
//            }
//        }
//        if (mustStop())
//            break;
//    }
//    if (mustStop())
//        return;
//    if (dimension==0) {
//        vm->setLocalVariableValue(localId, result.first());
//    }
//    else {
//        vm->setLocalVariableValue(localId, QVariant(result));
//    }
}

void Run::handleOutputArgumentRequest(const QVariant & value,
                                     const QString &varName,
                                     const QList<int> &bounds)
{
//    if (value.type()!=QVariant::List) {
//        QString out = varName + "=";
//        if (!value.isValid())
//            out += tr("undefined");
//        else if (value.type()==QVariant::String)
//            out += "\""+value.toString()+"\"";
//        else if (value.type()==QVariant::Char)
//            out += "'"+value.toString()+"'";
//        else
//            out += value.toString();
//        out += "\n";
//        emit output(out);
//    }
//    else {
//        const QVariantList & list = value.toList();
//        int totalItems = 1;
//        int currentIndex = 0;
//        int dimension = 0;
//        int z = 0;
//        int y = 0;
//        int x = 0;
//        int size0 = 0;
//        int size1 = 0;
//        int size2 = 0;
//        if (bounds.size()==2) {
//            dimension = 1;
//            size0 = bounds[1]-bounds[0]+1;
//            x = bounds[0];
//            totalItems = size0;
//        }
//        else if (bounds.size()==4) {
//            dimension = 2;
//            size0 = bounds[3]-bounds[2]+1;
//            size1 = bounds[1]-bounds[0]+1;
//            x = bounds[0];
//            y = bounds[2];
//            totalItems = size0 * size1;
//        }
//        else if (bounds.size()==6) {
//            dimension = 3;
//            size0 = bounds[5]-bounds[4]+1;
//            size1 = bounds[3]-bounds[2]+1;
//            size2 = bounds[1]-bounds[0]+1;
//            x = bounds[0];
//            y = bounds[2];
//            z = bounds[4];
//            totalItems = size0 * size1 * size2;
//        }
//        for ( ; currentIndex<totalItems; currentIndex ++) {
//            QString varNameAndIndeces = varName;
//            if (dimension>0) {
//                QStringList indeces;
//                indeces.prepend(QString::number(x+currentIndex%size0));
//                if (dimension>=2)
//                    indeces.prepend(QString::number(y+currentIndex%(size0*size1)));
//                if (dimension==3)
//                    indeces.prepend(QString::number(z+currentIndex%(size0*size1*size2)));
//                varNameAndIndeces += "["+indeces.join(",")+"]";
//            }
//            QString out = varNameAndIndeces + "=";
//            const QVariant & val = list[currentIndex];
//            if (!val.isValid())
//                out += tr("undefined");
//            else if (val.type()==QVariant::String)
//                out += "\""+val.toString()+"\"";
//            else if (val.type()==QVariant::Char)
//                out += "'"+val.toString()+"'";
//            else
//                out += val.toString();
//            out += "\n";
//            emit output(out);
//        }
//    }
}

void Run::handleExternalRequest(const QString &pluginName,
                                const QString &functionName,
                                const QVariantList &arguments,
                                const QList<quintptr> &references,
                                const QList<int> &indeces)
{
//    mutex_interactDone->lock();
//    b_interactDone = false;
//    list_funcResults.clear();
//    v_funcResult = QVariant::Invalid;
//    s_funcError = "";
//    mutex_interactDone->unlock();
//    QVariantList result;
//    emit externalFunctionCall(pluginName, functionName, arguments);
//    bool done = false;
//    forever {
//        mutex_interactDone->lock();
//        done = b_interactDone;
//        mutex_interactDone->unlock();
//        if (!done) {
//            msleep(1);
//        }
//        else {
//            break;
//        }
//        if (mustStop())
//            break;
//    }
//    if (mustStop())
//        return;
//    Q_ASSERT(result.size()==references.size());
//    vm->pushValueToStack(v_funcResult);
//    vm->setResults(s_funcError, references, indeces, list_funcResults);
}

void Run::prepareExternalCall()
{
    mutex_interactDone->lock();
    b_interactDone = false;
    mutex_interactDone->unlock();
}

void Run::handleExternalRequest(const QList<quintptr> &references)
{
//    bool done = false;
//    forever {
//        mutex_interactDone->lock();
//        done = b_interactDone;
//        mutex_interactDone->unlock();
//        if (!done) {
//            msleep(1);
//        }
//        else {
//            break;
//        }
//        if (mustStop())
//            break;
//    }
//    if (mustStop())
//        return;
//    Q_ASSERT(list_funcResults.size()==references.size());
//    if (v_funcResult.isValid())
//        vm->pushValueToStack(v_funcResult);
//    v_funcResult = QVariant::Invalid;
//    vm->setResults(s_funcError, references, QList<int>(), list_funcResults);
//    list_funcResults.clear();
//    b_interactDone = false;
//    s_funcError.clear();
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

bool Run::makeOutput(
        const std::deque<String> & formats,
        const std::deque<Variable> & values
        )
{
    Kumir::IO::OutputStream os;
    for (int i=0; i<formats.size(); i++) {
        Kumir::String format = formats[i];
        if (values[i].baseType()==VT_int) {
            Kumir::IO::IntFormat fmt = Kumir::IO::parseIntFormat(format);
            if (Kumir::Core::getError().length()>0) return true;
            Kumir::IO::writeInteger(os, values[i].toInt(), fmt);
        }
        else if (values[i].baseType()==VT_real) {
            Kumir::IO::RealFormat fmt = Kumir::IO::parseRealFormat(format);
            if (Kumir::Core::getError().length()>0) return true;
            Kumir::IO::writeReal(os, values[i].toDouble(), fmt);
        }
        else if (values[i].baseType()==VT_bool) {
            Kumir::IO::BoolFormat fmt = Kumir::IO::parseBoolFormat(format);
            if (Kumir::Core::getError().length()>0) return true;
            Kumir::IO::writeBool(os, values[i].toBool(), fmt);
        }
        else if (values[i].baseType()==VT_char) {
            Kumir::IO::CharFormat fmt = Kumir::IO::parseCharFormat(format);
            if (Kumir::Core::getError().length()>0) return true;
            Kumir::IO::writeChar(os, values[i].toChar(), fmt);
        }
        else if (values[i].baseType()==VT_string) {
            Kumir::IO::StringFormat fmt = Kumir::IO::parseStringFormat(format);
            if (Kumir::Core::getError().length()>0) return true;
            Kumir::IO::writeString(os, values[i].toString(), fmt);
        }
    }
    QString data = QString::fromStdWString(os.getBuffer());
    emit output(data);
    return true;
}

bool Run::noticeOnValueChange(int l, const String & s)
{
    emit marginText(l, QString::fromStdWString(s));
    return true;
}

bool Run::noticeOnLineNoChanged(int lineNo)
{
    mutex_stepDone->lock();
    b_stepDone = true;
    mutex_stepDone->unlock();
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

bool Run::mustStop()
{
    QMutexLocker l1(mutex_stopping);
    QMutexLocker l2(mutex_stepDone);

    if (vm->error().length()>0)
        return true;

    if (b_stopping)
        return true;

    if (e_runMode==RM_StepOut) {
        return b_algDone;
    }
    else if (e_runMode!=RM_ToEnd) {
        return b_stepDone;
    }
    else {
        return false;
    }
}

void Run::handlePauseRequest()
{
    e_runMode = RM_StepOver;
}

void Run::handleAlgorhitmDone(int lineNo)
{
    mutex_algDone->lock();
    b_algDone = true;
    mutex_algDone->unlock();
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
            emit lineChanged(vm->effectiveLineNo());
            emit error(QString::fromStdWString(vm->error()));
            break;
        }
    }
//    bool wasError = vm->error().length()>0;
    // Unclosed files is an error only if program reached end
    bool unclosedFilesIsNotError = b_stopping || vm->hasMoreInstructions();
    // Must close all files if program reached end or user terminated
    bool programFinished = b_stopping || !vm->hasMoreInstructions();
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

void Run::loadProgramFromBinaryBuffer(std::list<char> &stream)
{
    String error;
    if (!vm->loadProgramFromBinaryBuffer(stream, error)) {
        std::string msg;
#if defined(WIN32) || defined(_WIN32)
        msg = Kumir::Coder::encode(Kumir::CP866, error);
#else
        msg = Kumir::Coder::encode(Kumir::UTF8, error);
#endif
        std::cerr << msg << std::endl;
    }
}

void Run::loadProgramFromTextBuffer(const std::string &stream)
{
    String error;
    if (!vm->loadProgramFromTextBuffer(stream, error)) {
        std::string msg;
#if defined(WIN32) || defined(_WIN32)
        msg = Kumir::Coder::encode(Kumir::CP866, error);
#else
        msg = Kumir::Coder::encode(Kumir::UTF8, error);
#endif
        std::cerr << msg << std::endl;
    }
}

QVariant Run::value(quint8 mod, quint16 alg, quint16 var) const
{
    const AnyValue & v = vm->value(mod, alg, var);
    QVariant result;
    if (v.type()==VT_int)
        result = v.toInt();
    else if (v.type()==VT_real)
        result = v.toReal();
    else if (v.type()==VT_char)
        result = QChar(v.toChar());
    else if (v.type()==VT_string)
        result = QString::fromStdWString(v.toString());
    else if (v.type()==VT_bool)
        result = v.toBool();
    return result;
}

QList<int> Run::bounds(quint8 mod, quint16 alg, quint16 var) const
{
    const std::list<int> b = vm->bounds(mod, alg, var);
    return QList<int>::fromStdList(b);
}

QList<int> Run::reference(quint8 mod, quint16 alg, quint16 var) const
{
    // Get triple  { module id, algorhitm id, variable id }
    // for reference target or empty list if not initialized
    ReferenceInfo r = vm->reference(mod, alg, var);
    QList<int> result;
    if (r.valid) {
        result << r.module << r.algorithm << r.variable;
    }
    return result;
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

QVariantList Run::remainingValues() const
{
    QVariantList result;
    std::list<AnyValue> vls = vm->remainingValues();
    for (auto it=vls.begin(); it!=vls.end(); ++it) {
        const AnyValue & var = (*it);
        if (var.type()==VT_void)
            result << QVariant::Invalid;
        else if (var.type()==VT_int)
            result << var.toInt();
        else if (var.type()==VT_real)
            result << var.toReal();
        else if (var.type()==VT_bool)
            result << var.toBool();
        else if (var.type()==VT_char)
            result << QChar(var.toChar());
        else if (var.type()==VT_string)
            result << QString::fromStdWString(var.toString());
    }
    return result;
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


} // namespace KumirCodeRun


