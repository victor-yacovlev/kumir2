#ifndef PYTHON3LANGUAGE_VARIABLESMODEL_H
#define PYTHON3LANGUAGE_VARIABLESMODEL_H

extern "C" {
#include <Python.h>
#include <frameobject.h>
}

#include <QtCore>
#include <QStandardItemModel>
#include <QStandardItem>


#include "pyutils.h"

namespace Python3Language {

class VariablesModelItem
        : public QStandardItem
{
public:
    enum ItemType {
        GlobalsTable, LocalsTable, Variable, Property
    };

    ItemType itemType() const;
    void setItemType(ItemType t);

    QString name() const;
    void setName(const QString &n);

    QString repr() const;
    void setRepr(const QString &r);


private:
    void updateText();

};


class VariablesModel
        : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit VariablesModel(QObject *parent = 0);
    void update(PyFrameObject * currentFrame);
    void resetModel();

Q_SIGNALS:
    void updateGlobalsRequest(const ValueRepresentation &);
    void updateLocalsRequest(const QList<ValueRepresentation> &);
    void clearAllItemsRequest();

private Q_SLOTS:
    void updateGlobals(const ValueRepresentation &source);
    void updateLocals(const QList<ValueRepresentation> & source);
    void clearAllItems();

private:
    static void updateTable(const ValueRepresentation &source, VariablesModelItem * target);


};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_VARIABLESMODEL_H
