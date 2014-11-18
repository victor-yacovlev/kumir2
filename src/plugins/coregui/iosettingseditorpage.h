#ifndef COREGUI_IOSETTINGSEDITORPAGE_H
#define COREGUI_IOSETTINGSEDITORPAGE_H

#include "extensionsystem/settings.h"

#include <QWidget>

namespace CoreGUI {

namespace Ui {
class IOSettingsEditorPage;
}

class IOSettingsEditorPage
        : public QWidget
{
    Q_OBJECT

public:
    static const char*   UseFixedWidthKey;
    static const bool    UseFixedWidthDefaultValue;
    static const char*   WidthSizeKey;
    static const quint16 WidthSizeDefaultValue;

    explicit IOSettingsEditorPage(ExtensionSystem::SettingsPtr settings, QWidget *parent = 0);
    ~IOSettingsEditorPage();

public slots:
    void accept();
    void init();
    void resetToDefaults();
signals:
    void settingsChanged(const QStringList & keys);

private:
    Ui::IOSettingsEditorPage *ui;
    ExtensionSystem::SettingsPtr settings_;
};


} // namespace CoreGUI
#endif // COREGUI_IOSETTINGSEDITORPAGE_H
