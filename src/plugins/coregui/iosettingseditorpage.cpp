#include "iosettingseditorpage.h"
#include "ui_iosettingseditorpage.h"

namespace CoreGUI {

const char*   IOSettingsEditorPage::UseFixedWidthKey = "TerminalFixedWidth";
const bool    IOSettingsEditorPage::UseFixedWidthDefaultValue = false;
const char*   IOSettingsEditorPage::WidthSizeKey = "TermnalWidth";
const quint16 IOSettingsEditorPage::WidthSizeDefaultValue = 80u;

IOSettingsEditorPage::IOSettingsEditorPage(ExtensionSystem::SettingsPtr settings, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::IOSettingsEditorPage)
    , settings_(settings)
{
    ui->setupUi(this);
}

IOSettingsEditorPage::~IOSettingsEditorPage()
{
    delete ui;
}

void IOSettingsEditorPage::accept()
{
    QStringList changedKeys;
    if (ui->useFixedWidth->isChecked() != settings_->value(UseFixedWidthKey, UseFixedWidthDefaultValue).toBool()) {
        settings_->setValue(UseFixedWidthKey, ui->useFixedWidth->isChecked());
        changedKeys << UseFixedWidthKey;
    }
    if (ui->widthInCharacters->value() != settings_->value(WidthSizeKey, WidthSizeDefaultValue).toInt()) {
        settings_->setValue(WidthSizeKey, ui->widthInCharacters->value());
        changedKeys << WidthSizeKey;
    }
    if (changedKeys.size() > 0) {
        emit settingsChanged(changedKeys);
    }
}

void IOSettingsEditorPage::init()
{
    ui->useFixedWidth->setChecked(settings_->value(UseFixedWidthKey, UseFixedWidthDefaultValue).toBool());
    ui->widthInCharacters->setValue(
                qMin(ui->widthInCharacters->maximum(),
                     qMax(
                         ui->widthInCharacters->minimum(),
                         settings_->value(WidthSizeKey, WidthSizeDefaultValue).toInt()
                         )
                     )
                );
}

void IOSettingsEditorPage::resetToDefaults()
{
    settings_->setValue(UseFixedWidthKey, UseFixedWidthDefaultValue);
    settings_->setValue(WidthSizeKey, WidthSizeDefaultValue);
    init();
    emit settingsChanged(QStringList() << UseFixedWidthKey << WidthSizeKey);
}

} // namespace CoreGUI
