#ifndef PYTHON3LANGUAGE_PYFILEHANDLER_H
#define PYTHON3LANGUAGE_PYFILEHANDLER_H

#include <QObject>
#include "interfaces/analizer_sourcefileinterface.h"

namespace Python3Language {

using namespace Shared::Analizer;

class PyFileHandler
        : public QObject
        , public SourceFileInterface
{
    Q_OBJECT
    Q_INTERFACES(Shared::Analizer::SourceFileInterface)
public:
    explicit PyFileHandler(QObject *parent = 0);

    QString toString(const Data & data) const;
    Data fromString(const QString & string) const;

    // PEP-0263 implementation
    Data fromBytes(const QByteArray & bytes, const QString & forceEncoding = "") const;
    QByteArray toBytes(const Data &data, const QString &forceEncoding = "") const;

private:
    static QString findEncodingDeclaration(const QString & source);

};

} // namespace Python3Language

#endif // PYTHON3LANGUAGE_PYFILEHANDLER_H
