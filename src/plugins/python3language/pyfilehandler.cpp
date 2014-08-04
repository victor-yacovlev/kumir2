#include "pyfilehandler.h"

namespace Python3Language {

PyFileHandler::PyFileHandler(QObject *parent)
    : QObject(parent)
{
}

static const QString ProtectedLine = "# -*- kumir: protected line -*-";
static const QString HiddentTextSignature = "# -*- kumir: hidden text signature: ";
static const QString BeginHiddenText = "# -*- kumir: begin hidden text -*-";

QString PyFileHandler::toString(const Data &data) const
{
    QStringList visibleLines = data.visibleText.split("\n", QString::KeepEmptyParts);
    for (int i=0; i<visibleLines.size(); i++) {
        QString & line = visibleLines[i];
        if (data.protectedLineNumbers.contains(i)) {
            line += "  " + ProtectedLine;
        }
    }
    QString result = visibleLines.join("\n");
    if (data.hasHiddenText) {
        if (data.hiddenTextSignature.length() > 0) {
            result += "\n" + HiddentTextSignature;
            result += data.hiddenTextSignature.toHex().toLower();
            result += " -*-";
        }
        result += "\n" + BeginHiddenText + "\n";
        result += data.hiddenText;
    }
    return result;
}

SourceFileInterface::Data PyFileHandler::fromString(const QString &string) const
{
    SourceFileInterface::Data result;
    result.hasHiddenText = false;
    QString & currentBlock = result.visibleText;
    QStringList lines = string.split("\n", QString::KeepEmptyParts);
    Q_FOREACH(const QString & sourceLine, lines) {
        int protectedBegin = sourceLine.lastIndexOf(ProtectedLine);
        QString line;
        if (-1==protectedBegin)
            line = sourceLine;
        else
            line = sourceLine.left(protectedBegin);
        while (line.endsWith(' ')) {
            line = line.left(line.length()-1);
        }

        line = line.replace('\t', "    ");

        if (line.startsWith(HiddentTextSignature)) {
            int signatureStart = HiddentTextSignature.length();
            int signatureEnd = line.lastIndexOf("-*-");
            QByteArray signatureHex = line.mid(signatureStart, signatureEnd-signatureStart).toLatin1();
            result.hiddenTextSignature = QByteArray::fromHex(signatureHex);
        }
        else if (line.startsWith(BeginHiddenText)) {
            currentBlock = result.hiddenText;
            result.hasHiddenText = true;
        }
        else {
            if (currentBlock.length() > 0)
                currentBlock += "\n";
            currentBlock += line;
        }
    }
    return result;
}

QString PyFileHandler::findEncodingDeclaration(const QString &source)
{
    // PEP-0263 three variants to declare encoding
    // # coding=<encoding name>
    // # -*- coding: <encoding name -*-
    // # vim: set fileencoding=<encoding name> :

    const QStringList lines = source.split("\n");
    static const QRegExp rx1("^coding\\s*=\\s*(\\S+)$");
    static const QRegExp rx2("^-\\*-\\s*coding:\\s*(\\S+)\\s*-\\*-$");
    static const QRegExp rx3("^vim:\\s*set\\s+fileencoding\\s*=\\s*(\\S+)\\s*:$");

    QString codec;
    Q_FOREACH (QString line, lines) {
        line = line.trimmed();
        if (line.startsWith("#")) {
            line.remove(0, 1);
            line = line.trimmed();
            if (rx1.exactMatch(line)) {
                codec = rx1.cap(1);
                break;
            }
            if (rx2.exactMatch(line)) {
                codec = rx2.cap(1);
                break;
            }
            if (rx3.exactMatch(line)) {
                codec = rx3.cap(1);
                break;
            }
        }
    }
    return codec.toUpper();
}

SourceFileInterface::Data PyFileHandler::fromBytes(const QByteArray &rawData, const QString & forceEncoding) const
{
    QString codec;

    if (forceEncoding.length() > 0) {
        codec = forceEncoding;
    }
    else {
        codec = findEncodingDeclaration(QString::fromLatin1(rawData));
        if (codec.isEmpty())
            codec = "UTF-8";
    }
    QByteArray buffer(rawData);
    QTextStream ts(&buffer);
    ts.setCodec(codec.toLatin1().data());
    ts.setAutoDetectUnicode(true);
    QString string = ts.readAll();
    return fromString(string);
}

QByteArray PyFileHandler::toBytes(const Data &data, const QString &forceEncoding) const
{
    const QString dataString = toString(data);
    QByteArray buffer;
    QTextStream stream(&buffer, QIODevice::WriteOnly);
    if (forceEncoding.length() > 0)
        stream.setCodec(forceEncoding.toLatin1().constData());
    else {
        QString codecName = findEncodingDeclaration(dataString);
        if (codecName.isEmpty())
            codecName = "UTF-8";
        stream.setCodec(codecName.toLatin1());
        if (codecName.startsWith("UTF") || codecName.startsWith("UCS"))
            stream.setGenerateByteOrderMark(true);
    }
    stream << dataString;
    stream.flush();
    return buffer;
}

} // namespace Python3Language
