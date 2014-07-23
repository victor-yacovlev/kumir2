#include "sevenziparchive.h"
#include "sevenziparchiveimpl.h"

namespace Q7Zip {

SevenZipArchive::SevenZipArchive(QIODevice * ioDevice, QObject *parent)
    : Archive(ioDevice, parent)
{
    impl = new SevenZipArchiveImpl(ioDevice);
}


} // namespace Q7Zip
