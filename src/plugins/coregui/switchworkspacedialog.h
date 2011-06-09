#ifndef COREGUI_SWITCHWORKSPACEDIALOG_H
#define COREGUI_SWITCHWORKSPACEDIALOG_H

#include <QtCore>
#include <QtGui>

namespace CoreGUI {

namespace Ui {
    class SwitchWorkspaceDialog;
}

class SwitchWorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SwitchWorkspaceDialog(QSettings * settings);
    ~SwitchWorkspaceDialog();
    void setCurrentWorkspace(const QString &path);
    QString currentWorkspace() const;
    static QString WorkspacesListKey;
    static QString CurrentWorkspaceKey;
    static QString SkipChooseWorkspaceKey;
private slots:
    void handleBrowseClicked();
    void handleAccepted();
private:
    Ui::SwitchWorkspaceDialog *ui;
    QSettings * m_settings;
};


} // namespace CoreGUI
#endif // COREGUI_SWITCHWORKSPACEDIALOG_H
