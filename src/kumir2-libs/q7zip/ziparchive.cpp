#include "ziparchive.h"
#include "ziparchiveimpl.h"

namespace Q7Zip {

ZipArchive::ZipArchive(QIODevice * ioDevice, QObject *parent)
    : Archive(ioDevice, parent)
{
    impl = new ZipArchiveImpl(ioDevice);
}

} // namespace Q7Zip
