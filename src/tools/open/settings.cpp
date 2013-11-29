#include "settings.h"

Settings & Settings::get()
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    static Settings settings("kumir2", "kumir2-open");
    return settings;
}

Settings::Settings(const QString & dir, const QString & file)
    : q_(dir, file)
{
    q_.setIniCodec("UTF-8");
}


bool Settings::skipOpenDialog() const
{
    return q_.value("SkipOpenDialog", false).toBool();
}

void Settings::setSkipOpenDialog(bool value)
{
    q_.setValue("SkipOpenDialog", value);
}

QString Settings::lastSelectedKey() const
{
    return q_.value("LastSelected", "").toString();
}

void Settings::setLastSelectedKey(const QString &key)
{
    q_.setValue("LastSelected", key);
}
