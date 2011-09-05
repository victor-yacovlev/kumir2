#include "clangcompiler.h"

#define __STDC_LIMIT_MACROS 100
#define __STDC_CONSTANT_MACROS 100
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/FrontendActions.h>
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/TextDiagnosticBuffer.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Basic/Diagnostic.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/HeaderSearch.h>
#include <clang/Lex/Lexer.h>
#include <llvm/Target/TargetSelect.h>



namespace KumirNativeGenerator {

CLangCompiler::CLangCompiler(QObject *parent) :
    QObject(parent)
{
    QDir tmp = QDir::temp();
    s_tempDirPath = QString::fromAscii("kumir-clang-%1").arg(qApp->applicationPid());
    tmp.mkdir(s_tempDirPath);
    s_tempDirPath = tmp.absoluteFilePath(s_tempDirPath);
    createCompilerInstance();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

}

void CLangCompiler::reset()
{

}

QString CLangCompiler::kumirLibraryPath()
{
    QString result = qApp->applicationDirPath();
#ifdef Q_OS_UNIX
    QString libPrefix = QString::fromLocal8Bit(IDE_LIBRARY_BASENAME);
    result += "/../"+libPrefix+"/kumir2";
#endif
#ifdef Q_OS_MAC
#error Implement me: set path to libKumirStdLib etc.
#endif
    return QDir::cleanPath(result);
}

QString CLangCompiler::targetTriple()
{
    QString cpu;
    QString vendor;
    QString os;
    QString linker;

    if (QString(IDE_LIBRARY_BASENAME).endsWith("64")) {
        cpu = "x86_64";
    }
    else {
        cpu = "i386";
    }

#ifdef Q_OS_MAC
    vendor = "apple";
    os = "darwin";
#else
    vendor = "pc";
#endif

#ifdef Q_OS_LINUX
    os = "linux";
#endif
#ifdef Q_OS_FREEBSD
    os = "freebsd";
#endif

    linker = "gnu";

    return cpu+"-"+vendor+"-"+os+"-"+linker;
}

void CLangCompiler::doCompilation(const QList<NamedData> &headers, const QList<NamedData> &sources)
{
    saveFilesToTempDir(headers, sources);

    llvm::SmallVector<const char *, 64> Args;
    Args.push_back("-v");
    static const QString gnuInclude = QString("%1/kumirnativegenerator/gcc45-include")
            .arg(qApp->property("sharePath").toString());
//    static const QString libraryPath = QString("-L%1").arg(kumirLibraryPath());
//    char * L = (char*)calloc(libraryPath.toLocal8Bit().size()+1, sizeof(char));
//    qMemCopy(L, libraryPath.toLocal8Bit().constData(), libraryPath.toLocal8Bit().size());
//    L[libraryPath.toLocal8Bit().size()] = '\0';

//    Args.push_back(L);
//    Args.push_back("-lKumirStdLib");
//    for (int i=0; i<sources.size(); i++) {
//        char * sourceName = (char*)calloc(sources[i].first.toLocal8Bit().size()+1, sizeof(char));
//        qMemCopy(sourceName, sources[i].first.toLocal8Bit().constData(), sources[i].first.toLocal8Bit().size());
//        sourceName[sources[i].first.toLocal8Bit().size()] = '\0';
//        Args.push_back(sourceName);
//    }
    clang::CompilerInvocation inv;

    inv.getFrontendOpts().ShowStats = true;
//    clang::CompilerInvocation::CreateFromArgs(inv, Args.begin(), Args.end(), m_compiler->getDiagnostics());
    inv.getHeaderSearchOpts().AddPath(llvm::StringRef(gnuInclude.toStdString()), clang::frontend::Angled, true, false, false);
    inv.setLangDefaults(clang::IK_C, clang::LangStandard::lang_c99);
    for (int i=0; i<sources.size(); i++) {
        std::pair<clang::InputKind, std::string> p;
        p.first = clang::IK_C;
        p.second = sources[i].first.toStdString();
        inv.getFrontendOpts().Inputs.push_back(p);
    }
    inv.getTargetOpts().Triple = targetTriple().toStdString();

    QList< QPair<llvm::StringRef, llvm::MemoryBuffer*> > inFiles = saveFilesToBuffers(headers, sources);
    for (int i=0; i<inFiles.size(); i++) {
//        inv.getPreprocessorOpts().addRemappedFile(inFiles[i].first, inFiles[i].second);
    }

    m_compiler->setInvocation(&inv);
    std::string Error;
    m_compiler->createOutputFile(QString(s_tempDirPath+"aaa.o").toStdString(), Error);
    llvm::errs() << Error;



    clang::FrontendAction * action = new clang::EmitCodeGenOnlyAction();
    if (!m_compiler->ExecuteAction(*action)) {
    }
    else {

    }

    printDiagnostics();

}

QList< QPair<llvm::StringRef, llvm::MemoryBuffer*> > CLangCompiler::saveFilesToBuffers(const QList<NamedData> &headers, const QList<NamedData> &sources)
{

    QList< QPair<llvm::StringRef, llvm::MemoryBuffer*> > result;
    QList<NamedData> allData = headers + sources;
    for (int i=0; i<allData.size(); i++) {
        NamedData d = allData[i];
        QPair<llvm::StringRef, llvm::MemoryBuffer*> p;
        p.first = d.first.toStdString();
        QByteArray ba = d.second.toLocal8Bit();
        char * ch = (char*)calloc(ba.size()+1, sizeof(char));
        qMemCopy(ch, ba.constData(), ba.size());
        ch[ba.size()] = '\0';
        llvm::StringRef sr(ch);
        Q_ASSERT(ba.size()==(int)sr.size());
        p.second = llvm::MemoryBuffer::getMemBuffer(sr);
        int a = p.second->getBuffer().str().length();
        int b = ba.size();
        Q_ASSERT(a==b);

        result << p;
    }
    return result;
}

void CLangCompiler::printDiagnostics()
{
    qDebug() << "Clang errors: ";
    clang::TextDiagnosticBuffer::const_iterator it;
    clang::TextDiagnosticBuffer * dd = m_errorBuffer;
    for (it=dd->err_begin(); it!=dd->err_end(); it++) {
        std::pair< clang::SourceLocation, std::string > item = *it;
        item.first.print(llvm::errs(), m_compiler->getSourceManager());
        llvm::errs() << item.second;
        llvm::errs() << "\n";
    }
    for (it=dd->warn_begin(); it!=dd->warn_end(); it++) {
        std::pair< clang::SourceLocation, std::string > item = *it;
        item.first.print(llvm::errs(), m_compiler->getSourceManager());
        llvm::errs() << item.second;
        llvm::errs() << "\n";
    }
    for (it=dd->note_begin(); it!=dd->note_end(); it++) {
        std::pair< clang::SourceLocation, std::string > item = *it;
        item.first.print(llvm::errs(), m_compiler->getSourceManager());
        llvm::errs() << item.second;
        llvm::errs() << "\n";
    }
}

void CLangCompiler::saveFilesToTempDir(const QList<NamedData> &headers, const QList<NamedData> &sources)
{
    for (int i=0; i<headers.size(); i++) {
        QFile f(s_tempDirPath+"/"+headers[i].first);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(headers[i].second.toUtf8());
            f.close();
        }
    }
    for (int i=0; i<sources.size(); i++) {
        QFile f(s_tempDirPath+"/"+sources[i].first);
        if (f.open(QIODevice::WriteOnly)) {
            f.write(sources[i].second.toUtf8());
            f.close();
        }
    }
}

void CLangCompiler::createCompilerInstance()
{
    m_compiler = new clang::CompilerInstance();
    llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> diagId = llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs>(new clang::DiagnosticIDs());
    llvm::IntrusiveRefCntPtr<clang::Diagnostic> diag = llvm::IntrusiveRefCntPtr<clang::Diagnostic>(new clang::Diagnostic(diagId));
    m_errorBuffer = new clang::TextDiagnosticBuffer();
    diag->setClient(m_errorBuffer);
    m_compiler->setDiagnostics(diag.getPtr());
    clang::FileSystemOptions fso;
    fso.WorkingDir = s_tempDirPath.toStdString();
    clang::FileManager * fm = new clang::FileManager(fso);
    m_compiler->setFileManager(fm);
    clang::SourceManager * sm = new clang::SourceManager(*diag, *fm);
    m_compiler->setSourceManager(sm);

}



void CLangCompiler::doLinkage()
{

}

QString CLangCompiler::lastError() const
{
    return "";
}

QByteArray CLangCompiler::getResult() const
{
    return QByteArray();
}

QString CLangCompiler::makeOneBigModule(const QList<NamedData> &headers, const QList<NamedData> &sources)
{
    QString result;
    foreach (const NamedData & hpp, headers) {
        result += hpp.second + "\n";
    }
    result += "// =============================================== \n";

    foreach (NamedData cpp, sources) {
        result += cpp.second + "\n";
    }
//    qDebug() << result;
    QStringList lines = result.split("\n");
    for (int i=0; i<lines.size(); i++) {
        if (lines[i].startsWith("#include \""))
            lines[i] = "";
    }
    result = lines.join("\n");
//    qDebug() << result;
    return result;
}

} // namespace KumirNativeGenerator
