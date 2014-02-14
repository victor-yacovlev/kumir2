#include "fpcanalizer.h"
#include "simplepascalsyntaxanalizer.h"


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

void FpcAnalizer::setSourceText(const QString &plainText)
{
    sourceText_ = plainText;
    sourceLines_ = plainText.split('\n');
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
    syntaxAnalizer_->processSyntaxAnalysis(
                sourceLines_
                , unitNames_
                , functionNames_
                , typeNames_
                , lineProps_
                , lineRanks_
                );
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
