#ifndef Q7ZIP_ZIPARCHIVE_H
#define Q7ZIP_ZIPARCHIVE_H

#include "archive.h"

namespace Q7Zip {

class EXPORT ZipArchive
        : public Archive
{
    Q_OBJECT
public:
    explicit ZipArchive(QIODevice * ioDevice, QObject *parent = 0);
};

} // namespace Q7Zip

#endif // Q7ZIP_ZIPARCHIVE_H
