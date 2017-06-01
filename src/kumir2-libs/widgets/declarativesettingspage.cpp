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
    QList<Entry> entriesArray;
    entriesArray.reserve(entries.size());
    Q_FOREACH(const QString &key, entries.keys()) {
        Entry entry = entries[key];
        entry.key = key;
        entriesArray.push_back(entry);
    }
    qSort(entriesArray);
    Q_FOREACH(const Entry & entry, entriesArray) {
        const QString & key = entry.key;
        if (entry.type==Integer) {
            pImpl_->addIntegerField(key, entry);
        }
        else if (entry.type==Double) {
            pImpl_->addRealField(key, entry);
        }
        else if (entry.type==Color) {
            pImpl_->addColorField(key, entry);
        }
        else if (entry.type==String) {
            pImpl_->addStringField(key, entry);
        }
        else if (entry.type==Choice) {
            pImpl_->addChoiceField(key, entry);
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
