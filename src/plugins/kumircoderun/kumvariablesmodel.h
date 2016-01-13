#ifndef KUMIRCODERUN_KUMVARIABLESMODEL_H
#define KUMIRCODERUN_KUMVARIABLESMODEL_H

#define DO_NOT_DECLARE_STATIC
#include "vm/vm.hpp"


#include <QAbstractItemModel>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QHash>

namespace KumirCodeRun {

typedef const std::vector<VM::Variable> TableOfVariables;

class KumVariableItem {
    friend class KumVariablesModel;
public:
    enum Type {
        GlobalsTable, LocalsTable, Variable, ArrayItem
    };
    inline Type itemType() const { return type_; }
    inline int numberInTable() const { return tableNumber_; }
    inline TableOfVariables * table() const { return table_; }
    inline const VM::Variable * variable() const { return variable_; }
    inline QVector<int> arrayIndeces() const { return indeces_; }
    QString name() const;
    QString variableTypeName() const;
    QString valueRepresentation() const;
    QString arrayRepresentation() const;
    bool hasValue() const;
    bool isReference() const;
    inline quint64 framePointer() const { return framePointer_; }
    inline void setFramePointer(quint64 p) { framePointer_ = p; }
private:
    explicit KumVariableItem(TableOfVariables * table, int row);
    explicit KumVariableItem(TableOfVariables * table, int row,
                             const QString & name);
    explicit KumVariableItem(const VM::Variable * variable, int row,
                             TableOfVariables * table);
    explicit KumVariableItem(const VM::Variable * variable, int row,
                             const QVector<int> & indeces);

    QString array1Representation(const QVector<int> & indeces, int maxItems, int & readItems) const;
    QString array2Representation(const QVector<int> & indeces, int maxItems, int & readItems) const;
    QString array3Representation(const QVector<int> & indeces, int maxItems, int & readItems) const;

    Type type_;
    const VM::Variable * variable_;
    TableOfVariables * table_;
    int tableNumber_;
    QVector<int> indeces_;
    QString algorithmName_;
    quint64 framePointer_;
};

class KumVariablesModel : public QAbstractItemModel
{
    friend class Run;
    Q_OBJECT    
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
signals:
    
public slots:

protected:
    explicit KumVariablesModel(std::shared_ptr<VM::KumirVM> vm,
                               std::shared_ptr<VM::CriticalSectionLocker> mutex,
                               QObject *parent = 0);

    QModelIndex valueIndex(int row, int column, TableOfVariables * table) const;
    QModelIndex arrayIndex(int row, int column, const VM::Variable * variable, const QVector<int> & prevIndeces) const;
    QModelIndex topLevelIndex(int row) const;

    void clear();
    void emitValueChanged(const VM::Variable & variable, const QVector<int> & indeces);

private:
    std::shared_ptr<VM::KumirVM> vm_;
    std::shared_ptr<VM::CriticalSectionLocker> mutex_;
    QHash<QModelIndex, QModelIndex> parents_;
    mutable QList<KumVariableItem*> cache_;
    mutable QHash<KumVariableItem*, QModelIndex> modelIndeces_;
};


} // namespace KumirCodeRun

#endif // KUMIRCODERUN_KUMVARIABLESMODEL_H
