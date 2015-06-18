#include "variablesmodel.h"
#include "pyutils.h"

#include <QFont>

namespace Python3Language {

void VariablesModelItem::setItemType(VariablesModelItem::ItemType t)
{
    setData(QVariant(quint32(t)), Qt::UserRole+1);
    QFont fnt = font();
    fnt.setBold(GlobalsTable==t || LocalsTable==t);
    setFont(fnt);
    updateText();
}

VariablesModelItem::ItemType VariablesModelItem::itemType() const
{
    return ItemType(data(Qt::UserRole+1).toUInt());
}

void VariablesModelItem::setName(const QString &n)
{
    setData(n, Qt::UserRole+2);
    updateText();
}

QString VariablesModelItem::name() const
{
    return data(Qt::UserRole+2).toString();
}

void VariablesModelItem::setRepr(const QString &r)
{
    setData(r, Qt::UserRole+3);
    updateText();
}

QString VariablesModelItem::repr() const
{
    return data(Qt::UserRole+3).toString();
}

void VariablesModelItem::updateText()
{
    ItemType t = itemType();
    if (GlobalsTable==t || LocalsTable==t)
        setText(name());
    else
        setText(name() + " = " + repr());
}

VariablesModel::VariablesModel(QObject *parent)
    : QStandardItemModel(parent)
{
    // QStandardItemModel is not thread-safe!!!
    // Use SIGNAL/SLOT bridge to make actual changes in GUI thread

    connect(this, SIGNAL(updateGlobalsRequest(ValueRepresentation)),
            this, SLOT(updateGlobals(ValueRepresentation)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(clearAllItemsRequest()),
            this, SLOT(clearAllItems()),
            Qt::QueuedConnection);
    connect(this, SIGNAL(updateLocalsRequest(QList<ValueRepresentation>)),
            this, SLOT(updateLocals(QList<ValueRepresentation>)),
            Qt::QueuedConnection);
}

void VariablesModel::resetModel()
{
    Q_EMIT clearAllItemsRequest();
}

void VariablesModel::clearAllItems()
{
    clear();
}

void VariablesModel::update(PyFrameObject *currentFrame)
{
    // Update globals
    ValueRepresentation globals = PyObjectToValueRepresentation("", currentFrame->f_globals);
    Q_EMIT updateGlobalsRequest(globals);

    // Update all frames locals
    QList<ValueRepresentation> localsList;

    while (currentFrame) {
        PyFrame_FastToLocals(currentFrame);
        if (currentFrame->f_locals && currentFrame->f_globals!=currentFrame->f_locals) {
            ValueRepresentation locals = PyObjectToValueRepresentation("", currentFrame->f_locals);
            PyObject * py_name = currentFrame->f_code->co_name;
            QString funcName = PyUnicodeToQString(py_name);
            locals.name = funcName;
            localsList.append(locals); // topmost frame at begin
        }
        currentFrame = currentFrame->f_back;
    }

    Q_EMIT updateLocalsRequest(localsList);
}

void VariablesModel::updateGlobals(const ValueRepresentation &source)
{
    VariablesModelItem * target = 0;
    int targetIndex = -1;
    for (int i=0; i<rowCount(); i++) {
        VariablesModelItem * it = dynamic_cast<VariablesModelItem*>(item(i));
        Q_ASSERT(it);
        if (VariablesModelItem::GlobalsTable==it->itemType()) {
            target = it;
            targetIndex = i;
            break;
        }
    }
    if (source.children.isEmpty() && target) {
        // Remove unneccessary "Globals" root
        QList<QStandardItem*> garbage = takeRow(targetIndex);
        Q_FOREACH(QStandardItem * garbageItem, garbage)
            delete garbageItem;
    }
    else if (!source.children.isEmpty() && !target) {
        // Create globals table
        target = new VariablesModelItem;
        target->setItemType(VariablesModelItem::GlobalsTable);
        target->setName(tr("Globals"));
        insertRow(0, target);
    }

    if (target) {
        // Update globals table
        updateTable(source, target);
    }
}

void VariablesModel::updateLocals(const QList<ValueRepresentation> &source)
{
    typedef QPair<VariablesModelItem*,int> ModelIndex;
    QList<ModelIndex> target;
    for (int i=0; i<rowCount(); i++) {
        VariablesModelItem* localsCandidate =
                dynamic_cast<VariablesModelItem*>(item(i));
        if (VariablesModelItem::LocalsTable==localsCandidate->itemType()) {
            target.append(ModelIndex(localsCandidate,i));
        }
    }

    // Remove extra tables from traceback
    while (target.size() > source.size()) {
        int index = target.first().second;
        QList<QStandardItem*> garbage = takeRow(index);
        Q_FOREACH(QStandardItem * garbageItem, garbage)
            delete garbageItem;
        target.pop_front();
    }

    // Add extra tables
    while (source.size() > target.size()) {
        VariablesModelItem * locals = new VariablesModelItem;
        locals->setItemType(VariablesModelItem::LocalsTable);
        int index = -1;
        if (target.isEmpty()) {
            index = rowCount();
            appendRow(locals);
        }
        else {
            int nextIndex = target.first().second;
            index = nextIndex - 1;
            insertRow(nextIndex, locals);
        }
        target.prepend(ModelIndex(locals, index));
    }

    // Update values
    Q_ASSERT(source.size() == target.size());
    for (int i=0; i<source.size(); i++) {
        VariablesModelItem * targetItem = target[i].first;
        const ValueRepresentation & sourceItem = source[i];
        targetItem->setName(tr("'%1' Locals").arg(sourceItem.name));
        updateTable(sourceItem, targetItem);
    }
}

void VariablesModel::updateTable(const ValueRepresentation &source, VariablesModelItem *target)
{
    QStringList names;
    Q_FOREACH(const ValueRepresentation & child, source.children) {
        names.append(child.name);
    }

    // Remove old entries from target or update existing
    for (int i=target->rowCount()-1; i>=0; i--) {
        VariablesModelItem * it = dynamic_cast<VariablesModelItem*>(target->child(i));
        const QString name = it->name();
        if (!names.contains(name)) {
            QList<QStandardItem*> garbage = it->takeRow(i);
            Q_FOREACH(QStandardItem * garbageItem, garbage)
                delete garbageItem;
        }
        else {
            Q_FOREACH(const ValueRepresentation & child, source.children) {
                if (child.name==name) {
                    it->setRepr(child.repr);
                    if (child.children.size() > 0) {
                        updateTable(child, it);
                    }
                    names.removeAll(name);
                    break;
                }
            }
        }
    }

    // Add new entries
    Q_FOREACH(const QString & name, names) {
        if (!name.startsWith("_")) {
            VariablesModelItem * it = new VariablesModelItem;
            it->setItemType(
                        VariablesModelItem::GlobalsTable==target->itemType() ||
                        VariablesModelItem::LocalsTable==target->itemType()
                        ? VariablesModelItem::Variable
                        : VariablesModelItem::Property
                          );
            it->setName(name);
            Q_FOREACH(const ValueRepresentation & child, source.children) {
                if (child.name==name) {
                    it->setRepr(child.repr);
                    if (child.children.size() > 0) {
                        updateTable(child, it);
                    }
                }
            }
            target->appendRow(it);
        }
    }
}



} // namespace Python3Language
