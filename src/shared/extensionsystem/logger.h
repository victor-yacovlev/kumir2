#ifndef EXTENSIONSYSTEM_LOGGER_H
#define EXTENSIONSYSTEM_LOGGER_H

class QFile;

#include <QString>

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

namespace ExtensionSystem {

class EXTENSIONSYSTEM_EXPORT Logger
{
public:
    enum LogLevel {
        Release,
        Debug
    };

    static Logger * instance();
    inline void debug(const char * message) { debug(QString::fromLocal8Bit(message)); }
    inline void warning(const char * message) { warning(QString::fromLocal8Bit(message)); }
    inline void critical(const char * message) { critical(QString::fromLocal8Bit(message)); }
    inline void fatal(const char * message) { fatal(QString::fromLocal8Bit(message)); }

    void debug(const QString & message);
    void warning(const QString & message);
    void critical(const QString & message);
    void fatal(const QString & message);

    ~Logger();
private:
    void writeLog(const char * type, const QString &message);

    Logger(const QString & filePath, LogLevel logLevel);
    QFile * loggerFile_;
    LogLevel logLevel_;
    static Logger * self_;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_LOGGER_H
