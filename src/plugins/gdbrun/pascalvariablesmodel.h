#ifndef GDBRUN_PASCALVARIABLESMODEL_H
#define GDBRUN_PASCALVARIABLESMODEL_H

#include <QStandardItemModel>
#include <QStandardItem>

namespace GdbRun {

class PascalVariable
        : public QStandardItem
{
public:
    explicit PascalVariable(const QString & name, const QString & type);
    QString name() const;
    QString typee() const;
    QVariant value() const;
private:
    void updateText();
    static const int NAME_ROLE = Qt::UserRole + 1;
    static const int TYPE_ROLE = Qt::UserRole + 2;
    static const int VALUE_ROLE = Qt::UserRole + 3;
};

class PascalGlobalsTable
        : public QStandardItem
{
public:
    explicit PascalGlobalsTable(const QString &title);
    void addVariable(const QString & name, const QString & type);
};


class PascalVariablesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit PascalVariablesModel(QObject *parent = 0);
    void addGlobalVariable(const QString & name, const QString & pascalType);

signals:

public slots:

private:
    PascalGlobalsTable * globalsTable_;

};

} // namespace GdbRun

#endif // GDBRUN_PASCALVARIABLESMODEL_H
