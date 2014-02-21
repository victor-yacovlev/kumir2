#include "pascalvariablesmodel.h"

namespace GdbRun {


PascalGlobalsTable::PascalGlobalsTable(const QString & title)
    : QStandardItem()
{
    setText(title);
}

QString PascalVariable::name() const
{
    return data(NAME_ROLE).toString();
}

QString PascalVariable::typee() const
{
    return data(TYPE_ROLE).toString();
}

QVariant PascalVariable::value() const
{
    return data(VALUE_ROLE);
}

PascalVariable::PascalVariable(const QString &name, const QString &type)
    : QStandardItem()
{
    setData(name.toLower(), NAME_ROLE);
    setData(type.toLower(), TYPE_ROLE);
    setData(QVariant::Invalid, VALUE_ROLE);
    updateText();
}

void PascalVariable::updateText()
{
    QString txt = name() + ": " + typee();
    if (value().isValid()) {
        txt += " = " + value().toString();
    }
    setText(txt);
}

void PascalGlobalsTable::addVariable(const QString &name, const QString &type)
{
    appendRow(new PascalVariable(name, type));
}

PascalVariablesModel::PascalVariablesModel(QObject *parent) :
    QStandardItemModel(parent)
{
    globalsTable_ = 0;
}

void PascalVariablesModel::addGlobalVariable(const QString &name, const QString &pascalType)
{
    if (!globalsTable_) {
        globalsTable_ = new PascalGlobalsTable(tr("Globals"));
        globalsTable_->addVariable(name, pascalType);
        QStandardItem * root = invisibleRootItem();
        root->appendRow(globalsTable_);
    }
}

} // namespace GdbRun
