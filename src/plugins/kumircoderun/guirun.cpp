#include "guirun.h"
#include "run.h"
#include "util.h"

namespace KumirCodeRun {

namespace Gui {


VM::ExternalModuleLoadFunctor::NamesList
ExternalModuleLoadFunctor::operator ()(const String & moduleName, const std::string&)
{
    // In GUI mode this functor should not throw in any case
    NamesList result;
    Shared::ActorInterface * actor = Util::findActor(moduleName);
    if (actor) {
        foreach (const QString & name, actor->funcList()) {
            result.push_back(name.toStdWString());
        }
    }
    return result;
}


InputFunctor::InputFunctor()
    : QObject(0)
    , finishedFlag_(false)
    , finishedMutex_(new QMutex)
    , inputValues_(QVariantList())
    , converter_(nullptr)
    , runner_(nullptr)
{

}

void InputFunctor::setCustomTypeFromStringFunctor(
        VM::CustomTypeFromStringFunctor *f)
{
    converter_ = f;
}

void InputFunctor::setRunnerInstance(Run *runner)
{
    runner_ = runner;
    connect(runner_, SIGNAL(finishInput(QVariantList)),
            this, SLOT(handleInputDone(QVariantList)));
    connect(this, SIGNAL(requestInput(QString)),
            runner_, SIGNAL(input(QString)));
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
            Util::SleepFunctions::msleep(1);
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
        const AnyValue val = Util::QVariantToValue(inputValues_.at(i), 0);
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

OutputFunctor::OutputFunctor()
    : QObject(0)
    , converter_(nullptr)
{
}

void OutputFunctor::setCustomTypeToStringFunctor(
        VM::CustomTypeToStringFunctor *f)
{
    converter_ = f;
}

void OutputFunctor::setRunnerInstance(Run *runner)
{
    connect(this, SIGNAL(requestOutput(QString)),
            runner, SIGNAL(output(QString)));
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
    Util::SleepFunctions::usleep(100);
}

GetMainArgumentFunctor::GetMainArgumentFunctor()
    : QObject(0)
    , finishedFlag_(false)
    , finishedMutex_(new QMutex)
    , inputValues_(QVariantList())
    , converter_(nullptr)
    , runner_(nullptr)
{

}

void GetMainArgumentFunctor::setCustomTypeFromStringFunctor(
        VM::CustomTypeFromStringFunctor *f)
{
    converter_ = f;
}

void GetMainArgumentFunctor::setRunnerInstance(Run *runner)
{
    runner_ = runner;
    connect(runner, SIGNAL(finishInput(QVariantList)),
            this, SLOT(handleInputDone(QVariantList)));
    connect(this, SIGNAL(requestInput(QString)),
            runner, SIGNAL(input(QString)));
    connect(this, SIGNAL(requestOutput(QString)),
            runner, SIGNAL(output(QString)));
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
    finishedMutex_->lock();
    finishedFlag_ = true;
    inputValues_ = values;
    finishedMutex_->unlock();
}


ReturnMainValueFunctor::ReturnMainValueFunctor()
    : QObject(0)
    , converter_(nullptr)
{
}

void ReturnMainValueFunctor::setCustomTypeToStringFunctor(
        VM::CustomTypeToStringFunctor *f)
{
    converter_ = f;
}

void ReturnMainValueFunctor::setRunnerInstance(Run *runner)
{
    connect(this, SIGNAL(requestOutput(QString)),
            runner, SIGNAL(output(QString)));
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
            Util::SleepFunctions::msleep(1);
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



}

}
