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
    inline Type itemType() const { return _type; }
    inline int numberInTable() const { return _tableNumber; }
    inline TableOfVariables * table() const { return _table; }
    inline const VM::Variable * variable() const { return _variable; }
    inline QVector<int> arrayIndeces() const { return _indeces; }
    QString name() const;
    QString variableTypeName() const;
    QString valueRepresentation() const;
    QString arrayRepresentation() const;
    bool hasValue() const;
    bool isReference() const;
    inline quint64 framePointer() const { return _framePointer; }
    inline void setFramePointer(quint64 p) { _framePointer = p; }
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

    Type _type;
    const VM::Variable * _variable;
    TableOfVariables * _table;
    int _tableNumber;
    QVector<int> _indeces;
    QString _algorithmName;
    quint64 _framePointer;
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
    std::shared_ptr<VM::KumirVM> _vm;
    std::shared_ptr<VM::CriticalSectionLocker> mutex_;
    QHash<QModelIndex, QModelIndex> parents_;
    mutable QList<KumVariableItem*> cache_;
    mutable QHash<KumVariableItem*, QModelIndex> modelIndeces_;
};


} // namespace KumirCodeRun

#endif // KUMIRCODERUN_KUMVARIABLESMODEL_H
