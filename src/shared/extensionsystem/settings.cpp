#include "settings.h"

#include <QDir>
#include <QCoreApplication>

namespace ExtensionSystem {

Settings::Settings(const QString & pluginName)
    : pluginName_(pluginName)
    , mutex_(new QMutex)
    , qsettings_(new QSettings(defaultSettingsScope(), pluginName))
{
    qsettings_->setIniCodec("UTF-8");
    settingsFile_ = qsettings_->fileName();
}

void Settings::setValue(const QString &key, const QVariant &value_)
{
    mutex_->lock();
    qsettings_->setValue(key, value_);
    mutex_->unlock();
}

QVariant Settings::value(const QString &key, const QVariant &default_) const
{
    QVariant result;
    mutex_->lock();
    result = qsettings_->value(key, default_);
    mutex_->unlock();
    return result;
}

void Settings::changeWorkingDirectory(const QString &workDirPath)
{
    workDirPath_ = workDirPath;
    mutex_->lock();
    qsettings_->sync();

    QDir workDir(workDirPath);
    workDir.mkdir(".settings");

    const QString settingsFileName =
            workDirPath + "/.settings/" + pluginName_ + ".conf";
    QSettings * sett = new QSettings(settingsFileName, QSettings::IniFormat);
    sett->setIniCodec("UTF-8");

    qsettings_.reset(sett);
    settingsFile_ = qsettings_->fileName();

    mutex_->unlock();
}

QString Settings::locationDirectory() const
{
    mutex_->lock();
    const QFileInfo fileInfo(qsettings_->fileName());
    mutex_->unlock();
    return fileInfo.absoluteDir().absolutePath();
}

QString Settings::defaultSettingsScope()
{
#ifdef Q_OS_MAC
    static const QString result = QString::fromAscii("kumir2");
    return result;
#else
    static const QString applicationLanucher = qApp->arguments().at(0);
    static const QString applicationName =
            applicationLanucher.startsWith(qApp->applicationDirPath())
            ? applicationLanucher.mid(qApp->applicationDirPath().length() + 1)
            : applicationLanucher;
    return applicationName;
#endif
}

} // namespace ExtensionSystem
