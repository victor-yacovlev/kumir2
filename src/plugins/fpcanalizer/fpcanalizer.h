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
        , public Analizer::HelperInterface
        , public Analizer::ExternalExecutableCompilerInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Analizer::InstanceInterface
                 Shared::Analizer::HelperInterface
                 Shared::Analizer::ExternalExecutableCompilerInterface
                 )
    friend class FpcAnalizerPlugin;
public:
    void setSourceDirName(const QString &path);
    void setSourceText(const QString &plainText);
    virtual std::string rawSourceData() const;
    QList<Analizer::Error> errors() const;
    QList<Analizer::LineProp> lineProperties() const;
    QList<QPoint> lineRanks() const { return lineRanks_; }
    Analizer::LineProp lineProp(int lineNo, const QString & text) const;
    inline QString correctCapitalization(const QString & name, LexemType lxType) const
    { return syntaxAnalizer_->correctCapitalization(name, lxType); }

    QList<Suggestion> suggestAutoComplete(int lineNo, const QString & before, const QString & after) const;
    TextAppend closingBracketSuggestion(int lineNo) const;
    QStringList importModuleSuggestion(int lineNo) const;
    QStringList imports() const;
    QString createImportStatementLine(const QString &importName) const;
    QString suggestFileName() const;

    void prepareToRun();
    inline bool compiledWithoutErrors() const { return compiledWithoutErrors_; }
    QString executableFilePath() const { return executableFilePath_; }


protected:
    static QString fpcCommandName();
    static QStringList fpcPlatformFlags();
    QPair<QByteArray,QString> startFpcProcessToCheck();
    QPair<QByteArray,QString> startFpcToPrepareRun();
    void parseFpcErrors(const QByteArray & bytes, const QString & fileName);
    void parseFpcOutput(const QByteArray & bytes, const QString & fileName);
    void parseCallLine(const QString &line);
    void parseDefinition(const QString &line);
    explicit FpcAnalizer(QObject * plugin, uint instanceIndex);
    QProcess* fpc_;
    QTextCodec* textCodec_;
    SimplePascalSyntaxAnalizer * syntaxAnalizer_;
    QString sourceDirName_;
    QString sourceText_;
    QStringList sourceLines_;
    QList<Analizer::Error> errors_;
    QList<Analizer::LineProp> lineProps_;
    QList<QPoint> lineRanks_;
    QSet<QString> unitNames_;
    QSet<QString> functionNames_;
    QSet<QString> typeNames_;
    uint instanceIndex_;

    bool compiledWithoutErrors_;
    QString executableFilePath_;
};

}

#endif
