#ifndef PYTHON3LANGUAGE_SYNTAXCHECKSETTINGSPAGE_H
#define PYTHON3LANGUAGE_SYNTAXCHECKSETTINGSPAGE_H

#include <QObject>
#include <QWidget>
#include <QCheckBox>

#include "extensionsystem/settings.h"

namespace Python3Language {

class SyntaxCheckSettingsPage : public QWidget
{
    Q_OBJECT
public:
    static const char * UsePep8Key;
    static const bool UsePep8DefaultValue;

    explicit SyntaxCheckSettingsPage(ExtensionSystem::SettingsPtr settings, QWidget *parent = 0);
    ~SyntaxCheckSettingsPage();

    inline void setSettingsObject(ExtensionSystem::SettingsPtr settings) {
        settings_ = settings;
    }

public Q_SLOTS:
    void accept();
    void init();
    void resetToDefaults();

Q_SIGNALS:
    void settingsChanged(const QStringList & keys);

private:
    ExtensionSystem::SettingsPtr settings_;
    QCheckBox * usePep8_;

};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_SYNTAXCHECKSETTINGSPAGE_H
