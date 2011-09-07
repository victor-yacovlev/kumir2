#include "llvmbackend.h"
#include <iostream>

namespace KumirNativeGenerator {

LlvmBackend::LlvmBackend(QObject *parent) :
    QObject(parent)
{
    m_process = new QProcess(this);
#ifdef QT_DEBUG
    setVerbose(true);
#else
    setVerbose(false);
#endif
    b_cleanTempDir = true;
}

void LlvmBackend::setVerbose(bool v)
{
    b_verbose = v;
    m_process->setProcessChannelMode(v? QProcess::ForwardedChannels : QProcess::SeparateChannels);
}

void LlvmBackend::setTemporaryDir(const QString &path, bool autoclean)
{
    s_tempDir = path;
    b_cleanTempDir = autoclean;
}

QString LlvmBackend::generateExecuable(
        const QList<StringPair> &headers,
        const QList<StringPair> &sources,
        const QStringList &systemLibs,
        const QStringList &kumirLibs,
        QIODevice *out)
{
    QString outName = "out"+execuableSuffix();
    if (out->metaObject()->className()==QString("QFile")) {
        QFile * outFile = qobject_cast<QFile*>(out);
        outName = outFile->fileName();
    }
    QString command = llvmCommand()+" "+generateArgumentsLine(sources, outName, systemLibs, kumirLibs);
    if (!QDir::root().exists(tempDirName())) {
        if (!QDir::root().mkpath(tempDirName()))
            return "Can't create temporary directory: "+tempDirName();
    }
    if (!saveFiles(headers+sources)) {
        return "Can't save files in: "+tempDirName();
    }

    if (b_verbose) {
        std::cerr << command.toLocal8Bit().data() << std::endl;
    }
    m_process->setWorkingDirectory(tempDirName());
    m_process->start(command);
    m_process->waitForStarted();
    if (m_process->state()!=QProcess::Running)
        return "Error staring process: "+m_process->errorString();
    m_process->waitForFinished();
    if (!QFile::exists(outName))
        return "Compilation error";
    if (b_cleanTempDir) {
        QList<StringPair> files = headers+sources;
        QDir td(tempDirName());
        for (int i=0; i<files.size(); i++) {
            td.remove(files[i].first);
        }
        QDir::root().remove(tempDirName());
    }
    return "";

}

QString LlvmBackend::generateArgumentsLine(
        const QList<StringPair> &sources,
        const QString & outFileName,
        const QStringList &systemLibs,
        const QStringList &kumirLibs)
{
    QStringList result;
    result << "-I"+qApp->property("sharePath").toString()+"/kumirnativegenerator/";
#ifdef Q_OS_WIN32
    result << "-I"+qApp->property("sharePath").toString()+"/kumirnativegenerator/mingw-include/";
#else
    result << "-I"+qApp->property("sharePath").toString()+"/kumirnativegenerator/gcc45-include/";
#endif
#ifdef QT_DEBUG
    result << "-g";
#endif
    result << "-O0";
    result << "-std=c99";
#ifdef Q_OS_WIN32
    result << "-DWIN32";
    result << "-Wl,-enable-auto-import";
    result << "-Wl,-enable-runtime-pseudo-reloc";
    result << "-mthreads";
#endif
    result << QString("-L")+libraryPath();
    result << rpath();
    for (int i=0; i<systemLibs.size(); i++) {
        result << "-l"+debugSystemLib(systemLibs[i]);
    }
    for (int i=0; i<kumirLibs.size(); i++) {
        result << "-l"+debugKumirLib(kumirLibs[i]);
    }
    result << "-o" << outFileName;
    for (int i=0; i<sources.size(); i++) {
        result << sources[i].first;
    }
    return result.join(" ");
}

QString LlvmBackend::libraryPath()
{
#ifdef Q_OS_WIN32
    return QCoreApplication::applicationDirPath();
#endif
#ifdef Q_OS_UNIX
    return QDir::cleanPath(QCoreApplication::applicationDirPath()+"/../"+QString::fromLocal8Bit(IDE_LIBRARY_BASENAME)+"/kumir2");
#endif
}

QString LlvmBackend::debugSystemLib(const QString & n)
{
#ifdef QT_NO_DEBUG
    if (n.startsWith("Qt")) {
#ifdef Q_OS_WIN32
        unsigned int qtMajor = QT_VERSION >> 16;
        return n + QString::number(qtMajor);
#endif
        return n;
    }
    else {
        return n;
    }
#endif
#ifdef Q_OS_WIN32
    if (n.startsWith("Qt")) {
        unsigned int qtMajor = QT_VERSION >> 16;
        return n + QString::number(qtMajor) + "d";
    }
    else {
        return n;
    }
#endif
#ifdef Q_OS_MAC
    if (n.startsWith("Qt")) {
        unsigned int qtMajor = QT_VERSION >> 16;
        return n + QString::number(qtMajor) + "_debug";
    }
    else {
        return n + "_debug";
    }
#endif
    return n;
}

QString LlvmBackend::debugKumirLib(const QString &n)
{
#ifdef QT_NO_DEBUG
    return n;
#endif
#ifdef Q_OS_WIN32
    return n+"d";
#endif
#ifdef Q_OS_MAC
    return n+"_debug";
#endif
    return n;
}

QString LlvmBackend::rpath()
{
#ifndef Q_OS_WIN32
    return "-Wl,-rpath="+libraryPath();
#endif
    return "";
}

QString LlvmBackend::execuableSuffix()
{
#ifdef Q_OS_WIN32
    return ".exe";
#endif
    return ".bin";
}

QString LlvmBackend::llvmCommand()
{
#ifdef Q_OS_WIN32
    return QDir::cleanPath(QCoreApplication::applicationDirPath()+"/clang.exe");
#endif
    return "clang";
}

QString LlvmBackend::tempDirName() const
{
    if (s_tempDir.isEmpty())
        return QDir::tempPath()+"/kumir2-nativegenerator-"+QString::number(QCoreApplication::applicationPid());
    else
        return s_tempDir;
}

bool LlvmBackend::saveFiles(const QList<StringPair> &files) const
{
    for (int i=0; i<files.size(); i++) {
        const QString path = tempDirName()+"/"+files[i].first;
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly)) {
            return false;
        }
        f.write(files[i].second.toLocal8Bit());
        f.close();
    }
    return true;
}

} // namespace KumirNativeGenerator
