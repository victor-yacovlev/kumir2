#include "settings.h"

#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>

namespace ExtensionSystem {

Settings::Settings(const QString & pluginName)
    : pluginName_(pluginName)
    , mutex_(new QMutex)
#ifdef Q_OS_WIN32
    , qsettings_(new QSettings(QDesktopServices::storageLocation(QDesktopServices::DataLocation)+"/"+defaultSettingsScope()+"/"+pluginName+".conf", QSettings::IniFormat))
#else
    , qsettings_(new QSettings(defaultSettingsScope(), pluginName))
#endif
{    
    qsettings_->setIniCodec("UTF-8");
    settingsFile_ = qsettings_->fileName();
}

void Settings::setValue(const QString &key, const QVariant &value_)
{
    if (!mutex_)
        return;
    mutex_->lock();
    qsettings_->setValue(key, value_);
    mutex_->unlock();
}

QVariant Settings::value(const QString &key, const QVariant &default_) const
{
    if (!mutex_)
        return QVariant();
    QVariant result;
    mutex_->lock();
    result = qsettings_->value(key, default_);
    mutex_->unlock();
    return result;
}

void Settings::flush()
{
    mutex_->lock();
    qsettings_->sync();
    mutex_->unlock();
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
    static const QString applicationLanucher = QDir::fromNativeSeparators(qApp->arguments().at(0));
    static QString applicationName =
            applicationLanucher.startsWith(qApp->applicationDirPath())
            ? applicationLanucher.mid(qApp->applicationDirPath().length() + 1)
            : applicationLanucher;
#ifdef Q_OS_WIN32
    if (applicationName.endsWith(".exe")) {
        applicationName.remove(applicationName.length()-4, 4);
    }
#endif
    return applicationName;
#endif
}

} // namespace ExtensionSystem
