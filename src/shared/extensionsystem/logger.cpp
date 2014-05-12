#include "logger.h"

#include <QFile>
#include <QStringList>
#include <QCoreApplication>
#include <QDateTime>
#include <QBuffer>
#include <QTextStream>


extern "C" {
#include <stdio.h>
}

namespace ExtensionSystem {

Logger* Logger::instance()
{
    QString path;
    foreach (QString arg, qApp->arguments()) {
        if (arg.startsWith("--log=")) {
            path = arg.mid(6);
            break;
        }
    }
    static Logger logger = Logger(path, qApp->arguments().contains("--debug")? Debug : Release);
    return &logger;
}

Logger::Logger(const QString & filePath, LogLevel logLevel)
    : loggerFile_(0), logLevel_(logLevel)
{
    if (filePath.length() > 0) {
        loggerFile_ = new QFile(filePath);
        loggerFile_->open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    }
    writeLog("STARTED", "");
}

Logger::~Logger()
{
    writeLog("EXITING", "");
    if (loggerFile_) {
        loggerFile_->close();
        delete loggerFile_;
        loggerFile_ = 0;
    }
}

void Logger::writeLog(const char *type, const char *message)
{
    QByteArray buffer;
    buffer += QDateTime::currentDateTime().toString("hh:mm:ss");
    buffer += "\t";
    buffer += QByteArray(type);
    buffer += "\t";
    buffer += QByteArray(message);
    buffer += "\r\n";
    if (loggerFile_) {
        loggerFile_->write(buffer);
        loggerFile_->flush();
    }
    else {
        fprintf(stderr, "%s", buffer.data());
    }
}

void Logger::debug(const char *message)
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

void Logger::warning(const char *message)
{
    writeLog("WARNING", message);
}

void Logger::critical(const char *message)
{
    writeLog("CRITICAL", message);
}

void Logger::fatal(const char *message)
{
    writeLog("FATAL", message);
}



} // namespace ExtensionSystem
