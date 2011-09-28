#include "ippc.h"
#define READ_TIMEOUT 1

namespace PascalAnalizer {

IPPC::IPPC(const QStringList & unitpaths, QObject *parent) :
    QObject(parent)
{
    process = new QProcess(this);
//    process->setOpenMode(QIODevice::ReadWrite | QIODevice::Unbuffered);
    process->setProcessChannelMode(QProcess::SeparateChannels);
    process->setReadChannel(QProcess::StandardOutput);
    codec_oem866 = QTextCodec::codecForName("IBM866");
    ts.setDevice(process);
    ts.setCodec(codec_oem866);
    s_command = "ippc";
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(handleReadyStdErr()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(handleFinished(int,QProcess::ExitStatus)));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(handleError(QProcess::ProcessError)));
    ensureProcessRunning();
    foreach (const QString & unitpath, unitpaths) {
        ts << "addunitpath " << unitpath << " \n";
    }
    ts.flush();
    process->waitForBytesWritten();

}

IPPC::~IPPC()
{
    process->kill();
    process->waitForFinished();
    process->deleteLater();
}

bool IPPC::ensureProcessRunning()
{
    if (process->state()!=QProcess::Running) {
        process->start(s_command);
        process->waitForStarted();
    }
    if (process->state()!=QProcess::Running) {
        qDebug() << "ippc process is not running!";
        qDebug() << process->errorString();
        return false;
    }
    return true;
}


LineProp IPPC::lineSyntax(const QString &text, const Names & names1, const Names & names2)
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

    process->waitForBytesWritten();

    QStringList lines = readAllLines();

//    qDebug() << lines;
    QVector<LineProp> props = extractLineProps(lines, QStringList() << text, names1, names2);
    lp = props[0];
    return lp;
}

QStringList IPPC::readAllLines()
{
    QStringList result;
    QString line;
    int bufferSize = 0;
    do {
        process->waitForReadyRead(READ_TIMEOUT);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        bufferSize = process->bytesAvailable();
        if (process->state()!=QProcess::Running)
            break;
        if (bufferSize) {
            line = codec_oem866->toUnicode(process->readLine()).trimmed();
            result << line;
        }
    } while (line!="end");
    return result;
}

IPPC::CheckResult IPPC::processAnalisys(const QString &text, const Names &oldNames, const Names & usedNames, bool fetchTables)
{
    CheckResult result;
    if (text.trimmed().isEmpty()) {
        return result;
    }
    if (!ensureProcessRunning()) {
        return result;
    }
    QString command = fetchTables? "checkandgettables " : "check ";
    command += screenString(text)+" \n";
//    qDebug() << "Wrote: " << command;


    ts << command;
    ts.flush();
    process->waitForBytesWritten();
    QStringList lines = readAllLines();

//    qDebug() << lines;
    const QStringList textLines = text.split("\n", QString::KeepEmptyParts);
    bool hasError = false;
    foreach (const QString & line, lines) {
        if (line.startsWith("error")) {
            hasError = true;
            break;
        }
    }

    Names newNames1 = extractNames(lines);
    Names newNames2 = extractUsedNames(lines);
    Names names1 = hasError? oldNames : newNames1;
    Names names2 = hasError || !fetchTables? usedNames : newNames2;
    result.lineProps = extractLineProps(lines, textLines, names1, names2).toList();
    result.ranks = extractIndentRanks(lines, textLines).toList();
    result.errors = extractErrors(lines, result.lineProps);
    for (int i=0; i<result.errors.size(); i++) {
        int lineNo = result.errors[i].line;
        int pos = result.errors[i].start;
        int len = result.errors[i].len;
        if (lineNo>=0 && lineNo<result.lineProps.size()) {
            for (int j=0; j<len; j++) {
                result.lineProps[lineNo][pos+j] = LexemType(result.lineProps[lineNo][pos+j] | LxTypeError);
            }
        }
    }
    result.localnames = names1;
    result.usednames = names2;
    return result;
}

IPPC::Names IPPC::extractNames(const QStringList &lines)
{
    IPPC::Names result;
    foreach (const QString & line, lines) {
        if (line.startsWith("unitname local")) {
            result.units << line.mid(15).toLower();
        }
        else if (line.startsWith("procedure local")) {
            result.procedures << line.mid(16).toLower();
        }
        else if (line.startsWith("class local")) {
            result.classes << line.mid(12).toLower();
        }
    }

    return result;
}

IPPC::Names IPPC::extractUsedNames(const QStringList &lines)
{
    IPPC::Names result;
    foreach (const QString & line, lines) {
        if (line.startsWith("unitname uses")) {
            result.units << line.mid(14).toLower();
        }
        else if (line.startsWith("procedure uses")) {
            result.procedures << line.mid(15).toLower();
        }
        else if (line.startsWith("class uses")) {
            result.classes << line.mid(11).toLower();
        }
    }

    return result;
}

QVector<LineProp> IPPC::extractLineProps(const QStringList &lines, const QStringList & textLines, const Names &names1, const Names & names2)
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
                    else if (sub.length()>=2 && sub[0]=='\'') {
                        pt = LxConstLiteral;
                    }
                    else if (baseTypes.contains(sub.toLower()) || names1.classes.contains(sub.toLower()) || names2.classes.contains(sub.toLower())) {
                        pt = LxNameClass;
                    }
                }
                if (pt==LxTypeName) {
                    QString sub = textLines[line].mid(pos,len).toLower();
                    if (names1.units.contains(sub) || names2.units.contains(sub))
                        pt = LxNameModule;
                    else if (names1.procedures.contains(sub) || names2.procedures.contains(sub))
                        pt = LxNameAlg;
                }
                for (int j=0; j<=len; j++) {
                    index = pos+j;
                    if (index>=0 && index<textLines[line].length()) {
                        result[line][index] = pt;
                    }
                    if (pt==LxConstLiteral && pos+len+1<textLines[line].length() && textLines[line][pos+len+1]=='\'')
                        result[line][pos+len+1] = LxConstLiteral;
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
