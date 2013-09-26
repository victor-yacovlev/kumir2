#ifndef EDITOR_MACROEDITOR_H
#define EDITOR_MACROEDITOR_H

#include <QDialog>
#include <QTableWidgetItem>

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
    void setMacro(class Macro * macro);

private slots:
    void checkMacroTitle(const QString & title);
    void changeMacroLetter(QTableWidgetItem * item, QTableWidgetItem *);
    
private:
    Ui::MacroEditor *ui;
    class Macro * macro_;
};


} // namespace Editor
#endif // EDITOR_MACROEDITOR_H
