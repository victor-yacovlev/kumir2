#ifndef COREGUI_SWITCHWORKSPACEDIALOG_H
#define COREGUI_SWITCHWORKSPACEDIALOG_H

#include "settings.h"

#include <QtCore>
#include <QtGui>

namespace ExtensionSystem {

namespace Ui {
    class SwitchWorkspaceDialog;
}

class SwitchWorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SwitchWorkspaceDialog(SettingsPtr settings);
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
    SettingsPtr settings_;
};


} // namespace CoreGUI
#endif // COREGUI_SWITCHWORKSPACEDIALOG_H
