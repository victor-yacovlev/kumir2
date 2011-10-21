#include "kumfile.h"


QString KumFile::toString(const Data &data)
{
    QString result;
    const QStringList lines = data.visibleText.split("\n", QString::KeepEmptyParts);
    for (int i=0; i<lines.count(); i++) {
        result += lines[i];
        if (data.protectedLineNumbers.contains(i)) {
            result += "|@protected";
        }
        if (i<lines.count()-1)
            result += "\n";
    }
    const QStringList hiddenLines = !data.hiddenText.isEmpty()? data.hiddenText.split("\n", QString::KeepEmptyParts) : QStringList();
    if (!result.isEmpty())
        result += "\n";
    for (int i=0; i<hiddenLines.count(); i++) {
        result += hiddenLines[i];
        result += "|@hidden";
        if (i<hiddenLines.count()-1)
            result += "\n";
    }
    return result;
}


QDataStream & operator <<(QDataStream & ds, const KumFile::Data & data)
{
    QByteArray buffer;
    QTextStream ts(&buffer);
    ts.setCodec("UTF-8");
    ts.setGenerateByteOrderMark(true);
    ts << KumFile::toString(data);
    ts.flush();
    ds.writeRawData(buffer.constData(), buffer.size());
    return ds;
}

KumFile::Data KumFile::fromString(const QString &s)
{
    const QStringList lines = s.split("\n", QString::KeepEmptyParts);
    KumFile::Data data;
    data.hasHiddenText = false;
    int lineNo = -1;
    for (int i=0; i<lines.count(); i++) {
        QString line = lines[i];
        while (line.startsWith(" "))
            line.remove(0, 1);
        if (line.endsWith("|@hidden")) {
            data.hasHiddenText = true;
            if (!data.hiddenText.isEmpty() && data.visibleText.isEmpty())
                data.hiddenText += "\n";
            data.hiddenText += line.left(line.length()-8);
            if (i<lines.count()-1 && lines[i+1].endsWith("|@hidden"))
                data.hiddenText += "\n";
        }
        else {
            lineNo ++;
            if (line.endsWith("|@protected")) {
                data.protectedLineNumbers.insert(lineNo);
                data.visibleText += line.left(line.length()-11);
            }
            else {
                data.visibleText += line;
            }
            if (i<lines.count()-1 && !lines[i+1].endsWith("|@hidden"))
                data.visibleText += "\n";
        }
    }
    return data;
}

bool hasWrongSymbols(const QString & s) {
    static const QString CyrillicAlphabet =
            QString::fromUtf8("абвгдеёжзийклмнопрстуфхцчшщъыьэюя");
    unsigned short code = 0;
    foreach (const QChar & ch, s) {
        code = ch.unicode();
        if (code==0) {
            return true;
        }
        if (code>127) {
            if (!CyrillicAlphabet.contains(ch, Qt::CaseInsensitive))
                return true;
        }
    }

    return false;
}

QString KumFile::readRawDataAsString(QByteArray rawData)
{
    QTextStream ts(rawData, QIODevice::ReadOnly);
    ts.setCodec("UTF-16");
    ts.setAutoDetectUnicode(true);
    return ts.readAll();
//    static const QStringList CodecsToTry = QStringList()
//         << "UTF-16" << "UTF-8" << "CP1251" << "KOI8-R" << "IBM866";
//    QTextCodec * currentCodec = 0;
//    QTextDecoder * decoder = 0;
//    QString currentCodecName;
//    QString s;
//    for (int i=0; i<CodecsToTry.size(); i++) {
//        currentCodecName = CodecsToTry[i];
//        currentCodec = QTextCodec::codecForName(currentCodecName.toAscii().constData());
//        decoder = currentCodec->makeDecoder(QTextCodec::ConvertInvalidToNull);
//        s = decoder->toUnicode(rawData);
//        if (!hasWrongSymbols(s)) {
//            return s;
//        }
//    }
//    return "| Incorrect character encoding. I can't load this file\n";
}

QDataStream & operator >>(QDataStream & ds, KumFile::Data & data)
{
    QByteArray buffer;
    char * bb = (char*)calloc(65536, sizeof(char));

    int cnt = 0;
    while (!ds.atEnd()) {
        cnt = ds.readRawData(bb, 65536);
        if (cnt>0) {
            buffer.append(bb, cnt);
        }
    }
    const QString s = KumFile::readRawDataAsString(buffer);
    data = KumFile::fromString(s);
    return ds;
}


