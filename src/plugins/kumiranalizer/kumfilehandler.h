#ifndef KUMIRANALIZER_KUMFILEHANDLER_H
#define KUMIRANALIZER_KUMFILEHANDLER_H

#include <QObject>
#include <kumir2/analizer_sourcefileinterface.h>

namespace KumirAnalizer {

using namespace Shared::Analizer;

class KumFileHandler
        : public QObject
        , public SourceFileInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Analizer::SourceFileInterface)

public:
    explicit KumFileHandler(QObject *parent = 0);

    QString toString(const Data & data) const;
    Data fromString(const QString & string) const;

    // KumFile implementation shoud use UTF-16 encoding if no BOM present
    // to allows compatibility with Kumir 1.x
    inline Data fromBytes(const QByteArray & bytes, const QString & forceEncoding = "") const {
        QByteArray bytesCopy(bytes);
        QTextStream stream(&bytesCopy);
        if (forceEncoding.length() > 0)
            stream.setCodec(forceEncoding.toLatin1().constData());
        else {
            stream.setCodec("UTF-16");
            stream.setAutoDetectUnicode(true);
        }
        QString string = stream.readAll();
        return fromString(string);
    }
private:
    static QString& trimLeadingSpaces(QString &line);
};

} // namespace KumirAnalizer

#endif // KUMIRANALIZER_KUMFILEHANDLER_H
