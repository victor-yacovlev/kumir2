#include "ziparchiveimpl.h"
#include "instream.h"

namespace Q7Zip {

ZipArchiveImpl::ZipArchiveImpl(QIODevice *ioDevice)
    : ArchiveImpl(ioDevice)
{
    handler = new NArchive::NZip::CHandler();
    inArchive_ = handler;
    outArchive_ = handler;
}

bool ZipArchiveImpl::open(QIODevice::OpenMode openMode)
{
    openMode_ = openMode;
    inArchiveStream_ = new InStream(archiveIODevice_);
    return S_OK == handler->Open(inArchiveStream_, 0, 0);
}

ZipArchiveImpl::~ZipArchiveImpl()
{
    delete handler;
}

} // namespace Q7Zip
