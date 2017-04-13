#include "instream.h"

namespace Q7Zip {

InStream::InStream(QIODevice* ioDevice)
    : IInStream()
    , ioDevice_(ioDevice)
{
    if (!ioDevice->isOpen()) {
        ioDevice->open(QIODevice::ReadOnly);
    }
}

LONG InStream::Read(void *data, UInt32 size, UInt32 *processedSize)
{
    if (processedSize)
        *processedSize = 0;
    if (!ioDevice_->isReadable())
        return E_FAIL;
    char* buffer = reinterpret_cast<char*>(data);
    qint64 readBytes = ioDevice_->read(buffer, qint64(size));
    if (-1 == readBytes)
        return E_FAIL;
    if (processedSize)
        *processedSize = UInt32(readBytes);
    return S_OK;
}

LONG InStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
{
    if (newPosition)
        *newPosition = UInt64(ioDevice_->pos());
    if (!ioDevice_->isReadable() || ioDevice_->isSequential())
        return E_FAIL;
    qint64 pos = ioDevice_->pos();
    if (SEEK_SET == seekOrigin)
        pos = offset;
    else if (SEEK_CUR == seekOrigin)
        pos += offset;
    else if (SEEK_END == seekOrigin)
        pos = ioDevice_->size() - offset;
    if (ioDevice_->seek(pos)) {
        if (newPosition)
            *newPosition = UInt64(ioDevice_->pos());
        return S_OK;
    }
    else {
        return E_FAIL;
    }
}

LONG InStream::GetSize(UInt64 *size)
{
    if (ioDevice_->isSequential()) {
        *size = 0UL;
        return E_FAIL;
    }
    else {
        *size = UInt64(ioDevice_->size());
        return S_OK;
    }
}

} // namespace Q7Zip
