#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVariant>

class Settings
{
public:
    static Settings & get();

    bool skipOpenDialog() const;
    void setSkipOpenDialog(bool value);

    QString lastSelectedKey() const;
    void setLastSelectedKey(const QString & key);

private:
    explicit Settings(const QString & dir, const QString & file);

    QSettings q_;
};

#endif // SETTINGS_H
