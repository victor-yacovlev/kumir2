#ifndef KUMIRNATIVEGENERATOR_LLVMBACKEND_H
#define KUMIRNATIVEGENERATOR_LLVMBACKEND_H

#include <QtCore>
#include "compilerbackend.h"

typedef QPair<QString,QString> StringPair;

namespace KumirNativeGenerator {

class LlvmBackend
        : public QObject
        , public AbstractCompilerBackend
{
    Q_OBJECT
    Q_INTERFACES(KumirNativeGenerator::AbstractCompilerBackend)
public:
    explicit LlvmBackend(QObject *parent = 0);
    QString generateExecuable(
            const QList<StringPair> &headers,
            const QList<StringPair> &sources,
            const QStringList &systemLibs,
            const QStringList &kumirLibs,
            QIODevice *out
            );
    void setVerbose(bool v);
    void setTemporaryDir(const QString &path, bool autoclean);
protected:
    static QString generateArgumentsLine(const QList<StringPair> &sources,
                                         const QString & outFileName,
                                         const QStringList &systemLibs,
                                         const QStringList &kumirLibs);
    static QString debugSystemLib(const QString & n);
    static QString debugKumirLib(const QString & n);
    static QStringList libraryPaths();
    static QString execuableSuffix();
    static QString llvmCommand();
    QString tempDirName() const;
    bool saveFiles(const QList<StringPair> & files) const;
    static QString rpath();
    QProcess * m_process;
    bool b_verbose;
    QString s_tempDir;
    bool b_cleanTempDir;
};

} // namespace KumirNativeGenerator

#endif // KUMIRNATIVEGENERATOR_LLVMBACKEND_H
