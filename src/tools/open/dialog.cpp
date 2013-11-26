#include "dialog.h"
#include "ui_dialog.h"

OpenDialog::OpenDialog(const QList<Application> & apps)
    : QDialog(0)
    , ui_(new Ui::OpenDialog)
{
    ui_->setupUi(this);
    foreach (const Application & app, apps) {
        ui_->comboBox->addItem(app.icon, app.name, app.id);
    }
}

quintptr OpenDialog::selectedApplicationId() const
{
    return ui_->comboBox->itemData(ui_->comboBox->currentIndex()).toULongLong();
}
