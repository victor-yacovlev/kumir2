#ifndef ANALIZER_SOURCEFILEINTERFACE_H
#define ANALIZER_SOURCEFILEINTERFACE_H

#include <QtCore>

namespace Shared { namespace Analizer {

class SourceFileInterface {

public /*types*/:
    struct Data
    {
        QString visibleText;
        QSet<int> protectedLineNumbers;
        QString hiddenText;
        bool hasHiddenText;
        QString sourceEncoding;
        QByteArray hiddenTextSignature;
        QString canonicalSourceLanguageName; // the same as file suffix after '.'
        QUrl sourceUrl;
    };

public /*methods*/:

    // Abstract methods to read and write from/to string in language-specific way

    virtual QString toString(const Data & data) const = 0;
    virtual Data fromString(const QString & string) const = 0;


    // Generic methods to read and write from/to raw byte array. Default implementation
    // assumes UTF-8 encoding with BOM mark, but it is possible to override

    inline virtual QByteArray toBytes(const Data & data, const QString & forceEncoding = "") const {
        QByteArray buffer;
        QTextStream stream(&buffer, QIODevice::WriteOnly);
        if (forceEncoding.length() > 0)
            stream.setCodec(forceEncoding.toLatin1().constData());
        else {
            stream.setCodec("UTF-8");
            stream.setGenerateByteOrderMark(true);
        }
        stream << toString(data);
        stream.flush();
        return buffer;
    }

    inline virtual Data fromBytes(const QByteArray & bytes, const QString & forceEncoding = "") const {
        QByteArray bytesCopy(bytes);
        QTextStream stream(&bytesCopy);
        if (forceEncoding.length() > 0)
            stream.setCodec(forceEncoding.toLatin1().constData());
        else {
            stream.setCodec("UTF-8");
            stream.setAutoDetectUnicode(true);
        }
        QString string = stream.readAll();
        return fromString(string);
    }

};

}}

Q_DECLARE_INTERFACE(Shared::Analizer::SourceFileInterface,
                    "kumir2.Analizer.SourceFileInterface")

#endif // ANALIZER_SOURCEFILEINTERFACE_H
