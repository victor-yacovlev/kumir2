#ifndef EDITOR_MACROLISTEDITOR_H
#define EDITOR_MACROLISTEDITOR_H

#include <macro.h>

#include <QDialog>
#include <QListWidgetItem>

namespace Editor {

namespace Ui {
class MacroListEditor;
}

class MacroListEditor : public QDialog
{
    Q_OBJECT
    
public:
    explicit MacroListEditor(const QDir & resourcesRoot, QWidget *parent = 0);
    void initialize(const QList<Macro> & macros,
                    const QList<Macro> &systemMacros);
    QList<Macro> result() const;
    ~MacroListEditor();
    

private slots:
    void handleItemChanged(QListWidgetItem * current, QListWidgetItem*);
    void removeMacro();
    void editMacro();

private:
    Ui::MacroListEditor *ui;
    QList<Macro> macros_;
    QList<Macro> systemMacros_;
    QString prefix_;
};


} // namespace Editor
#endif // EDITOR_MACROLISTEDITOR_H
