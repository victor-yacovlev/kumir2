#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QShowEvent>
#include <QTextBrowser>

namespace CoreGUI {

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void copySystemInformationToClipboard();
    void initializeEnvironmentData();

private:
    void showEvent(QShowEvent *event);

    void addQtVersion();
    void addOsVersion();
    void addExecuablePath();
    void addLoadedModules();
    void addSettingsFilesPaths();
    Ui::AboutDialog *ui;
};

} // namespace CoreGUI
#endif // ABOUTDIALOG_H
