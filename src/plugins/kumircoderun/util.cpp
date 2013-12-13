#include "util.h"

#include "extensionsystem/pluginmanager.h"

namespace KumirCodeRun {
namespace Util {

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

AnyValue QVariantToValue(const QVariant & var, int dim)
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
            aval = AnyValue(wchar_t(var.toChar().unicode()));
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

ActorInterface* findActor(const std::string & moduleAsciiName)
{
    const QByteArray qModuleName = QByteArray(moduleAsciiName.c_str());
    return findActor(qModuleName);
}

ActorInterface* findActor(const QByteArray & qModuleName)
{
    using namespace ExtensionSystem;

    const QList<KPlugin*> actorPlugins =
            PluginManager::instance()->loadedPlugins("Actor*");

    ActorInterface * actor = nullptr;
    foreach ( KPlugin * p, actorPlugins ) {
        actor = qobject_cast<ActorInterface*>(p);
        if (actor && actor->asciiModuleName()==qModuleName) {
            break;
        }
        actor = nullptr;
    }

    return actor;
}


}} // namespaces

