#ifndef FPC_ANALIZER_H
#define FPC_ANALIZER_H

#include "interfaces/analizer_instanceinterface.h"
#include "simplepascalsyntaxanalizer.h"

#include <QObject>
#include <QProcess>
#include <QTextCodec>

namespace FpcAnalizer {

using namespace Shared;

class FpcAnalizer
        : public QObject
        , public Analizer::InstanceInterface
{
    friend class FpcAnalizerPlugin;

public:
    void setSourceDirName(const QString &path);
    void setSourceText(const QString &plainText);
    virtual std::string rawSourceData() const;
    inline virtual QList<Analizer::Error> errors() const { return errors_; }
    virtual QList<Analizer::LineProp> lineProperties() const { return lineProps_; }
    virtual QList<QPoint> lineRanks() const { return lineRanks_; }
    virtual Analizer::LineProp lineProp(int lineNo, const QString & text) const;
    inline QString correctCapitalization(const QString & name, LexemType lxType) const
    { return syntaxAnalizer_->correctCapitalization(name, lxType); }

protected:
    explicit FpcAnalizer(QObject * plugin);
    QProcess* fpc_;
    QTextCodec* textCodec_;
    SimplePascalSyntaxAnalizer * syntaxAnalizer_;
    QString sourceDirName_;
    QString sourceText_;
    QStringList sourceLines_;
    QList<Analizer::Error> errors_;
    QList<Analizer::LineProp> lineProps_;
    QList<QPoint> lineRanks_;
    QStringList unitNames_;
    QStringList functionNames_;
    QStringList typeNames_;
};

}

#endif
