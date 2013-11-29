#include "dialog.h"
#include "ui_dialog.h"
#include "settings.h"

OpenDialog::OpenDialog(const QList<Application> & apps)
    : QDialog(0)
    , ui_(new Ui::OpenDialog)
{
    ui_->setupUi(this);

    const Settings & sett = Settings::get();

    int selectedIndex = 0;

    foreach (const Application & app, apps) {
        if (app.key == sett.lastSelectedKey()) {
            selectedIndex = ui_->comboBox->count();
        }
        ui_->comboBox->addItem(app.icon, app.name, app.id);
    }

    ui_->comboBox->setCurrentIndex(selectedIndex);
    ui_->checkBox->setChecked(sett.skipOpenDialog());
    ui_->hint->setVisible(ui_->checkBox->isChecked());
}

quintptr OpenDialog::selectedApplicationId() const
{
    return ui_->comboBox->itemData(ui_->comboBox->currentIndex()).toULongLong();
}

void OpenDialog::accept()
{
    Settings & sett = Settings::get();
    sett.setSkipOpenDialog(ui_->checkBox->isChecked());
    QDialog::accept();
}
