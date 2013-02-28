#include <cstdlib>
#include "stdlib/kumirstdlib.hpp"
#include "vm/variant.hpp"
#include "vm/vm.hpp"
#include "run.h"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"

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
    mutex_vm = nullptr;
    programLoaded = false;
    vm = new KumirVM();
    mutex_vm = new Mutex;
    vm->setMutex(mutex_vm);

    i_originFunctionDeep = 0;
    b_interactDone = b_stopping = b_stepDone = b_algDone = false;
    mutex_stopping = new QMutex;
    mutex_stepDone = new QMutex;
    mutex_algDone = new QMutex;
    mutex_interactDone = new QMutex;
    e_runMode = RM_ToEnd;

    vm->setExternalHandler(this);
    connect(this, SIGNAL(externalFunctionCall(QString,quint16,QVariantList)),
            this, SLOT(handleExternalFunctionCall(QString,quint16,QVariantList))
            , Qt::DirectConnection
            );
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

void Run::lockVMMutex()
{
    mutex_vm->lock();
}

void Run::unlockVMMutex()
{
    mutex_vm->unlock();
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
    b_stopping = false;
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


bool Run::inputScalarArgument(const QString &message, const QString & format, AnyValue &val)
{
    emit output(message);
    mutex_interactDone->lock();
    list_inputResult.clear();
    mutex_interactDone->unlock();
    QVariantList result;
    emit input(format);
    forever {
        mutex_interactDone->lock();
        result = list_inputResult;
        mutex_interactDone->unlock();
        if (result.isEmpty())
            msleep(50);
        else
            break;
        if (mustStop())
            return false;
    }
    if      (format[0]=='i')
        val = result[0].toInt();
    else if (format[0]=='r')
        val = result[0].toDouble();
    else if (format[0]=='b')
        val = result[0].toBool();
    else if (format[0]=='c')
        val = result[0].toChar().unicode();
    else if (format[0]=='s')
        val = result[0].toString().toStdWString();
    return true;
}


bool Run::makeInputArgument(Variable & reference)
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
        if (inputScalarArgument(tr("Please enter %1: ").arg(QString::fromStdWString(reference.name())),format,val))
            reference.setValue(val);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            AnyValue val;
            if (inputScalarArgument(tr("Please enter %1[%2]: ").arg(QString::fromStdWString(reference.name())).arg(x),format,val))
                reference.setValue(x,val);
            else
                return true;
        }
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                AnyValue val;
                if (inputScalarArgument(tr("Please enter %1[%2,%3]: ").arg(QString::fromStdWString(reference.name())).arg(y).arg(x),format,val))
                    reference.setValue(y,x,val);
                else
                    return true;
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
                    if (inputScalarArgument(tr("Please enter %1[%2,%3,%4]: ").arg(QString::fromStdWString(reference.name())).arg(z).arg(y).arg(x),format,val))
                        reference.setValue(z,y,x,val);
                    else
                        return true;
                }
            }
        }
    }
    return true;
}

bool Run::makeOutputArgument(const Variable & reference) {
    if (!reference.isValid())
        return true;
    QString repr;
    emit output(QString::fromStdWString(reference.name())+" = ");
    if (reference.dimension()==0) {
        if (reference.hasValue()) {
            repr = QString::fromStdWString(reference.value().toString());
            if (reference.baseType()==Bytecode::VT_string)
                repr = "\"" + repr + "\"";
            else if (reference.baseType()==Bytecode::VT_char)
                repr = "'" + repr + "'";
        }
        emit output(repr);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        emit output("{ ");
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            repr = "";
            if (reference.hasValue(x)) {
                repr = QString::fromStdWString(reference.value(x).toString());
                if (reference.baseType()==Bytecode::VT_string)
                    repr = "\"" + repr + "\"";
                else if (reference.baseType()==Bytecode::VT_char)
                    repr = "'" + repr + "'";
            }
            emit output(repr);
            if (x<bounds[1]) {
                emit output(", ");
            }
        }
        emit output(" }");
    }
    else if (reference.dimension()==2) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        emit output("{ ");
        for (int y=bounds[0]; y<=bounds[1]; y++) {
            emit output("{ ");
            for (int x=bounds[2]; x<=bounds[3]; x++) {
                repr = "";
                if (reference.hasValue(y,x)) {
                    repr = QString::fromStdWString(reference.value(y,x).toString());
                    if (reference.baseType()==Bytecode::VT_string)
                        repr = "\"" + repr + "\"";
                    else if (reference.baseType()==Bytecode::VT_char)
                        repr = "'" + repr + "'";
                }
                emit output(repr);
                if (x<bounds[1]) {
                    emit output(", ");
                }
            }
            emit output(" }");
            if (y<bounds[1]) {
                emit output(", ");
            }
        }
        emit output(" }");
    }
    else if (reference.dimension()==3) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        emit output("{ ");
        for (int z=bounds[0]; z<=bounds[1]; z++) {
            emit output("{ ");
            for (int y=bounds[2]; y<=bounds[3]; y++) {
                emit output("{ ");
                for (int x=bounds[4]; x<=bounds[5]; x++) {
                    repr = "";
                    if (reference.hasValue(z,y,x)) {
                        repr = QString::fromStdWString(reference.value(z,y,x).toString());
                        if (reference.baseType()==Bytecode::VT_string)
                            repr = "\"" + repr + "\"";
                        else if (reference.baseType()==Bytecode::VT_char)
                            repr = "'" + repr + "'";
                    }
                    emit output(repr);
                    if (x<bounds[1]) {
                        emit output(", ");
                    }
                }
                emit output(" }");
                if (y<bounds[1]) {
                    emit output(", ");
                }
            }
            emit output(" }");
            if (z<bounds[1]) {
                emit output(", ");
            }
        }
        emit output(" }");
    }
    emit output("\n");
    return true;
}


bool Run::makeInput(std::deque<Variable> & references)
{
    mutex_interactDone->lock();
    b_interactDone = false;
    list_inputResult.clear();
    mutex_interactDone->unlock();
    QVariantList result;
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
            else if (references[i].baseType()==VT_record) {
                const QVariantList alist = result[i].toList();
                Record record;
                for (int j=0; j<alist.size(); j++) {
                    const QVariant & aval = alist[j];
                    if (aval.type()==QVariant::Int)
                        record.fields.push_back(AnyValue(aval.toInt()));
                    else if (aval.type()==QVariant::Double)
                        record.fields.push_back(AnyValue(Kumir::real(aval.toDouble())));
                    else if (aval.type()==QVariant::Bool)
                        record.fields.push_back(AnyValue(aval.toBool()));
                    else if (aval.type()==QVariant::Char)
                        record.fields.push_back(AnyValue(Kumir::Char(aval.toChar().unicode())));
                    else if (aval.type()==QVariant::String)
                        record.fields.push_back(AnyValue(aval.toString().toStdWString()));
                }
                val = AnyValue(record);
            }
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

bool Run::evaluateExternalFunction(
        /* IN */      const String & moduleName,
        /* IN */      uint16_t functionId,
        /* IN */      const std::deque<Variable> & arguments,
        /* OUT */     Variable & result,
        /* OUT */     String & moduleRuntimeError
        )
{
    mutex_interactDone->lock();
    b_interactDone = false;
    list_funcResults.clear();
    v_funcResult.clear();
    s_funcError = "";
    mutex_interactDone->unlock();

    // Prepare arguments
    QVariantList qArgs;
    typedef std::deque<Variable>::const_iterator CIt;
    typedef std::deque<Variable>::iterator It;
    for (CIt i=arguments.begin(); i!=arguments.end() ; ++i) {
        const Variable & arg = (*i);
        qArgs << VariableToQVariant(arg);
    } // end preparing arguments

    // Call Signal/Slot threading bridge to Run::handleExternalFunctionCall(...)
    emit externalFunctionCall(QString::fromStdWString(moduleName), functionId, qArgs);

    // Wait for actor thread finishes...
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
    // Return a flag "processed" in case of User-termination to avoid runtime error
    if (mustStop())
        return true;

    // Set function result if need
    result = Variable(QVariantToValue(v_funcResult, 0));


    // TODO implement out-parameters

    // Set error
    moduleRuntimeError = s_funcError.toStdWString();

    // Return a flag "processed"
    return true;
}

void Run::handleExternalFunctionCall(
        const QString &moduleName
        , quint16 algId
        , const QVariantList &arguments
        )
/* Signal/Slot threading bridge */
{
    QList<ExtensionSystem::KPlugin*> plugins =
            ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
    Shared::ActorInterface * actor = 0;
    for (int i=0; i<plugins.size(); i++) {
        actor = qobject_cast<Shared::ActorInterface*>(plugins[i]);
        if (actor) {
            if (actor->name()==moduleName)
                break;
            else
                actor = 0;
        }
    }
    if (actor) {
        Shared::EvaluationStatus status = actor->evaluate(algId, arguments);
        QMutexLocker locker(mutex_interactDone);
        if (status==Shared::ES_Error)
            s_funcError = actor->errorText();
        if (status==Shared::ES_StackResult || status==Shared::ES_StackRezResult)
            v_funcResult = actor->result();
        if (status==Shared::ES_RezResult || status==Shared::ES_StackRezResult)
            list_funcResults = actor->algOptResults();
        b_interactDone = true;
    }
    else {
        QMutexLocker locker(mutex_interactDone);
        b_interactDone = true;
    }
}

bool Run::convertExternalTypeFromString(
        const String &stringg, Variable &variable, bool &ok
        )
{
    const QString modName =
            QString::fromStdWString(variable.recordModuleName());
    const QString className =
            QString::fromStdWString(variable.recordClassName());
    const QString qString = QString::fromStdWString(stringg);

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
        ok = value.isValid();
        if (ok) {
            variable.setValue(QVariantToValue(value, 0));
        }
        return true;
    }
    else {
        return false;
    }
}

bool Run::convertExternalTypeToString(const Variable &variable, String &out)
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
    if (actor) {
        Shared::ActorInterface::CustomType ct;
        ct.first = className;
        QVariant value = VariableToQVariant(variable);
        const QString qString = actor->customValueToString(ct, value);
        out = qString.toStdWString();
        return true;
    }
    else {
        return false;
    }
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
//    QMutexLocker l(mutex_interactDone);
//    b_interactDone = true;
//    list_funcResults = results;
//    v_funcResult = retval;
//    s_funcError = error;
}

bool Run::makeOutput(
        const std::deque< std::pair<int,int> > & formats,
        const std::deque<Variable> & values
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
            convertExternalTypeToString(values[i], svalue);
            Kumir::IO::writeString(os, svalue, format.first);
        }
    }
    QString data = QString::fromStdWString(os.getBuffer());
    emit output(data);
    usleep(100);
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

bool Run::loadExternalModule(
          const String &moduleName
        , const String &fileName
        , std::list<String> &availableMethods
     )
{
    // TODO implement loading modules on demand
    QList<ExtensionSystem::KPlugin*> plugins =
            ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
    bool found = false;
    for (int i=0; i<plugins.size(); i++) {
        Shared::ActorInterface * actor = qobject_cast<Shared::ActorInterface*>(plugins[i]);
        if (actor && actor->name()==QString::fromStdWString(moduleName)) {
            found = true;
            for (int j=0; j<actor->funcList().size(); j++) {
                availableMethods.push_back(actor->funcList()[j].toStdWString());
            }
            break;
        }
    }
    return found;
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
            int lineNo = vm->effectiveLineNo();
            emit lineChanged(lineNo);
            emit error(QString::fromStdWString(vm->error()));
            break;
        }
    }
//    bool wasError = vm->error().length()>0;
    // Unclosed files is an error only if program reached end
    bool unclosedFilesIsNotError = b_stopping || vm->hasMoreInstructions();
    // Must close all files if program reached end or user terminated
    bool programFinished = b_stopping || !vm->hasMoreInstructions() || vm->error().length();
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


} // namespace KumirCodeRun


