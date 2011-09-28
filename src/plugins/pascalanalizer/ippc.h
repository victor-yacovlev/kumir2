#ifndef PASCALANALIZER_IPPC_H
#define PASCALANALIZER_IPPC_H

#include <QtCore>
#include "dataformats/lexem.h"
#include "interfaces/lineprop.h"
#include "interfaces/error.h"

namespace PascalAnalizer {

using namespace Shared;
using namespace AST;

class IPPC : public QObject
{
    Q_OBJECT
public:
    struct Names {
        QStringList units;
        QStringList procedures;
        QStringList classes;
    };
    struct CheckResult {
        QList< LineProp > lineProps;
        QList<Error> errors;
        QList<QPoint> ranks;
        Names names;
    };

    explicit IPPC(const QStringList & unitpaths, QObject *parent = 0);
    ~IPPC();
    LineProp lineSyntax(const QString & text, const Names & names);
    CheckResult processAnalisys(const QString & text, const Names & oldNames);
    inline void setSourceName(const QString &s) { s_sourceName = s; }
signals:

private slots:
    void handleReadyStdErr();
    void handleFinished(int code, QProcess::ExitStatus stat);
    void handleError(QProcess::ProcessError err);


private:

    QStringList readAllLines();
    bool ensureProcessRunning();
    QVector<LineProp> extractLineProps(const QStringList & lines, const QStringList & textLines, const Names & names);
    QVector<QPoint> extractIndentRanks(const QStringList & lines, const QStringList & textLines);
    QList<Error> extractErrors(const QStringList & lines, const QList<LineProp> &props);
    Names extractNames(const QStringList & lines);
    QString sourceName() const;
    QString screenString(QString s);
    QString s_sourceName;
    static LexemType typeByString(const QString &s);
    QTextCodec * codec_oem866;
    QTextStream ts;
    QString s_command;
    QProcess * process;


};

} // namespace PascalAnalizer

#endif // PASCALANALIZER_IPPC_H
