#include "pasfilehandler.h"

namespace FpcAnalizer {

static const QString ProtectedLines = "{ -*- kumir: protected lines: ";
static const QString HiddentTextSignature = "{ -*- kumir: hidden text signature: ";
static const QString BeginHiddenText = "{ -*- kumir: begin hidden text -*-";

PasFileHandler::PasFileHandler(QObject *parent) :
    QObject(parent)
{
}

QString PasFileHandler::toString(const Data &data) const
{
    QString result = data.visibleText;
    if (data.hasHiddenText) {
        if (data.hiddenTextSignature.length() > 0) {
            result += "\n" + HiddentTextSignature;
            result += data.hiddenTextSignature.toHex().toLower();
            result += " -*- }";
        }
        result += "\n" + BeginHiddenText + " }\n";
        result += data.hiddenText;
    }
    if (!data.protectedLineNumbers.isEmpty()) {
        result += "\n" + ProtectedLines;
        Q_FOREACH(int value, data.protectedLineNumbers.toList()) {
            result += QString::number(value) + " ";
        }
        result += "-*- }";
    }
    return result;
}

SourceFileInterface::Data PasFileHandler::fromString(const QString &string) const
{
    SourceFileInterface::Data result;
    result.hasHiddenText = false;
    QString * currentBlock = &result.visibleText;
    QStringList lines = string.split("\n", QString::KeepEmptyParts);
    Q_FOREACH(const QString & sourceLine, lines) {

        QString line = sourceLine;
        line = line.replace('\t', "    ");

        if (line.startsWith(HiddentTextSignature)) {
            int signatureStart = HiddentTextSignature.length();
            int signatureEnd = line.lastIndexOf("-*-");
            QByteArray signatureHex = line.mid(signatureStart, signatureEnd-signatureStart).toLatin1();
            result.hiddenTextSignature = QByteArray::fromHex(signatureHex);
        }
        else if (line.startsWith(BeginHiddenText)) {
            currentBlock = &result.hiddenText;
            result.hasHiddenText = true;
        }
        else if (line.startsWith(ProtectedLines)) {
            int rangeStart = ProtectedLines.length();
            int rangeEnd = line.lastIndexOf("-*-");
            QStringList numbers = line.mid(rangeStart, rangeEnd-rangeStart).split(" ", QString::SkipEmptyParts);
            QStringList visibleLines = result.visibleText.split("\n", QString::KeepEmptyParts);
            Q_FOREACH(const QString & number, numbers) {
                int value = number.toInt();
                if (value < visibleLines.size())
                    result.protectedLineNumbers.insert(value);
            }
        }
        else {
            if (currentBlock->length() > 0)
                currentBlock->append("\n");
            currentBlock->append(line);
        }
    }
    return result;
}

} // namespace FpcAnalizer
