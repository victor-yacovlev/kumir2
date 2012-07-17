#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui>
#include "kplugin.h"

namespace ExtensionSystem {

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    void addPage(QWidget* page);
    ~SettingsDialog();
public slots:
    int exec();
    void accept();

private slots:
    void handleGroupSelected(int index);

private:
    Ui::SettingsDialog *ui;
    QList<QWidget*> l_pluginPages;
};


} // namespace ExtensionSystem
#endif // SETTINGSDIALOG_H
