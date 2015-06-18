#include "syntaxchecksettingspage.h"

#include <QVBoxLayout>
#include <QSpacerItem>
#include <QGroupBox>

namespace Python3Language {

const char* SyntaxCheckSettingsPage::UsePep8Key = "SyntaxCheck/PEP8";
const bool SyntaxCheckSettingsPage::UsePep8DefaultValue = false;

SyntaxCheckSettingsPage::SyntaxCheckSettingsPage(ExtensionSystem::SettingsPtr settings, QWidget *parent)
    : QWidget(parent)
    , settings_(settings)
    , usePep8_(new QCheckBox(this))
{
    setWindowTitle(tr("Syntax checking"));
    QVBoxLayout * l = new QVBoxLayout;
    QVBoxLayout * ll = new QVBoxLayout;
    setLayout(l);
    QGroupBox * groupBox = new QGroupBox(tr("Additional syntax checkers to use"), this);
    groupBox->setLayout(ll);
    ll->addWidget(usePep8_);
    l->addWidget(groupBox);
    usePep8_->setText(tr("PEP-8 Coding Style"));
    l->addStretch();
}

SyntaxCheckSettingsPage::~SyntaxCheckSettingsPage()
{

}

void SyntaxCheckSettingsPage::accept()
{
    QStringList changedKeys;
    if (settings_) {
        if (settings_->value(UsePep8Key, UsePep8DefaultValue).toBool() != usePep8_->isChecked()) {
            changedKeys.append(UsePep8Key);
        }
        settings_->setValue(UsePep8Key, usePep8_->isChecked());
    }
    if (!changedKeys.isEmpty()) {
        Q_EMIT settingsChanged(changedKeys);
    }
}

void SyntaxCheckSettingsPage::init()
{
    if (settings_) {
        usePep8_->setChecked(settings_->value(UsePep8Key, UsePep8DefaultValue).toBool());
    }
    else {
        resetToDefaults();
    }
}

void SyntaxCheckSettingsPage::resetToDefaults()
{
    usePep8_->setChecked(UsePep8DefaultValue);
}

} // namespace Python3Language

