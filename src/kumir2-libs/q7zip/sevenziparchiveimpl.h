#ifndef Q7ZIP_SEVENZIPARCHIVEIMPL_H
#define Q7ZIP_SEVENZIPARCHIVEIMPL_H

#include "archiveimpl.h"
#include "7zip/Archive/7z/7zHandler.h"

namespace Q7Zip {

struct SevenZipArchiveImpl
        : public ArchiveImpl
{
    SevenZipArchiveImpl(QIODevice* ioDevice);
    ~SevenZipArchiveImpl();

    bool open(QIODevice::OpenMode openMode) override;

    NArchive::N7z::CHandler * handler;
};

} // namespace Q7Zip

#endif // Q7ZIP_SEVENZIPARCHIVEIMPL_H
