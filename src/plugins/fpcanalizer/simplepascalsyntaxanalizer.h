#ifndef FPCANALIZER_SIMPLEPASCALSYNTAXANALIZER_H
#define FPCANALIZER_SIMPLEPASCALSYNTAXANALIZER_H

#include "interfaces/analizer_instanceinterface.h"

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
    void processSyntaxAnalysis(const QStringList & lines,
                               const QStringList & unitNames,
                               const QStringList & functionNames,
                               const QStringList & typeNames,
                               QList<LineProp> & lineProps,
                               QList<QPoint> & lineRanks);
    void processLineProp(const QString& line,
                         const QStringList & unitNames,
                         const QStringList & functionNames,
                         const QStringList & typeNames,
                         int lineNo,
                         LineProp & lineProp
                         );
    QString correctCapitalization(const QString & name, LexemType lxType) const ;


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
                     LineProp & lineProp, QPoint& rank, State& endState
                     ) const;
    static QPoint keywordRank(const QString &keyword);
    const QStringList Keywords;
    const QStringList Operators;
    const QRegExp Delimeters;
    const QStringList StandardTypes;
    QStringList unitNames_;
    QStringList functionNames_;
    QStringList typeNames_;

    QList<State> lineStartStates_;

};

} // namespace FpcAnalizer

#endif // FPCANALIZER_SIMPLEPASCALSYNTAXANALIZER_H
