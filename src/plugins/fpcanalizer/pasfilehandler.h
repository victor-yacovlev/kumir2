#ifndef FPCANALIZER_PASFILEHANDLER_H
#define FPCANALIZER_PASFILEHANDLER_H

#include <QObject>
#include "interfaces/analizer_sourcefileinterface.h"

namespace FpcAnalizer {

using namespace Shared::Analizer;

class PasFileHandler
    : public QObject
    , public SourceFileInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Analizer::SourceFileInterface)
public:
    explicit PasFileHandler(QObject *parent = 0);

    QString toString(const Data &data) const;
    Data fromString(const QString &string) const;

    // Use CP866 encoding to allow compatibility with old TurboPascal sources

    inline virtual QByteArray toBytes(const Data & data, const QString & forceEncoding = "") const {
        QByteArray buffer;
        QTextStream stream(&buffer, QIODevice::WriteOnly);
        if (forceEncoding.length() > 0)
            stream.setCodec(forceEncoding.toLatin1().constData());
        else {
            stream.setCodec("CP866");
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
            stream.setCodec("CP866");
            stream.setAutoDetectUnicode(true);
        }
        QString string = stream.readAll();
        return fromString(string);
    }

};

} // namespace FpcAnalizer

#endif // FPCANALIZER_PASFILEHANDLER_H
