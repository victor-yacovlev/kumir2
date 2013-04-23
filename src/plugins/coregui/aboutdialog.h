#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

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

private:
    void addQtVersion();
    void addOsVersion();
    void addExecuablePath();
    void addLoadedModules();
    Ui::AboutDialog *ui;
};


} // namespace CoreGUI
#endif // ABOUTDIALOG_H
