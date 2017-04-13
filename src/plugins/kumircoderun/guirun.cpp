#include "guirun.h"
#include "run.h"
#include "util.h"

#include <kumir2-libs/extensionsystem/pluginmanager.h>
#include <kumir2/actorinterface.h>

#define DO_NOT_DECLARE_STATIC
#include <kumir2-libs/vm/vm.hpp>
#include <kumir2-libs/vm/variant.hpp>

namespace KumirCodeRun {

namespace Gui {


VM::ExternalModuleLoadFunctor::NamesList
ExternalModuleLoadFunctor::operator ()(const std::string & moduleAsciiName,
                                       const Kumir::String & moduleName , Kumir::String * error)
{
    NamesList result;
    Shared::ActorInterface * actor = Util::findActor(moduleAsciiName);
    if (actor) {
        foreach (const Shared::ActorInterface::Function & function, actor->functionList()) {
            result.push_back(std::string(function.asciiName));
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
    , rawBuffer_(QString())
    , rawBufferLastReadChar_(QChar::Null)
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

bool InputFunctor::operator ()(VariableReferencesList references, Kumir::String * error)
{
    // Clear state
    finishedFlag_ = false;
    inputValues_.clear();

    // Prepare input format for GUI
    String format;
    for (int i=0; i<(int)references.size(); i++) {
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
                    Kumir::Core::fromAscii(variable.recordModuleAsciiName())+
                    Kumir::Core::fromAscii("::")+
                    Kumir::Core::fromAscii(variable.recordClassAsciiName())+
                    Kumir::Core::fromAscii("::")+
                    variable.recordClassLocalizedName();
            format.append(typeFullName);
        }
        if (i<(int)references.size()-1) format.push_back(';');
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
        return false; // Do nothing on exit

    // Store input values
    Q_ASSERT(inputValues_.size()==references.size());
    for (int i=0; i<inputValues_.size(); i++) {
        const AnyValue val = Util::QVariantToValue(inputValues_.at(i), 0);
        references[i].setValue(val);
    }
    return true;
}

void InputFunctor::handleInputDone(const QVariantList & values)
{
    finishedMutex_->lock();
    finishedFlag_ = true;
    inputValues_ = values;
    finishedMutex_->unlock();
}

void InputFunctor::clear()
{
    rawBufferLastReadChar_ = QChar(QChar::Null);
    rawBuffer_.clear();
}

void InputFunctor::pushLastCharBack()
{
    rawBuffer_.prepend(rawBufferLastReadChar_);
    rawBufferLastReadChar_ = QChar(QChar::Null);
}

bool InputFunctor::readRawChar(Kumir::Char &ch)
{
    if (rawBuffer_.isEmpty()) {
        // Request for user input to fill buffer
        finishedFlag_ = false;
        inputValues_.clear();
        static const QString format = "w";
            // the same as 's', but different status bar message
        emit requestInput(format);
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

        if (runner_->mustStop() || inputValues_.isEmpty())
            return false;

        rawBuffer_ = inputValues_.first().toString();
        if (rawBuffer_.isEmpty()) {
            return false;
        }
    }
    rawBufferLastReadChar_ = rawBuffer_.at(0);
    rawBuffer_.remove(0, 1);
    ch = Kumir::Char(rawBufferLastReadChar_.unicode());
    return true;
}

void SimulatedInputBuffer::clear()
{
    lastChar_ = prevChar_ = QChar::Null;
}

bool SimulatedInputBuffer::readRawChar(Char &ch)
{
    if (QChar::Null != prevChar_) {
        ch = prevChar_.unicode();
        lastChar_ = prevChar_;
        prevChar_ = QChar::Null;
        return true;
    }
    else {
        const QString s = io_->read(1);
        if (s.isEmpty()) {
            return false;
        }
        else {
            lastChar_ = s[0];
            ch = lastChar_.unicode();
            return true;
        }
    }
}

void SimulatedInputBuffer::pushLastCharBack()
{
    prevChar_ = lastChar_;
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
        FormatsList formats, Kumir::String * error
        )
{
    Kumir::IO::OutputStream os;
    for (int i=0; i<(int)formats.size(); i++) {
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
                svalue = (*converter_)(values[i], error);
                Kumir::IO::writeString(os, svalue, format.first);
            }
        }
        if (error && error->length() > 0) {
            return;
        }
    }
    QString data = QString::fromStdWString(os.getBuffer());
    // Fix fake 152 symbol used by CP1251
    data = data.replace(QChar(152), QChar(' '));
    emit requestOutput(data);
    Util::SleepFunctions::msleep(10);
}

void OutputFunctor::writeRawString(const String &s)
{
    const QString data = QString::fromStdWString(s);
    emit requestOutput(data);
    Util::SleepFunctions::msleep(10);
}

void SimulatedOutputBuffer::writeRawString(const String &s)
{
    const QString data = QString::fromStdWString(s);
    *io_ << data;
    io_->flush();
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

void GetMainArgumentFunctor::operator ()(Variable & reference, Kumir::String * error)
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
    else if (VT_record == reference.baseType()
             && 0==reference.recordModuleAsciiName().length()
             && "file"==reference.recordClassAsciiName())
        format.push_back('f');
    else if (reference.baseType()==VT_record) {
        const String typeFullName =
                Kumir::Core::fromAscii(reference.recordModuleAsciiName())+
                Kumir::Core::fromAscii("::")+
                Kumir::Core::fromAscii(reference.recordClassAsciiName())+
                Kumir::Core::fromAscii("::")+
                reference.recordClassLocalizedName();
        format.append(QString::fromStdWString(typeFullName));
    }

    if (reference.dimension()==0) {
        AnyValue val;
        if (inputScalarArgument(QString::fromStdWString(reference.name()), "","",format,val))
            reference.setValue(val);
    }
    else if (reference.dimension()==1) {
        int bounds[7];
        reference.getEffectiveBounds(bounds);
        for (int x=bounds[0]; x<=bounds[1]; x++) {
            AnyValue val;
            if (inputScalarArgument(QString::fromStdWString(reference.name()), QString::fromLatin1("[%1]: ").arg(x),"",format,val))
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
                if (inputScalarArgument(QString::fromStdWString(reference.name()), QString::fromLatin1("[%1,%2]: ").arg(y).arg(x),"",format,val))
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
                    if (inputScalarArgument(QString::fromStdWString(reference.name()), QString::fromLatin1("[%1,%2,%3]: ").arg(z).arg(y).arg(x),"",format,val))
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
        const QString & name,
        const QString & indeces,
        const QString & messageSuffix,
        const QString & format,
        AnyValue &val
        )
{
    QString message;

    if ("f" == format) {
        static const QString FileNameSuffix = QString::fromUtf8(" (имя файла)");
        static const QString FileModeSuffix = QString::fromUtf8(" (режим открытия: [ч]тение, [з]апись, [д]обавление)");

        AnyValue fileNameValue;
        QString fileName;
        bool inputNameSuccess = false;
        Q_FOREVER {
            inputNameSuccess = inputScalarArgument(name, indeces, FileNameSuffix, "s", fileNameValue);
            if (!inputNameSuccess)
                return false;
            fileName = QString::fromStdWString(fileNameValue.toString());
            if (!QFile::exists(fileName)) {
                emit requestOutput(trUtf8("Файл %1 не существует. Введите имя еще раз.\n").arg(fileName));
            }
            else {
                break;
            }
        }

        AnyValue fileModeValue;
        QString fileMode;
        static const QStringList ReadModes = QStringList()
                << "read" << "r"
                << QString::fromUtf8("чтение")
                << QString::fromUtf8("ч");
        static const QStringList WriteModes = QStringList()
                << "write" << "w"
                << QString::fromUtf8("запись")
                << QString::fromUtf8("з");
        static const QStringList AppendModes = QStringList()
                << "append" << "a"
                << QString::fromUtf8("добавление")
                << QString::fromUtf8("д");
        bool inputModeSuccess = false;
        Q_FOREVER {
            inputModeSuccess = inputScalarArgument(name, indeces, FileModeSuffix, "s", fileModeValue);
            if (!inputModeSuccess)
                return false;
            fileMode = QString::fromStdWString(fileModeValue.toString());
            Kumir::FileType::OpenMode openMode = Kumir::FileType::NotOpen;
            if (ReadModes.contains(fileMode.trimmed().toLower()))
                openMode = Kumir::FileType::Read;
            if (WriteModes.contains(fileMode.trimmed().toLower()))
                openMode = Kumir::FileType::Write;
            if (AppendModes.contains(fileMode.trimmed().toLower()))
                openMode = Kumir::FileType::Append;
            if (Kumir::FileType::NotOpen == openMode) {
                emit requestOutput(trUtf8("Неверный режим доступа. Введите еще раз.\n"));
            }
            else {
                QFileInfo fi(fileName);
                bool enoughtPermissions = false;
                if (Kumir::FileType::Write == openMode || Kumir::FileType::Append == openMode)
                    enoughtPermissions = fi.isWritable();
                else if (Kumir::FileType::Read == openMode)
                    enoughtPermissions = fi.isReadable();
                if (!enoughtPermissions) {
                    emit requestOutput(trUtf8("Недостаточно прав для доступа к файлу. Попробуйте другой режим.\n"));
                }
                else {
                    Kumir::FileType file = Kumir::Files::open(fileName.toStdWString(), openMode, false, 0);
                    VM::Record fileRecord = KumirVM::toRecordValue(file);
                    val = AnyValue(fileRecord);
                    break;
                }
            }
        }
        return true;
    }


    message = trUtf8("Введите %1%2%3: ").arg(name).arg(indeces).arg(messageSuffix);


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

    val = Util::QVariantToValue(inputValues_.at(0), 0);

    return true;
}

void ReturnMainValueFunctor::operator()(const Variable & reference, Kumir::String * error)
{
    if (!reference.isValid())
        return;
    QString repr;
    emit requestOutput(QString::fromStdWString(reference.name())+" = ");
    if (reference.dimension()==0) {
        if (reference.hasValue()) {
            const AnyValue & value = reference.value();
            const std::wstring valueRepr = value.toString();
            repr = QString::fromStdWString(valueRepr);
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


PauseFunctor::PauseFunctor()
    : QObject()
    , VM::PauseFunctor()
{
}

void PauseFunctor::operator ()()
{
    emit requestPause();
}

DelayFunctor::DelayFunctor()
    : QThread()
    , VM::DelayFunctor()
    , stopFlag_(false)
{
    stopMutex_.reset(new QMutex);
}

void DelayFunctor::stop()
{
    stopMutex_->lock();
    stopFlag_ = true;
    stopMutex_->unlock();
}

void DelayFunctor::operator ()(quint32 msec)
{
    stopMutex_->lock();
    stopFlag_ = false;
    stopMutex_->unlock();
    static const quint32 QuantSize = 250u;
    const quint32 quantsCount = msec / QuantSize;
    const quint32 lastQuantSize = msec - (quantsCount * QuantSize);
    bool mustBreak = false;
    for (quint32 i=0; i<quantsCount; i++) {
        stopMutex_->lock();
        mustBreak = stopFlag_;
        stopMutex_->unlock();
        if (mustBreak)
            break;
        msleep(QuantSize);
    }
    if (!mustBreak) {
        msleep(lastQuantSize);
    }
}

ExternalModuleResetFunctor::ExternalModuleResetFunctor()
    : QObject()
    , VM::ExternalModuleResetFunctor()
{
}

void ExternalModuleResetFunctor::operator ()(const std::string & moduleName, const String & localizedName, Kumir::String * error)
{
    using namespace Shared;
    using namespace ExtensionSystem;

    ActorInterface * actor = Util::findActor(moduleName);

    if (actor) {
        actor->reset();
        if (callFunctor_) {
            callFunctor_->checkForActorConnected(moduleName);
        }
    }
    else {
        const QString qModuleName = QString::fromStdWString(localizedName);
        const Kumir::String errorMessage =
                QString::fromUtf8(
                    "Ошибка инициализации исполнителя: нет исполнителя "
                    "с именем %1"
                    ).arg(qModuleName).toStdWString();
        if (error) {
            error->assign(errorMessage);
        }
        return;
    }

    emit showActorWindow(QByteArray(moduleName.c_str()));
}

}

}
