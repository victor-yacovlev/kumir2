#include "guisettingspage.h"
#include "ui_guisettingspage.h"

namespace CoreGUI {

const QString GUISettingsPage::LayoutKey = "MainWindowLayout";
const QString GUISettingsPage::RowsFirstValue = "RowsFirst";
const QString GUISettingsPage::ColumnsFirstValue = "ColumnsFirst";

GUISettingsPage::GUISettingsPage(ExtensionSystem::SettingsPtr settings, QWidget *parent)
    : settings_(settings)
    , QWidget(parent)
    , ui(new Ui::GUISettingsPage)
{
    ui->setupUi(this);
    ui->lblRowsFirst->setPixmap(QPixmap(":/coregui/layout-rows-first.png"));
    ui->lblColumnsFirst->setPixmap(QPixmap(":/coregui/layout-columns-first.png"));
    ui->gridLayout_2->setAlignment(ui->lblColumnsFirst, Qt::AlignHCenter|Qt::AlignBottom);
    ui->gridLayout_2->setAlignment(ui->lblRowsFirst, Qt::AlignHCenter|Qt::AlignBottom);
    ui->gridLayout_2->setAlignment(ui->btnColumnsFirst, Qt::AlignHCenter|Qt::AlignTop);
    ui->gridLayout_2->setAlignment(ui->btnRowsFirst, Qt::AlignHCenter|Qt::AlignTop);
    init();
}

void GUISettingsPage::init()
{
    const QString layoutValue = settings_->value(LayoutKey, RowsFirstValue).toString();
    if (layoutValue == ColumnsFirstValue) {
        ui->btnColumnsFirst->setChecked(true);
    }
    else {
        ui->btnRowsFirst->setChecked(true);
    }
}

void GUISettingsPage::accept()
{
    const QString layoutValue = settings_->value(LayoutKey, RowsFirstValue).toString();
    if (ui->btnColumnsFirst->isChecked()) {
        settings_->setValue(LayoutKey, ColumnsFirstValue);
    }
    else {
        settings_->setValue(LayoutKey, RowsFirstValue);
    }
    if (layoutValue != settings_->value(LayoutKey, RowsFirstValue).toString()) {
        emit settingsChanged(QStringList() << LayoutKey);
    }
}

void GUISettingsPage::resetToDefaults()
{
    const QString layoutValue = settings_->value(LayoutKey, RowsFirstValue).toString();
    ui->btnRowsFirst->setChecked(true);
    settings_->setValue(LayoutKey, RowsFirstValue);
    if (layoutValue != settings_->value(LayoutKey, RowsFirstValue).toString()) {
        emit settingsChanged(QStringList() << LayoutKey);
    }
}

GUISettingsPage::~GUISettingsPage()
{
    delete ui;
}

} // namespace CoreGUI
