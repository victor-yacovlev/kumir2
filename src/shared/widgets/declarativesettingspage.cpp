#include "declarativesettingspage.h"
#include "declarativesettingspage_impl.h"

namespace Widgets {

DeclarativeSettingsPage::DeclarativeSettingsPage(
        const QString &title,
        ExtensionSystem::SettingsPtr settings,
        const QMap<QString, Entry> entries)
    : QWidget(nullptr)
    , pImpl_(new DeclarativeSettingsPageImpl(this))
{
    setMaximumWidth(400);
    setWindowTitle(title);
    pImpl_->entries_ = entries;
    QVBoxLayout * layout = new QVBoxLayout();
    setLayout(layout);
    foreach (const QString &key, entries.keys()) {
        const Entry entry = entries[key];
        if (entry.type==Integer) {
            pImpl_->addIntegerField(key, entry);
        }
        else if (entry.type==Color) {
            pImpl_->addColorField(key, entry);
        }
        else {
            qFatal("Not implemented");
        }
    }
    layout->addStretch();
    setSettingsObject(settings);
}

void DeclarativeSettingsPage::setSettingsObject(ExtensionSystem::SettingsPtr settings)
{
    pImpl_->settings_ = settings;
    pImpl_->init();
}

ExtensionSystem::SettingsPtr DeclarativeSettingsPage::settingsObject() const
{
    return pImpl_->settings_;
}

void DeclarativeSettingsPage::init()
{
    pImpl_->init();
}

void DeclarativeSettingsPage::resetToDefaults()
{
    pImpl_->resetToDefaults();
}

void DeclarativeSettingsPage::accept()
{
    pImpl_->accept();
}

}
