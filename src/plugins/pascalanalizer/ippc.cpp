#include "ippc.h"


namespace PascalAnalizer {

IPPC::IPPC(const QStringList & unitpaths, QObject *parent) :
    QProcess(parent)
{
    setOpenMode(QIODevice::ReadWrite | QIODevice::Unbuffered);
    setProcessChannelMode(QProcess::SeparateChannels);
    setReadChannel(QProcess::StandardOutput);
    codec_oem866 = QTextCodec::codecForName("IBM866");
    ts.setDevice(this);
    ts.setCodec(codec_oem866);
    s_command = "ippc";
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(handleReadyStdErr()));
    connect(this, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleFinished(int,QProcess::ExitStatus)));
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleError(QProcess::ProcessError)));
    ensureProcessRunning();
    foreach (const QString & unitpath, unitpaths) {
        ts << "addunitpath " << unitpath << " \n";
    }
    ts.flush();
    waitForBytesWritten();
}

IPPC::~IPPC()
{
    kill();
    waitForFinished();
}

bool IPPC::ensureProcessRunning()
{
    if (state()!=QProcess::Running) {
        start(s_command);
        waitForStarted();
    }
    if (state()!=QProcess::Running) {
        qDebug() << "ippc process is not running!";
        qDebug() << errorString();
        return false;
    }
    return true;
}

LineProp IPPC::lineSyntax(const QString &text)
{

    LineProp lp(text.length(), LxTypeEmpty);
    if (text.trimmed().isEmpty()) {
        return lp;
    }

    if (!ensureProcessRunning()) {
        return lp;
    }

    QString command = "linesyntax " + screenString(text) + " \n";
//    qDebug() << "Wrote: " << command;
    ts << command;
    ts.flush();

    waitForBytesWritten();
    waitForReadyRead();
    QStringList lines = QString::fromAscii(readAllStandardOutput()).split("\n");
//    qDebug() << lines;
    QVector<LineProp> props = extractLineProps(lines, QStringList() << text);
    lp = props[0];
    return lp;
}

IPPC::CheckResult IPPC::processAnalisys(const QString &text)
{
    CheckResult result;
    if (text.trimmed().isEmpty())
        return result;
    if (!ensureProcessRunning())
        return result;
    QString command = "check "+sourceName()+" "+screenString(text)+" \n";
    qDebug() << "Wrote: " << command;
    ts << command;
    ts.flush();
    waitForBytesWritten();
    waitForReadyRead();
    QStringList lines = QString::fromAscii(readAllStandardOutput()).split("\n");
    qDebug() << lines;
    const QStringList textLines = text.split("\n", QString::KeepEmptyParts);
    result.lineProps = extractLineProps(lines, textLines).toList();
    result.ranks = extractIndentRanks(lines, textLines).toList();
    result.errors = extractErrors(lines, result.lineProps);
    return result;
}

QVector<LineProp> IPPC::extractLineProps(const QStringList &lines, const QStringList & textLines)
{
    static const QStringList baseTypes = QStringList()
            << "integer" << "real" << "boolean" << "string" << "char" << "pchar";

    QVector<LineProp> result(textLines.size());
    for (int i=0; i<textLines.size(); i++) {
        result[i].fill(LxTypeEmpty, textLines[i].length());
    }
    foreach (const QString &line, lines) {
        if (line.startsWith("token")) {
            QStringList terms = line.split(" ");
            if (terms.size()>=5) {
                int line = terms[1].toInt();
                if (line<0 || line>=textLines.size())
                    continue;
                int pos = terms[2].toInt();
                int len = terms[3].toInt();
                int index;
                LexemType pt = typeByString(terms[4]);
                if (pt==LxTypeEmpty || pt==LxTypeName) {
                    QString sub = textLines[line].mid(pos,len);
                    if (sub.length()>0 && sub[0].isDigit()) {
                        pt = LxConstInteger;
                    }
                    else if (sub.length()>=2 && sub[0]=='\'' && sub[sub.length()-1]=='\'') {
                        pt = LxConstLiteral;
                    }
                    else if (baseTypes.contains(sub.toLower())) {
                        pt = LxNameClass;
                    }
                }
                for (int j=0; j<=len; j++) {
                    index = pos+j;
                    if (index>=0 && index<textLines[line].length()) {
                        result[line][index] = pt;
                    }
                }
            }
        }
    }
    return result;
}

QVector<QPoint> IPPC::extractIndentRanks(const QStringList &lines, const QStringList & textLines)
{
    QVector<QPoint> result(textLines.size(), QPoint(0,0));
    QVector< QList<QPoint> > temp(textLines.size());
    foreach (const QString &line, lines) {
        if (line.startsWith("token")) {
            QStringList terms = line.split(" ", QString::SkipEmptyParts);
            if (terms.size()>=7) {
                int lineNo = terms[1].toInt();
                if (lineNo<0 || lineNo>=textLines.size())
                    continue;
                if (terms[5].startsWith("+"))
                    terms[5].remove("+");
                if (terms[6].startsWith("+"))
                    terms[6].remove("+");
                int start = terms[5].toInt();
                int end = terms[6].toInt();
                temp[lineNo] << QPoint(start, end);
            }
        }
    }
    for (int i=0; i<temp.size(); i++) {
        for (int j=0; j<temp[i].size(); j++) {
            if (j==0) {
                result[i].setX(temp[i][j].x());
                result[i].setY(temp[i][j].y());
            }
            else {
                result[i].setY(result[i].y()+temp[i][j].x());
                result[i].setY(result[i].y()+temp[i][j].y());
            }
        }
    }
    return result;
}

QList<Error> IPPC::extractErrors(const QStringList &lines, const QList<LineProp> &props)
{
    QList<Error> result;
    foreach (const QString & line, lines) {
        if (line.startsWith("error")) {
            QStringList terms = line.split(" ");
            if (terms.size()>=4) {
                Error err;
                err.line = terms[1].toInt();
                err.start = terms[2].toInt();
                for (int j=3; j<terms.size(); j++) {
                    if (!err.code.isEmpty())
                        err.code += " ";
                    err.code += terms[j];
                }
                if (err.code.length()>0) {
                    err.code[0] = err.code[0].toUpper();
                }
                err.len = 1;
                if (err.line>=0 && err.line<props.size()) {
                    LineProp lp = props[err.line];
                    if (err.start<lp.size()) {
                        LexemType my = lp[err.start];
                        for (int j=err.start+1; j<lp.size(); j++) {
                            if (my==lp[j]) {
                                err.len++;
                            }
                            else {
                                break;
                            }
                        }
                    }
                    result << err;
                }
            }
        }
    }

    return result;
}


void IPPC::handleReadyStdErr()
{
//    qDebug() << "RECEIVED : " << readAllStandardError();
}

void IPPC::handleFinished(int code, QProcess::ExitStatus stat)
{
    qDebug() << "Finished: " << code << " " << stat;
}

void IPPC::handleError(QProcess::ProcessError err)
{
    qDebug() << "Error: " << err;
}

LexemType IPPC::typeByString(const QString &s)
{
    if (s=="prikeyword") {
        return LxTypePrimaryKwd;
    }
    else if (s=="seckeyword") {
        return LxTypeSecondaryKwd;
    }
    else if (s=="operator") {
        return LxTypeEmpty;
    }
    else if (s=="typename") {
        return LxNameClass;
    }
    else if (s=="name") {
        return LxTypeName;
    }
    else {
        return LxTypeEmpty;
    }
}

QString IPPC::screenString(QString s)
{
    s.replace("\\", "\\\\");
    s.replace("\n", "\\n");
    return s;
}

QString IPPC::sourceName() const
{
    if (s_sourceName.isEmpty()) {
        return "dummy.pas";
    }
    else {
        return s_sourceName;
    }
}

} // namespace PascalAnalizer
