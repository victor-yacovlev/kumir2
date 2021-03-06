#ifndef COREGUI_GUISETTINGSPAGE_H
#define COREGUI_GUISETTINGSPAGE_H

#include <kumir2-libs/extensionsystem/settings.h>

#include <QWidget>
#include <QCheckBox>

namespace CoreGUI {

namespace Ui {
class GUISettingsPage;
}

class GUISettingsPage : public QWidget
{
    Q_OBJECT

public:
    static const QString LayoutKey /* = "MainWindowLayout" */;
    static const QString RowsFirstValue /* = "RowsFirst" */;
    static const QString ColumnsFirstValue /* = "ColumnsFirst" */;
    explicit GUISettingsPage(ExtensionSystem::SettingsPtr settings, QWidget *parent = 0);
    ~GUISettingsPage();

public slots:
    void createVisibleIconsGrid();
    void accept();
    void init();
    void resetToDefaults();
signals:
    void settingsChanged(const QStringList & keys);

private:   
    Ui::GUISettingsPage *ui;
    ExtensionSystem::SettingsPtr settings_;
    QList<QCheckBox*> toolbarVisibleItems_;

};


} // namespace CoreGUI
#endif // COREGUI_GUISETTINGSPAGE_H
