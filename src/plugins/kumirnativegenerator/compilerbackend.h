#ifndef COMPILERBACKEND_H
#define COMPILERBACKEND_H

#include <QtCore>

typedef QPair<QString,QString> StringPair;

namespace KumirNativeGenerator
{

class AbstractCompilerBackend
{
public:
    virtual QString generateExecuable(const QList<StringPair> & headers,
                                  const QList<StringPair> & sources,
                                  const QStringList & systemLibs,
                                  const QStringList & kumirLibs,
                                  QIODevice * out) = 0;
    virtual void setVerbose(bool v) = 0;
    virtual void setTemporaryDir(const QString & path, bool autoclean) = 0;
};

}

Q_DECLARE_INTERFACE(KumirNativeGenerator::AbstractCompilerBackend, "ru.niisi.kumir2.KumirNativeGenerator.CompilerBackend")

#endif // COMPILERBACKEND_H
