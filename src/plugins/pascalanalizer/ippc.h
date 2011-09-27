#ifndef PASCALANALIZER_IPPC_H
#define PASCALANALIZER_IPPC_H

#include <QtCore>
#include "dataformats/lexem.h"
#include "interfaces/lineprop.h"
#include "interfaces/error.h"

namespace PascalAnalizer {

using namespace Shared;
using namespace AST;

class IPPC : public QProcess
{
    Q_OBJECT
public:
    struct CheckResult {
        QList< LineProp > lineProps;
        QList<Error> errors;
        QList<QPoint> ranks;
    };
    explicit IPPC(const QStringList & unitpaths, QObject *parent = 0);
    ~IPPC();
    LineProp lineSyntax(const QString & text);
    CheckResult processAnalisys(const QString & text);
    inline void setSourceName(const QString &s) { s_sourceName = s; }
signals:

private slots:
    void handleReadyStdErr();
    void handleFinished(int code, QProcess::ExitStatus stat);
    void handleError(QProcess::ProcessError err);


private:
    bool ensureProcessRunning();
    QVector<LineProp> extractLineProps(const QStringList & lines, const QStringList & textLines);
    QVector<QPoint> extractIndentRanks(const QStringList & lines, const QStringList & textLines);
    QList<Error> extractErrors(const QStringList & lines, const QList<LineProp> &props);
    QString sourceName() const;
    QString screenString(QString s);
    QString s_sourceName;
    static LexemType typeByString(const QString &s);
    QTextCodec * codec_oem866;
    QTextStream ts;
    QString s_command;

};

} // namespace PascalAnalizer

#endif // PASCALANALIZER_IPPC_H
