#include "settingsdialog.h"
#include "ui_settingsdialog.h"

namespace ExtensionSystem {

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(handleGroupSelected(int)));
    if (ui->listWidget->count()>0) {
        ui->listWidget->setCurrentRow(0);
    }
}

void SettingsDialog::accept()
{
    for (int i=0; i<l_pluginPages.size(); i++) {
        l_pluginPages[i]->accept();
    }
}

int SettingsDialog::exec()
{
    for (int i=0; i<l_pluginPages.size(); i++) {
        l_pluginPages[i]->exec();
    }
    return QDialog::exec();
}

void SettingsDialog::handleGroupSelected(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
}

void SettingsDialog::addPage(const SettingsEditorPage &page)
{
    if (!page.settingsPage)
        return;
    QListWidgetItem * item = new QListWidgetItem(ui->listWidget);
    item->setText(page.settingsGroupName);
    if (page.settingsGroupIcon)
        item->setIcon(*(page.settingsGroupIcon));
    else
        item->setIcon(QIcon::fromTheme("preferences-other"));
    ui->listWidget->addItem(item);
    ui->stackedWidget->addWidget(page.settingsPage);
    l_pluginPages << page.settingsPage;

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

} // namespace ExtensionSystem
