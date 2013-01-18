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
        if (var.baseType()==VT_int)
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
            for (int j=0; j<record.size(); j++) {
                const VM::AnyValue & field = record[j];
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
            aval = AnyValue(VT_record);
            Record & record = aval.toRecord();
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
                record.push_back(fieldVal);
            }
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
        /* OUT */     std::deque<Variable> & outArguments,
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

    // Set function OUT-parameters if need
    Q_ASSERT(list_funcResults.size()==outArguments.size());
    for (int i=0; i<list_funcResults.size(); i++) {
        const QVariant & value = list_funcResults.at(i);
        outArguments[i].setValue(QVariantToValue(value, 0));
    }

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
    }
    QString data = QString::fromStdWString(os.getBuffer());
    emit output(data);
    msleep(10);
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
    for (std::list<AnyValue>::const_iterator it=vls.begin(); it!=vls.end(); ++it) {
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


