#include "kumfilehandler.h"

namespace KumirAnalizer {

KumFileHandler::KumFileHandler(QObject *parent) :
    QObject(parent)
{
}


QString KumFileHandler::toString(const Data &data) const
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
    if (!data.hiddenTextSignature.isEmpty()) {
        result += QString::fromLatin1("|@signature %1|@hidden\n")
                .arg(QString::fromLatin1(data.hiddenTextSignature.toBase64()));
    }
    return result;
}

SourceFileInterface::Data KumFileHandler::fromString(const QString &s) const
{
    const QStringList lines = s.split("\n", QString::KeepEmptyParts);
    Data data;
    data.hasHiddenText = false;
    int lineNo = -1;
    for (int i=0; i<lines.count(); i++) {
        QString line = lines[i];

        trimLeadingSpaces(line);

        if (line.startsWith("|@signature ") && line.endsWith("|@hidden"))
        {
            const QString b64 = line.mid(12, line.length()-20);
            data.hiddenTextSignature = QByteArray::fromBase64(b64.toLatin1());
        }
        else if (line.endsWith("|@hidden")) {
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

QString &KumFileHandler::trimLeadingSpaces(QString &line)
{
    // 1. Find not screened comment marker
    QChar literalQuote = QChar(0);
    int commentStartPosition = -1;
    int spacesBeforeFirstSymbol = 0;
    for (int i=0; i<line.length(); ++i) {
        const QChar current = line.at(i);
        if (i==spacesBeforeFirstSymbol && current.isSpace()) {
            ++spacesBeforeFirstSymbol;
        }
        else if (literalQuote.isNull() && ('"'==current || '\''==current)) {
            literalQuote = current;
        }
        else if ( ! literalQuote.isNull() && current == literalQuote ) {
            literalQuote = QChar(0);
        }
        else if (literalQuote.isNull() && ('|'==current || '!'==current)) {
            commentStartPosition = i;
            break;
        }
    }
    if (-1 != commentStartPosition && commentStartPosition == spacesBeforeFirstSymbol) {
        // Do nothing in case of preserve comments formatting
    }
    else if (spacesBeforeFirstSymbol > 0) {
        line.remove(0, spacesBeforeFirstSymbol);
    }
    return line;
}



} // namespace KumirAnalizer
