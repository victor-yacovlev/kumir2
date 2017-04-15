#ifndef EXTENSIONSYSTEM_COMMANDLINEPARAMETER_H
#define EXTENSIONSYSTEM_COMMANDLINEPARAMETER_H


#include <QtGlobal>
#include <QString>
#include <QVariant>
#include <QObject>
#include <QList>

#ifdef EXTENSIONSYSTEM_LIBRARY
#define EXTENSIONSYSTEM_EXPORT Q_DECL_EXPORT
#else
#define EXTENSIONSYSTEM_EXPORT Q_DECL_IMPORT
#endif

namespace ExtensionSystem {

class EXTENSIONSYSTEM_EXPORT CommandLineParameter
        : public QObject
{
    Q_OBJECT
public:
    bool isValid() const;

    // Constructor for named parameter with value
    explicit CommandLineParameter(
            bool allowInGui,                // true, if allow parameter in GUI mode
            const QChar & shortName,        // One-letter parameter name
            const QString & longName,       // Many-letters parameter name
            const QString & description,    // Description shown in "--help"
            QVariant::Type acceptType,      // Parameter type
            bool required                   // true, if parameter is required
            );

    // Constructor for named flag
    explicit CommandLineParameter(
            bool allowInGui,                // true, if allow parameter in GUI mode
            const QChar & shortName,        // One-letter parameter name
            const QString & longName,       // Many-letters parameter name
            const QString & description     // Description shown in "--help"
            );

    // Constructor for unnamed parameter of startup module
    explicit CommandLineParameter(
            bool allowInGui,                // true, if allow parameter in GUI mode
            const QString & shortDescription,// Unnamed parameter "VARIABLE" shown in "--help"
            const QString & description,    // Description shown in "--help"
            QVariant::Type acceptType,      // Parameter type
            bool required                   // true, if parameter is required
            );

    // Copy constructors
    explicit CommandLineParameter(const CommandLineParameter &other);
    CommandLineParameter& operator=(const CommandLineParameter &other);

    friend class PluginManager;
    friend struct PluginManagerImpl;
    friend class CommandLine;
protected:
    bool allowInGui_;
    QChar shortName_;
    QString longName_;
    QVariant value_;
    QString shortDescription_;
    QString description_;
    bool acceptValue_;
    bool valueRequired_;
    QVariant::Type acceptType_;
    QString toHelpLine() const;
    void fillValue(const QString &argument);
};

class EXTENSIONSYSTEM_EXPORT CommandLine
{
public:
    /** returns true if command line contains @param shortName flag */
    bool hasFlag(const QChar & shortName) const;
    inline bool hasFlag(const char shortName) const
    { return hasFlag(QChar(shortName)); }

    /** returns true if command line contains @param longName flag */
    bool hasFlag(const QString & longName) const;

    /** returns value of command line parameter @param shortName */
    QVariant value(const QChar &shortName) const;
    inline QVariant value(const char shortName) const
    { return value(QChar(shortName)); }

    /** returns value of command line parameter @param longName */
    QVariant value(const QString &longName) const;

    /** returns count of unnamed command line parameters */
    size_t size() const;

    /** returns value of unnamed command line parameter by its numeric index */
    QVariant value(size_t index) const;



    friend class PluginManager;
    friend struct PluginManagerImpl;

    explicit CommandLine();

protected:
    explicit CommandLine(const QList<CommandLineParameter> &);
    QList<const CommandLineParameter*> unnamedParameters() const;
    QList<CommandLineParameter> data_;
};

} // namespace ExtensionSystem

#endif // EXTENSIONSYSTEM_COMMANDLINEPARAMETER_H
