#include "settingspage.h"
#include "ui_settingspage.h"

namespace Editor {

SettingsPage::SettingsPage(QSettings * settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsPage),
    m_settings(settings)
{
    ui->setupUi(this);
}

void SettingsPage::accept()
{

}

SettingsPage::~SettingsPage()
{
    delete ui;
}

} // namespace Editor
