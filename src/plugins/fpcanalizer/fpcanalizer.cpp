#include "fpcanalizer.h"
#include "simplepascalsyntaxanalizer.h"

#include <QTemporaryFile>
#include <QFileInfo>


namespace FpcAnalizer {

FpcAnalizer::FpcAnalizer(QObject * plugin)
    : QObject(plugin)
    , fpc_(new QProcess(this))
    , textCodec_(QTextCodec::codecForName("CP866"))
    , syntaxAnalizer_(0)
{
    syntaxAnalizer_ = SimplePascalSyntaxAnalizer::create(this);
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
    sourceText_ = plainText;
    sourceLines_ = sourceText_.split('\n');
    if (sourceLines_.last().isEmpty())
        sourceLines_.pop_back();
    lineProps_.clear();
    errors_.clear();
    lineRanks_.clear();
    unitNames_.clear();
    functionNames_.clear();
    for (int i=0; i<sourceLines_.size(); i++) {
        Analizer::LineProp lp = Analizer::LineProp(sourceLines_[i].size());
        lp.fill(LxTypeEmpty);
        lineProps_.append(lp);
        lineRanks_.append(QPoint(0, 0));
    }
    QPair<QByteArray,QString> fpcOut = startFpcProcessToCheck();
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

QPair<QByteArray,QString> FpcAnalizer::startFpcProcessToCheck()
{
    QTemporaryFile programFile("kumir-XXXXXX.pas");
    programFile.open();
    programFile.write(rawSourceData().c_str());
    QFileInfo fi(programFile);
    QString dirName = fi.absoluteDir().absolutePath();
    programFile.close();
    fpc_->setWorkingDirectory(dirName);
    QStringList arguments;
    arguments
            << "-g"  // generate debug info
            << "-E"  // do not linkage
            << fi.fileName();
    fpc_->setProcessChannelMode(QProcess::SeparateChannels);
    fpc_->start("fpc", arguments);
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

void FpcAnalizer::parseFpcErrors(const QByteArray &bytes, const QString &fileName)
{
    QStringList lines = textCodec_->toUnicode(bytes).split("\n");
    QString genericError;
    QRegExp pattern1("(\\S+)\\((\\d+),(\\d+)\\)\\s+(Error|Fatal):\\s+(.+)");
    QRegExp pattern2("(\\S+)\\((\\d+)\\)\\s+(Error|Fatal):\\s+(.+)");
    QRegExp errorsSummary("There were \\d+ errors compiling module, stopping");
    Q_FOREACH (QString line, lines) {
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
                    int start = col;
                    int end = col+1;
                    LineProp & lp = lineProps_[line];
                    LexemType tp = lp.at(col);
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
    if (genericError.length() > 0) {
        Error err;
        err.line = 0;
        err.code = genericError;
        err.start = 0;
        err.len = sourceLines_.at(0).length();
        errors_.append(err);
    }

}

void FpcAnalizer::parseFpcOutput(const QByteArray &bytes)
{
    QStringList lines = textCodec_->toUnicode(bytes).split("\n");
    qDebug() << lines;
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

}
