#include "sevenziparchiveimpl.h"
#include "instream.h"

namespace Q7Zip {

SevenZipArchiveImpl::SevenZipArchiveImpl(QIODevice *ioDevice)
    : ArchiveImpl(ioDevice)
{
    handler = new NArchive::N7z::CHandler();
    inArchive_ = handler;
    outArchive_ = handler;
}

bool SevenZipArchiveImpl::open(QIODevice::OpenMode openMode)
{
    openMode_ = openMode;
    inArchiveStream_ = new InStream(archiveIODevice_);
    return S_OK == handler->Open(inArchiveStream_, 0, 0);
}

SevenZipArchiveImpl::~SevenZipArchiveImpl()
{
    delete handler;
}

} // namespace Q7Zip
