/****************************************************************************
**
** Some parts of code taken from QtCreator 2.8.1 by Digia Plc
**
** Licensed under the terms of GNU Lesser GPL Licensee 2.1
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
****************************************************************************/
#include "gdbmivalue.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>

#include <ctype.h>

namespace GdbMi {


uchar fromhex(uchar c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return 10 + c - 'a';
    if (c >= 'A' && c <= 'Z')
        return 10 + c - 'A';
    return -1;
}

void skipCommas(const char *&from, const char *to)
{
    while (*from == ',' && from != to)
        ++from;
}

QTextStream &operator<<(QTextStream &os, const GdbMiValue &mi)
{
    return os << mi.toString();
}

void GdbMiValue::parseResultOrValue(const char *&from, const char *to)
{
    while (from != to && isspace(*from))
        ++from;

    //qDebug() << "parseResultOrValue: " << QByteArray(from, to - from);
    parseValue(from, to);
    if (isValid()) {
        //qDebug() << "no valid result in " << QByteArray(from, to - from);
        return;
    }
    if (from == to || *from == '(')
        return;
    const char *ptr = from;
    while (ptr < to && *ptr != '=') {
        //qDebug() << "adding" << QChar(*ptr) << "to name";
        ++ptr;
    }
    m_name = QByteArray(from, ptr - from);
    from = ptr;
    if (from < to && *from == '=') {
        ++from;
        parseValue(from, to);
    }
}

QByteArray GdbMiValue::parseCString(const char *&from, const char *to)
{
    QByteArray result;
    //qDebug() << "parseCString: " << QByteArray(from, to - from);
    if (*from != '"') {
        qDebug() << "MI Parse Error, double quote expected";
        ++from; // So we don't hang
        return QByteArray();
    }
    const char *ptr = from;
    ++ptr;
    while (ptr < to) {
        if (*ptr == '"') {
            ++ptr;
            result = QByteArray(from + 1, ptr - from - 2);
            break;
        }
        if (*ptr == '\\') {
            ++ptr;
            if (ptr == to) {
                qDebug() << "MI Parse Error, unterminated backslash escape";
                from = ptr; // So we don't hang
                return QByteArray();
            }
        }
        ++ptr;
    }
    from = ptr;

    int idx = result.indexOf('\\');
    if (idx >= 0) {
        char *dst = result.data() + idx;
        const char *src = dst + 1, *end = result.data() + result.length();
        do {
            char c = *src++;
            switch (c) {
                case 'a': *dst++ = '\a'; break;
                case 'b': *dst++ = '\b'; break;
                case 'f': *dst++ = '\f'; break;
                case 'n': *dst++ = '\n'; break;
                case 'r': *dst++ = '\r'; break;
                case 't': *dst++ = '\t'; break;
                case 'v': *dst++ = '\v'; break;
                case '"': *dst++ = '"'; break;
                case '\\': *dst++ = '\\'; break;
                case 'x': {
                        c = *src++;
                        int chars = 0;
                        uchar prod = 0;
                        while (true) {
                            uchar val = fromhex(c);
                            if (val == uchar(-1))
                                break;
                            prod = prod * 16 + val;
                            if (++chars == 3 || src == end)
                                break;
                            c = *src++;
                        }
                        if (!chars) {
                            qDebug() << "MI Parse Error, unrecognized hex escape";
                            return QByteArray();
                        }
                        *dst++ = prod;
                        break;
                    }
                default:
                    {
                        int chars = 0;
                        uchar prod = 0;
                        forever {
                            if (c < '0' || c > '7') {
                                --src;
                                break;
                            }
                            prod = prod * 8 + c - '0';
                            if (++chars == 3 || src == end)
                                break;
                            c = *src++;
                        }
                        if (!chars) {
                            qDebug() << "MI Parse Error, unrecognized backslash escape";
                            return QByteArray();
                        }
                        *dst++ = prod;
                    }
            }
            while (src != end) {
                char c = *src++;
                if (c == '\\')
                    break;
                *dst++ = c;
            }
        } while (src != end);
        *dst = 0;
        result.truncate(dst - result.data());
    }

    return result;
}

void GdbMiValue::parseValue(const char *&from, const char *to)
{
    //qDebug() << "parseValue: " << QByteArray(from, to - from);
    switch (*from) {
        case '{':
            parseTuple(from, to);
            break;
        case '[':
            parseList(from, to);
            break;
        case '"':
            m_type = Const;
            m_data = parseCString(from, to);
            break;
        default:
            break;
    }
}


void GdbMiValue::parseTuple(const char *&from, const char *to)
{
    //qDebug() << "parseTuple: " << QByteArray(from, to - from);
    //QTC_CHECK(*from == '{');
    ++from;
    parseTuple_helper(from, to);
}

void GdbMiValue::parseTuple_helper(const char *&from, const char *to)
{
    skipCommas(from, to);
    //qDebug() << "parseTuple_helper: " << QByteArray(from, to - from);
    m_type = Tuple;
    while (from < to) {
        if (*from == '}') {
            ++from;
            break;
        }
        GdbMiValue child;
        child.parseResultOrValue(from, to);
        //qDebug() << "\n=======\n" << qPrintable(child.toString()) << "\n========\n";
        if (!child.isValid())
            return;
        m_children += child;
        skipCommas(from, to);
    }
}

void GdbMiValue::parseList(const char *&from, const char *to)
{
    //qDebug() << "parseList: " << QByteArray(from, to - from);
    //QTC_CHECK(*from == '[');
    ++from;
    m_type = List;
    skipCommas(from, to);
    while (from < to) {
        if (*from == ']') {
            ++from;
            break;
        }
        GdbMiValue child;
        child.parseResultOrValue(from, to);
        if (child.isValid())
            m_children += child;
        skipCommas(from, to);
    }
}

static QByteArray ind(int indent)
{
    return QByteArray(2 * indent, ' ');
}

void GdbMiValue::dumpChildren(QByteArray * str, bool multiline, int indent) const
{
    for (int i = 0; i < m_children.size(); ++i) {
        if (i != 0) {
            *str += ',';
            if (multiline)
                *str += '\n';
        }
        if (multiline)
            *str += ind(indent);
        *str += m_children.at(i).toString(multiline, indent);
    }
}

QByteArray GdbMiValue::escapeCString(const QByteArray &ba)
{
    QByteArray ret;
    ret.reserve(ba.length() * 2);
    for (int i = 0; i < ba.length(); ++i) {
        const uchar c = ba.at(i);
        switch (c) {
            case '\\': ret += "\\\\"; break;
            case '\a': ret += "\\a"; break;
            case '\b': ret += "\\b"; break;
            case '\f': ret += "\\f"; break;
            case '\n': ret += "\\n"; break;
            case '\r': ret += "\\r"; break;
            case '\t': ret += "\\t"; break;
            case '\v': ret += "\\v"; break;
            case '"': ret += "\\\""; break;
            default:
                if (c < 32 || c == 127) {
                    ret += '\\';
                    ret += ('0' + (c >> 6));
                    ret += ('0' + ((c >> 3) & 7));
                    ret += ('0' + (c & 7));
                } else {
                    ret += c;
                }
        }
    }
    return ret;
}

QByteArray GdbMiValue::toString(bool multiline, int indent) const
{
    QByteArray result;
    switch (m_type) {
        case Invalid:
            if (multiline)
                result += ind(indent) + "Invalid\n";
            else
                result += "Invalid";
            break;
        case Const:
            if (!m_name.isEmpty())
                result += m_name + '=';
            result += '"' + escapeCString(m_data) + '"';
            break;
        case Tuple:
            if (!m_name.isEmpty())
                result += m_name + '=';
            if (multiline) {
                result += "{\n";
                dumpChildren(&result, multiline, indent + 1);
                result += '\n' + ind(indent) + '}';
            } else {
                result += '{';
                dumpChildren(&result, multiline, indent + 1);
                result += '}';
            }
            break;
        case List:
            if (!m_name.isEmpty())
                result += m_name + '=';
            if (multiline) {
                result += "[\n";
                dumpChildren(&result, multiline, indent + 1);
                result += '\n' + ind(indent) + ']';
            } else {
                result += '[';
                dumpChildren(&result, multiline, indent + 1);
                result += ']';
            }
            break;
    }
    return result;
}

void GdbMiValue::fromString(const QByteArray &ba)
{
    const char *from = ba.constBegin();
    const char *to = ba.constEnd();
    parseResultOrValue(from, to);
}

void GdbMiValue::fromStringMultiple(const QByteArray &ba)
{
    const char *from = ba.constBegin();
    const char *to = ba.constEnd();
    parseTuple_helper(from, to);
}

GdbMiValue GdbMiValue::operator[](const char *name) const
{
    for (int i = 0, n = m_children.size(); i < n; ++i)
        if (m_children.at(i).m_name == name)
            return m_children.at(i);
    return GdbMiValue();
}

qulonglong GdbMiValue::toAddress() const
{
    QByteArray ba = m_data;
    if (ba.endsWith('L'))
        ba.chop(1);
    if (ba.startsWith('*') || ba.startsWith('@'))
        ba = ba.mid(1);
    return ba.toULongLong(0, 0);
}



//////////////////////////////////////////////////////////////////////////////////
//
// Decoding
//
//////////////////////////////////////////////////////////////////////////////////

static QString quoteUnprintableLatin1(const QByteArray &ba)
{
    QString res;
    char buf[10];
    for (int i = 0, n = ba.size(); i != n; ++i) {
        const unsigned char c = ba.at(i);
        if (isprint(c)) {
            res += QLatin1Char(c);
        } else {
            qsnprintf(buf, sizeof(buf) - 1, "\\%x", int(c));
            res += QLatin1String(buf);
        }
    }
    return res;
}

static QDate dateFromData(int jd)
{
    return jd ? QDate::fromJulianDay(jd) : QDate();
}

static QTime timeFromData(int ms)
{
    return ms == -1 ? QTime() : QTime(0, 0, 0, 0).addMSecs(ms);
}

//QString decodeData(const QByteArray &ba, int encoding)
//{
//    switch (encoding) {
//        case Unencoded8Bit: // 0
//            return quoteUnprintableLatin1(ba);
//        case Base64Encoded8BitWithQuotes: { // 1, used for QByteArray
//            const QChar doubleQuote(QLatin1Char('"'));
//            QString rc = doubleQuote;
//            rc += quoteUnprintableLatin1(QByteArray::fromBase64(ba));
//            rc += doubleQuote;
//            return rc;
//        }
//        case Base64Encoded16BitWithQuotes: { // 2, used for QString
//            const QChar doubleQuote(QLatin1Char('"'));
//            const QByteArray decodedBa = QByteArray::fromBase64(ba);
//            QString rc = doubleQuote;
//            rc += QString::fromUtf16(reinterpret_cast<const ushort *>
//                (decodedBa.data()), decodedBa.size() / 2);
//            rc += doubleQuote;
//            return rc;
//        }
//        case Base64Encoded32BitWithQuotes: { // 3
//            const QByteArray decodedBa = QByteArray::fromBase64(ba);
//            const QChar doubleQuote(QLatin1Char('"'));
//            QString rc = doubleQuote;
//            rc += QString::fromUcs4(reinterpret_cast<const uint *>
//                (decodedBa.data()), decodedBa.size() / 4);
//            rc += doubleQuote;
//            return rc;
//        }
//        case Base64Encoded16Bit: { // 4, without quotes (see 2)
//            const QByteArray decodedBa = QByteArray::fromBase64(ba);
//            return QString::fromUtf16(reinterpret_cast<const ushort *>
//                (decodedBa.data()), decodedBa.size() / 2);
//        }
//        case Base64Encoded8Bit: { // 5, without quotes (see 1)
//            return quoteUnprintableLatin1(QByteArray::fromBase64(ba));
//        }
//        case Hex2EncodedLatin1WithQuotes: { // 6, %02x encoded 8 bit Latin1 data
//            const QChar doubleQuote(QLatin1Char('"'));
//            const QByteArray decodedBa = QByteArray::fromHex(ba);
//            return doubleQuote + QString::fromLatin1(decodedBa, decodedBa.size()) + doubleQuote;
//        }
//        case Hex4EncodedLittleEndianWithQuotes: { // 7, %04x encoded 16 bit data
//            const QChar doubleQuote(QLatin1Char('"'));
//            const QByteArray decodedBa = QByteArray::fromHex(ba);
//            return doubleQuote + QString::fromUtf16(reinterpret_cast<const ushort *>
//                (decodedBa.data()), decodedBa.size() / 2) + doubleQuote;
//        }
//        case Hex8EncodedLittleEndianWithQuotes: { // 8, %08x encoded 32 bit data
//            const QChar doubleQuote(QLatin1Char('"'));
//            const QByteArray decodedBa = QByteArray::fromHex(ba);
//            return doubleQuote + QString::fromUcs4(reinterpret_cast<const uint *>
//                (decodedBa.data()), decodedBa.size() / 4) + doubleQuote;
//        }
//        case Hex2EncodedUtf8WithQuotes: { // 9, %02x encoded 8 bit UTF-8 data
//            const QChar doubleQuote(QLatin1Char('"'));
//            const QByteArray decodedBa = QByteArray::fromHex(ba);
//            return doubleQuote + QString::fromUtf8(decodedBa) + doubleQuote;
//        }
//        case Hex8EncodedBigEndian: { // 10, %08x encoded 32 bit data
//            const QChar doubleQuote(QLatin1Char('"'));
//            QByteArray decodedBa = QByteArray::fromHex(ba);
//            for (int i = 0; i < decodedBa.size() - 3; i += 4) {
//                char c = decodedBa.at(i);
//                decodedBa[i] = decodedBa.at(i + 3);
//                decodedBa[i + 3] = c;
//                c = decodedBa.at(i + 1);
//                decodedBa[i + 1] = decodedBa.at(i + 2);
//                decodedBa[i + 2] = c;
//            }
//            return doubleQuote + QString::fromUcs4(reinterpret_cast<const uint *>
//                (decodedBa.data()), decodedBa.size() / 4) + doubleQuote;
//        }
//        case Hex4EncodedBigEndianWithQuotes: { // 11, %04x encoded 16 bit data
//            const QChar doubleQuote(QLatin1Char('"'));
//            QByteArray decodedBa = QByteArray::fromHex(ba);
//            for (int i = 0; i < decodedBa.size(); i += 2) {
//                char c = decodedBa.at(i);
//                decodedBa[i] = decodedBa.at(i + 1);
//                decodedBa[i + 1] = c;
//            }
//            return doubleQuote + QString::fromUtf16(reinterpret_cast<const ushort *>
//                (decodedBa.data()), decodedBa.size() / 2) + doubleQuote;
//        }
//        case Hex4EncodedLittleEndianWithoutQuotes: { // 12, see 7, without quotes
//            const QByteArray decodedBa = QByteArray::fromHex(ba);
//            return QString::fromUtf16(reinterpret_cast<const ushort *>
//                (decodedBa.data()), decodedBa.size() / 2);
//        }
//        case Hex2EncodedLocal8BitWithQuotes: { // 13, %02x encoded 8 bit UTF-8 data
//            const QChar doubleQuote(QLatin1Char('"'));
//            const QByteArray decodedBa = QByteArray::fromHex(ba);
//            return doubleQuote + QString::fromLocal8Bit(decodedBa) + doubleQuote;
//        }
//        case JulianDate: { // 14, an integer count
//            const QDate date = dateFromData(ba.toInt());
//            return date.toString(Qt::TextDate);
//        }
//        case MillisecondsSinceMidnight: {
//            const QTime time = timeFromData(ba.toInt());
//            return time.toString(Qt::TextDate);
//        }
//        case JulianDateAndMillisecondsSinceMidnight: {
//            const int p = ba.indexOf('/');
//            const QDate date = dateFromData(ba.left(p).toInt());
//            const QTime time = timeFromData(ba.mid(p + 1 ).toInt());
//            return QDateTime(date, time).toString(Qt::TextDate);
//        }
//    }
//    qDebug() << "ENCODING ERROR: " << encoding;
//    return QCoreApplication::translate("Debugger", "<Encoding error>");
//}

}
