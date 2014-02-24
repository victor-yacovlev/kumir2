#include "fpcanalizer.h"
#include "simplepascalsyntaxanalizer.h"
#include "fpcanalizerplugin.h"

#include <QTemporaryFile>
#include <QFileInfo>
#include <QDesktopServices>


namespace FpcAnalizer {

FpcAnalizer::FpcAnalizer(QObject * plugin, uint instanceIndex)
    : QObject(plugin)
    , fpc_(new QProcess(this))
    , textCodec_(QTextCodec::codecForName("CP866"))
    , syntaxAnalizer_(0)
    , instanceIndex_(instanceIndex)
{
    syntaxAnalizer_ = SimplePascalSyntaxAnalizer::create(this);

    // These functions have special implementation, but should be highlighted
    // as regular procedures
    functionNames_.insert("read");
    functionNames_.insert("readln");
    functionNames_.insert("write");
    functionNames_.insert("writeln");
}

void FpcAnalizer::setSourceDirName(const QString &path)
{
    sourceDirName_ = path;
}

QList<Analizer::Error> FpcAnalizer::errors() const
{
    return errors_;
}

QList<Analizer::LineProp> FpcAnalizer::lineProperties() const
{
    return lineProps_;
}

void FpcAnalizer::setSourceText(const QString &plainText)
{
    executableFilePath_.clear();
    sourceText_ = plainText;
    sourceLines_ = sourceText_.split('\n');
    if (sourceLines_.last().isEmpty())
        sourceLines_.pop_back();
    lineProps_.clear();
    errors_.clear();
    lineRanks_.clear();
    for (int i=0; i<sourceLines_.size(); i++) {
        Analizer::LineProp lp = Analizer::LineProp(sourceLines_[i].size());
        lp.fill(LxTypeEmpty);
        lineProps_.append(lp);
        lineRanks_.append(QPoint(0, 0));
    }
    QPair<QByteArray,QString> fpcOut = startFpcProcessToCheck();
    parseFpcOutput(fpcOut.first, fpcOut.second);
    syntaxAnalizer_->processSyntaxAnalysis(
                sourceLines_
                , unitNames_
                , functionNames_
                , typeNames_
                , lineProps_
                , lineRanks_
                );
    parseFpcErrors(fpcOut.first, fpcOut.second);
}

QString FpcAnalizer::analizerWorkSubdir() const
{
    uint index = instanceIndex_;
    qint64 pid = QCoreApplication::applicationPid();
    return QString::fromAscii("kumir2-fpcanalizer/editor-%1-%2")
            .arg(pid).arg(index);
}

QPair<QByteArray,QString> FpcAnalizer::startFpcProcessToCheck()
{

    QDir tempDir =
            QDir(QDesktopServices::storageLocation(QDesktopServices::TempLocation));
    QString subdir = analizerWorkSubdir();
    tempDir.mkpath(subdir);
    QDir workDir(tempDir.absoluteFilePath(subdir));
    QPair<QString,SimplePascalSyntaxAnalizer::SourceType> sourceInfo
            = syntaxAnalizer_->processUntilUnitInformation(sourceLines_);
    const QString unitFileName = sourceInfo.first.isEmpty()
            ? "unnamed" : sourceInfo.first.toLower();
    QString sourceFileName = workDir.absoluteFilePath(unitFileName+".pas");
    QFile programFile(sourceFileName);
    programFile.open(QIODevice::WriteOnly|QIODevice::Text);
    programFile.write(rawSourceData().c_str());
    QFileInfo fi(programFile);
    programFile.close();
    fpc_->setWorkingDirectory(workDir.absolutePath());
    static const QString fpcpluginLibexecs = QDir::toNativeSeparators(
                QDir::cleanPath(
                    QCoreApplication::applicationDirPath()+"/../libexec/kumir2/fpcanalizer/"
                    ));
    QString unitPath = sourceDirName_.isEmpty() ? workDir.absolutePath() : sourceDirName_;
    QStringList arguments = fpcPlatformFlags();
    arguments
            << "-g"  // generate debug info
            << "-E"  // do not linkage
            << "-AAS" // use 'as' command for assembler
            << "-ap" // use pipes to comminicate with "assembler"
            << "-Fu"+QDir::toNativeSeparators(unitPath) // Add unit path
            << "-Fo"+QDir::toNativeSeparators(workDir.absolutePath()) // Add obj path
            << "-FU"+QDir::toNativeSeparators(unitPath) // Add output unit path
            << "-e" + fpcpluginLibexecs // override 'as' with provided fake assembler
            << fi.fileName();
    fpc_->setProcessChannelMode(QProcess::SeparateChannels);
    fpc_->start(fpcCommandName(), arguments);
    if (!fpc_->waitForFinished()) {
        qDebug() << fpc_->errorString();
        return QPair<QByteArray,QString>(QByteArray(), fi.fileName());
    }
    else {
        fpc_->setReadChannel(QProcess::StandardOutput);
        QByteArray out = fpc_->readAll();
        fpc_->setReadChannel(QProcess::StandardError);
        out += fpc_->readAll();
        return QPair<QByteArray,QString>(out, fi.fileName());
    }
}

QPair<QByteArray,QString> FpcAnalizer::startFpcToPrepareRun(RunTarget target, QString &error)
{
    QDir tempDir =
            QDir(QDesktopServices::storageLocation(QDesktopServices::TempLocation));
    QString subdir = analizerWorkSubdir();
    tempDir.mkpath(subdir);
    QDir workDir(tempDir.absoluteFilePath(subdir));
    QPair<QString,SimplePascalSyntaxAnalizer::SourceType> sourceInfo
            = syntaxAnalizer_->processUntilUnitInformation(sourceLines_);
    if (sourceInfo.second == SimplePascalSyntaxAnalizer::PascalLibrary) {
        error = tr("Library is not runnable");
        return QPair<QByteArray,QString>();
    }
    const QString unitFileName = sourceInfo.first.isEmpty()
            ? "unnamed" : sourceInfo.first.toLower();
    QString sourceFileName = workDir.absoluteFilePath(unitFileName+".pas");
    mainSourceFilePath_ = sourceFileName; // file name for GDB debugger
    QString ppuFileName = workDir.absoluteFilePath(sourceInfo.first).toLower() + ".ppu";
    if (QFile(ppuFileName).exists()) {
        QFile(ppuFileName).remove();
    }
    QFile programFile(sourceFileName);
    programFile.open(QIODevice::WriteOnly|QIODevice::Text);
    programFile.write(rawSourceData().c_str());
    programFile.close();
    if (!QFileInfo(workDir.absoluteFilePath("KumirHelper.pas")).exists()) {
        const QString srcPath = FpcAnalizerPlugin::self()->resoursesDir()
                .absoluteFilePath("KumirHelper.pas");
        QFile src(srcPath);
        src.open(QIODevice::ReadOnly|QIODevice::Text);
        const QByteArray data = src.readAll();
        src.close();
        QFile dst(workDir.absoluteFilePath("KumirHelper.pas"));
        dst.open(QIODevice::WriteOnly|QIODevice::Text);
        dst.write(data);
        dst.close();
    }    
    if (sourceInfo.second == SimplePascalSyntaxAnalizer::PascalUnit) {
        QString execFileName = workDir.absoluteFilePath(sourceInfo.first).toLower()+"_exec.pas";
        QFile execFile(execFileName);
        execFile.open(QIODevice::WriteOnly|QIODevice::Text);
        QString src = QString::fromAscii("program %1_Exec; uses %1; begin end.")
                .arg(sourceInfo.first);
        execFile.write(src.toAscii());
        execFile.close();
        sourceFileName = execFileName;
    }
    programFile.close();
    QFileInfo fi(programFile);
    QString outFileName = fi.absoluteFilePath();    
    QString suffix;
    uint counter = 0;
#ifdef Q_OS_WIN32
    suffix += ".exe";
#else
    suffix += ".bin";
#endif
    QString testName = fi.absoluteFilePath() + suffix;
    while (QFile(testName).exists() && !QFile(testName).isWritable()) {
        counter += 1;
        testName = QString("%1-%2%3").arg(fi.absoluteFilePath()).arg(counter).arg(suffix);
    }
    outFileName = testName;
    QString unitPath = sourceDirName_.isEmpty() ? workDir.absolutePath() : sourceDirName_;
    fpc_->setWorkingDirectory(workDir.absolutePath());
    QStringList arguments = fpcPlatformFlags();
    arguments
            << "-g"  // generate debug info
//            << "-Ci"  // generate i/o checking
            << "-Cr"  // generate range checking
            << "-Ct"  // generate stack checking
            << "-FaKumirHelper" // force add unit "KumirHelper"
            << "-Fu"+QDir::toNativeSeparators(unitPath) // Add unit path
            << "-Fo"+QDir::toNativeSeparators(workDir.absolutePath()) // Add obj path
            << "-FU"+QDir::toNativeSeparators(unitPath) // Add output unit path
            << "-o"+QDir::toNativeSeparators(outFileName) // Override output file name
            << QFileInfo(sourceFileName).fileName();
    fpc_->setProcessChannelMode(QProcess::SeparateChannels);
//    class SThread: public QThread { public: inline static void pause() { msleep(10000); } };
//    SThread::pause();
    fpc_->start(fpcCommandName(), arguments);
    if (!fpc_->waitForFinished()) {
        qDebug() << fpc_->errorString();
        return QPair<QByteArray,QString>(QByteArray(), outFileName);
    }
    else {
        fpc_->setReadChannel(QProcess::StandardOutput);
        QByteArray out = fpc_->readAll();
        fpc_->setReadChannel(QProcess::StandardError);
        out += fpc_->readAll();
        qDebug() << out;
        error = fpc_->exitCode() == 0
                ? ""
                : tr("The program can not be run while it has compile errors");
        return QPair<QByteArray,QString>(out, outFileName);
    }
}

QString FpcAnalizer::fpcCommandName()
{
#ifdef Q_OS_WIN32
    QString path = QCoreApplication::applicationDirPath() +
            "/../fpc/bin/i386-win32/fpc.exe";
    path = QDir::cleanPath(path);
    path = QDir::toNativeSeparators(path);
    return path;
#else
    return "fpc";
#endif
}

QStringList FpcAnalizer::fpcPlatformFlags()
{
#ifdef Q_OS_WIN32
    QString unitsBase = QCoreApplication::applicationDirPath() +
            "/../fpc/units/i386-win32";
    unitsBase = QDir::cleanPath(unitsBase);
    unitsBase = QDir::toNativeSeparators(unitsBase);
    QString libBase = QCoreApplication::applicationDirPath() +
            "/../fpc/lib/i386-win32";
    libBase = QDir::cleanPath(libBase);
    libBase = QDir::toNativeSeparators(libBase);
    QString binBase = QCoreApplication::applicationDirPath() +
            "/../fpc/bin/i386-win32";
    binBase = QDir::cleanPath(binBase);
    binBase = QDir::toNativeSeparators(binBase);
    const QStringList result = QStringList()
            << "-Fu" + unitsBase
            << "-Fu" + unitsBase + "/*"
            << "-Fu" + unitsBase + "/rtl"
            << "-Fl" + libBase
            << "-FD" + binBase
               ;
    return result;
#else
    return QStringList();
#endif
}

QString FpcAnalizer::prepareToRun(RunTarget target)
{
    if (TestingRun == target) {
        return "Testing not implemented yet for Pascal language";
    }
    QString errorMessage;
    executableFilePath_.clear();
    QPair<QByteArray,QString> runRes = startFpcToPrepareRun(target, errorMessage);
    if (errorMessage.isEmpty()) {
        executableFilePath_ = runRes.second;
    }
    syntaxAnalizer_->processSyntaxAnalysis(sourceLines_,
                                           unitNames_,
                                           functionNames_,
                                           typeNames_,
                                           lineProps_,
                                           lineRanks_);
    return errorMessage;
}

void FpcAnalizer::parseFpcErrors(const QByteArray &bytes, const QString &fileName)
{
    QStringList lines = textCodec_->toUnicode(bytes).split("\n");
    QString genericError;
    QRegExp pattern1("(\\S+)\\((\\d+),(\\d+)\\)\\s+(Error|Fatal):\\s+(.+)");
    QRegExp pattern2("(\\S+)\\((\\d+)\\)\\s+(Error|Fatal):\\s+(.+)");
    QRegExp errorsSummary("There were \\d+ errors compiling module, stopping");
    bool asOut = false;
    Q_FOREACH (QString line, lines) {
        line.replace('\r', "");
        if (!asOut) {
            if ("===BEGIN_FAKE_AS_OUTPUT" == line) {
                asOut = true;
            }
        }
        else {
            if ("===END_FAKE_AS_OUTPUT" == line) {
                asOut = false;
            }
        }
        if (!asOut) {
            if (line.startsWith("Fatal: ") && line!="Fatal: Compilation aborted") {
                genericError = line.mid(7);
            }
            else if (line.startsWith(fileName)) {
                if (pattern1.exactMatch(line)) {
                    const QString fn = pattern1.cap(1);
                    if (fn == fileName) {
                        int line = pattern1.cap(2).toInt() - 1;
                        line = qMin(line, sourceLines_.size()-1);
                        int col = pattern1.cap(3).toInt() - 1;
                        const QString message = pattern1.cap(5);
                        int start = qMin(col, sourceLines_[line].length()-1);
                        int end = start+1;
                        LineProp & lp = lineProps_[line];
                        LexemType tp = lp.at(start);
                        while (start>0 && lp[start] == tp) {
                            start--;
                        }
                        if (lp[start] != tp) start++;
                        while (end < lp.size() && lp[end] != tp) {
                            end++;
                        }
                        for (int i=start; i<end; i++) {
                            lp[i] = LexemType(lp[i] | LxTypeError);
                        }
                        Error err;
                        err.line = line;
                        err.start = start;
                        err.len = end - start;
                        err.code = message;
                        errors_.append(err);
                    }
                }
                else if (pattern2.exactMatch(line)) {
                    const QString fn = pattern2.cap(1);
                    if (fn == fileName) {
                        const QString message = pattern2.cap(4);
                        if (!errorsSummary.exactMatch(message)) {
                            int line = pattern2.cap(2).toInt() - 1;
                            line = qMin(line, sourceLines_.size()-1);
                            Error err;
                            err.code = message;
                            err.line = line;
                            err.start = 0;
                            err.len = sourceLines_[err.line].length();
                            errors_.append(err);
                            LineProp & lp = lineProps_[err.line];
                            for (int i=0; i<lp.size(); i++) {
                                lp[i] = LexemType(lp[i] | LxTypeError);
                            }
                        }
                    }
                }
            }
        }
    }
    if (genericError.length() > 0) {
        Error err;
        err.line = 0;
        err.code = genericError;
        err.start = 0;
        err.len = sourceLines_.at(0).length();
        errors_.append(err);
    }

}

void FpcAnalizer::parseFpcOutput(const QByteArray & bytes, const QString & fileName)
{
    QStringList lines = textCodec_->toUnicode(bytes).split("\n");
    bool asOut = false;
    Q_FOREACH (QString line, lines) {
        if (!asOut) {
            if ("===BEGIN_FAKE_AS_OUTPUT" == line) {
                asOut = true;
            }
        }
        else {
            if ("===END_FAKE_AS_OUTPUT" == line) {
                asOut = false;
            }
        }
        if (asOut) {
            if (line.trimmed().startsWith("call")) {
                parseCallLine(line.trimmed());
            }
            else if (line.trimmed().startsWith("# Definition")) {
                parseDefinition(line.trimmed());
            }
        }
    }
}

void FpcAnalizer::parseCallLine(const QString &line)
{
    static const QRegExp rxSep("\\s+");
    const QStringList pair = line.split(rxSep, QString::SkipEmptyParts);
    const QString argument = pair[1];
    const QStringList parts = argument.split('$');
    const QString qname = parts[0];
    int us_pos = qname.indexOf('_');
    if (us_pos != -1) {
        const QString unitName = qname.left(us_pos).toLower();
        if ("fpc" != unitName) {
            const QString procName = qname.mid(us_pos+1).toLower();
            unitNames_.insert(unitName);
            functionNames_.insert(procName);
            for (int i=1; i<parts.size(); i++) {
                const QString typeName = parts[i].toLower();
                typeNames_.insert(typeName);
            }
        }
    }
}

void FpcAnalizer::parseDefinition(const QString &line)
{
    static const QRegExp rxSep("\\s+");
    const QStringList pair = line.split(rxSep, QString::SkipEmptyParts);
    if (2 == pair.size()) {
        const QString typeName = pair[1].toLower();
        typeNames_.insert(typeName);
    }
}

std::string FpcAnalizer::rawSourceData() const
{
    const QByteArray bytes = textCodec_->fromUnicode(sourceText_);
    return std::string(bytes.constData());
}

Analizer::LineProp FpcAnalizer::lineProp(int lineNo, const QString &text) const
{
    Analizer::LineProp lp(text.length());
    lp.fill(LxTypeEmpty);
    syntaxAnalizer_->processLineProp(
                text
                , unitNames_
                , functionNames_
                , typeNames_
                , lineNo
                , lp
                );
    return lp;
}

QString FpcAnalizer::suggestFileName() const
{
    return syntaxAnalizer_->thisUnitName();
}

QList<Analizer::Suggestion> FpcAnalizer::suggestAutoComplete(int lineNo, const QString &before, const QString &after) const
{
    return QList<Analizer::Suggestion>();
}

Analizer::TextAppend FpcAnalizer::closingBracketSuggestion(int lineNo) const
{
    return syntaxAnalizer_->closingBracketSuggestion(lineNo, sourceLines_);
}

QStringList FpcAnalizer::importModuleSuggestion(int lineNo) const
{
    return QStringList();
}

QStringList FpcAnalizer::imports() const
{
    return QStringList();
}

QString FpcAnalizer::createImportStatementLine(const QString &importName) const
{
    return QString::fromAscii("uses %1").arg(importName);
}



}
