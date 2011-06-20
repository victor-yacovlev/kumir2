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
        const QMetaObject * mo = l_pluginPages[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
            const QString signature = mo->method(j).signature();
            if (signature=="accept()") {
                m = mo->method(j);
                found = true;
                break;
            }
        }
        if (found) {
            m.invoke(l_pluginPages[i]);
        }
    }
    QDialog::accept();
}

int SettingsDialog::exec()
{
    for (int i=0; i<l_pluginPages.size(); i++) {
        const QMetaObject * mo = l_pluginPages[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
            const QString signature = mo->method(j).signature();
            if (signature=="init()") {
                m = mo->method(j);
                found = true;
                break;
            }
        }
        if (found) {
            m.invoke(l_pluginPages[i]);
        }
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
    page.settingsPage->setParent(this);
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
