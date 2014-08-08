#include "logger.h"

#include <QFile>
#include <QStringList>
#include <QCoreApplication>
#include <QDateTime>
#include <QBuffer>
#include <QTextStream>
#include <QDir>

extern "C" {
#include <stdio.h>
}

namespace ExtensionSystem {

Logger* Logger::self_ = 0;

Logger* Logger::instance()
{    
    if (!self_) {
        QString path;
        foreach (QString arg, qApp->arguments()) {
            if (arg.startsWith("--log=")) {
                path = arg.mid(6);
                break;
            }
        }
#if defined(Q_OS_WIN32) && defined(QT_NO_DEBUG)
        if (path.isEmpty() && qApp->arguments().contains("--debug")) {
            path = QDir::temp().absoluteFilePath("kumir-log.txt");
            if (QFile::exists(path)) {
                QFile::remove(path);
            }
        }
#endif
        self_ = new Logger(path, qApp->arguments().contains("--debug")? Debug : Release);
    }
    return self_;
}

Logger::Logger(const QString & filePath, LogLevel logLevel)
    : loggerFile_(0), logLevel_(logLevel)
{
    if (filePath.length() > 0) {
        loggerFile_ = new QFile(filePath);
        loggerFile_->open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    }
#ifdef NDEBUG
    bool process = Debug == logLevel_;
#else
    bool process = true;
#endif
    if (process) {
        writeLog("STARTED", "");
    }
}

Logger::~Logger()
{
#ifdef NDEBUG
    bool process = Debug == logLevel_;
#else
    bool process = true;
#endif
    if (process) {
        writeLog("EXITING", "");
    }
    if (loggerFile_) {
        loggerFile_->close();
        delete loggerFile_;
        loggerFile_ = 0;
    }
}

void Logger::writeLog(const char *type, const QString &message)
{
    QByteArray buffer;
    buffer += QDateTime::currentDateTime().toString("hh:mm:ss");
    buffer += "\t";
    buffer += QByteArray(type);
    buffer += "\t";
    buffer += message.toUtf8();
    buffer += "\r\n";
    if (loggerFile_) {
        loggerFile_->write(buffer);
        loggerFile_->flush();
    }
    else {
        fprintf(stderr, "%s", buffer.data());
    }
}

void Logger::debug(const QString &message)
{
#ifdef NDEBUG
    bool process = Debug == logLevel_;
#else
    bool process = true;
#endif
    if (process) {
        writeLog("DEBUG", message);
    }
}

void Logger::warning(const QString &message)
{
#ifdef NDEBUG
    bool process = Debug == logLevel_;
#else
    bool process = true;
#endif
    if (process) {
        writeLog("WARNING", message);
    }
}

void Logger::critical(const QString &message)
{
    writeLog("CRITICAL", message);
}

void Logger::fatal(const QString &message)
{
    writeLog("FATAL", message);
}



} // namespace ExtensionSystem
