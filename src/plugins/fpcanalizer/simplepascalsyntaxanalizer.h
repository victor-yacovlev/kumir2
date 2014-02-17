#ifndef FPCANALIZER_SIMPLEPASCALSYNTAXANALIZER_H
#define FPCANALIZER_SIMPLEPASCALSYNTAXANALIZER_H

#include "interfaces/analizer_instanceinterface.h"
#include "interfaces/analizer_helperinterface.h"

#include <QObject>

namespace FpcAnalizer {

using namespace Shared;
using namespace Shared::Analizer;

class SimplePascalSyntaxAnalizer : public QObject
{    
    Q_OBJECT
public:
    static SimplePascalSyntaxAnalizer* create(QObject *parent);
    void reset();
    inline QString thisUnitName() const { return thisUnitName_; }
    void processSyntaxAnalysis(const QStringList & lines,
                               const QSet<QString> & unitNames,
                               const QSet<QString> & functionNames,
                               const QSet<QString> & typeNames,
                               QList<LineProp> & lineProps,
                               QList<QPoint> & lineRanks);
    void processLineProp(const QString& line,
                         const QSet<QString> & unitNames,
                         const QSet<QString> & functionNames,
                         const QSet<QString> & typeNames,
                         int lineNo,
                         LineProp & lineProp
                         );
    QString correctCapitalization(const QString & name, LexemType lxType) const ;
    Analizer::TextAppend closingBracketSuggestion(int lineNo, const QStringList &lines) const;


private /*methods*/:
    explicit SimplePascalSyntaxAnalizer(QObject *parent,
                                        const QStringList& kwd,
                                        const QStringList& ops,
                                        const QStringList& tps,
                                        const QRegExp& rx);

signals:

public slots:

private:
    enum State {
        Program,
        String,
        LineComment,
        BlockComment
    };
    struct Lexem {
        int start;
        int length;
        State stateAfter;
    };
    Lexem takeLexem(const QString &line, int startPos, State startState) const;
    void processLine(const QString &line, const State initialState,
                     LineProp & lineProp, QPoint& rank, State& endState,
                     QString & previousKeyword
                     ) ;
    static QPoint keywordRank(const QString &keyword);
    const QStringList Keywords;
    const QStringList Operators;
    const QRegExp Delimeters;
    const QStringList StandardTypes;
    QSet<QString> unitNames_;
    QSet<QString> functionNames_;
    QSet<QString> typeNames_;

    QList<State> lineStartStates_;
    QStringList lineStartKeywords_;
    QString thisUnitName_;
    QMap<QString,QString> capitalizationHints_;

};

} // namespace FpcAnalizer

#endif // FPCANALIZER_SIMPLEPASCALSYNTAXANALIZER_H
