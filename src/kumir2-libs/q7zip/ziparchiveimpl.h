#ifndef Q7ZIP_ZIPARCHIVEIMPL_H
#define Q7ZIP_ZIPARCHIVEIMPL_H

#include "archiveimpl.h"
#include "7zip/Archive/Zip/ZipHandler.h"

namespace Q7Zip {

struct ZipArchiveImpl : public ArchiveImpl
{
    ZipArchiveImpl(QIODevice* ioDevice);
    ~ZipArchiveImpl();

    bool open(QIODevice::OpenMode openMode) override;

    NArchive::NZip::CHandler * handler;
};

} // namespace Q7Zip

#endif // Q7ZIP_ZIPARCHIVEIMPL_H
