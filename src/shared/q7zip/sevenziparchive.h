#ifndef Q7ZIP_SEVENZIPARCHIVE_H
#define Q7ZIP_SEVENZIPARCHIVE_H

#include "archive.h"

namespace Q7Zip {

class EXPORT SevenZipArchive
        : public Archive
{
    Q_OBJECT
public:
    explicit SevenZipArchive(QIODevice * ioDevice, QObject *parent = 0);
};

} // namespace Q7Zip

#endif // Q7ZIP_SEVENZIPARCHIVE_H
