#ifndef EDITOR_MACROEDITOR_H
#define EDITOR_MACROEDITOR_H

#include <QDialog>
#include <QTableWidgetItem>
#include <QSharedPointer>
#include "macro.h"

namespace Editor {

namespace Ui {
class MacroEditor;
}


class MacroEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit MacroEditor(QWidget *parent = 0);
    ~MacroEditor();
    void setUsedSymbols(const QString &symbols, const QStringList & names);
    void setMacro(QSharedPointer<Macro> macro);

private slots:
    void checkMacroTitle(const QString & title);
    void changeMacroLetter(QTableWidgetItem * item, QTableWidgetItem *);
    
private:
    Ui::MacroEditor *ui;
    QSharedPointer<Macro> macro_;
};


} // namespace Editor
#endif // EDITOR_MACROEDITOR_H
