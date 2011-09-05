#ifndef KUMIRNATIVEGENERATOR_CLANGCOMPILER_H
#define KUMIRNATIVEGENERATOR_CLANGCOMPILER_H

#include <QtCore>

#define __STDC_LIMIT_MACROS 100
#define __STDC_CONSTANT_MACROS 100
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/TextDiagnosticBuffer.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/MemoryBuffer.h>


namespace KumirNativeGenerator {

typedef QPair<QString,QString> NamedData;

class CLangCompiler : public QObject
{
    Q_OBJECT
public:
    explicit CLangCompiler(QObject *parent = 0);

signals:

public slots:
    void reset();
    void doCompilation(const QList<NamedData> & headers, const QList<NamedData> & sources);
    void doLinkage();
    QString lastError() const;
    QByteArray getResult() const;

private:
    static QString makeOneBigModule(const QList<NamedData> & headers, const QList<NamedData> & sources);
    void saveFilesToTempDir(const QList<NamedData> & headers, const QList<NamedData> & sources);
    QList< QPair<llvm::StringRef, llvm::MemoryBuffer*> > saveFilesToBuffers(const QList<NamedData> &headers, const QList<NamedData> &sources);
    void createCompilerInstance();
    void printDiagnostics();
    static QString kumirLibraryPath();
    static QString targetTriple();
    clang::CompilerInstance * m_compiler;
    QString s_tempDirPath;
    clang::TextDiagnosticBuffer * m_errorBuffer;

};

} // namespace KumirNativeGenerator

#endif // KUMIRNATIVEGENERATOR_CLANGCOMPILER_H
