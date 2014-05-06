#ifndef EXTENSIONSYSTEM_LOGGER_H
#define EXTENSIONSYSTEM_LOGGER_H

class QFile;
class QString;

#include <QtGlobal>

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
    void debug(const char * message);
    void warning(const char * message);
    void critical(const char * message);
    void fatal(const char * message);
    ~Logger();
private:
    void writeLog(const char * type, const char * message);

    Logger(const QString & filePath, LogLevel logLevel);
    QFile * loggerFile_;
    LogLevel logLevel_;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_LOGGER_H
