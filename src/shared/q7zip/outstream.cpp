#include "outstream.h"

namespace Q7Zip {

OutStream::OutStream(QBuffer * ioDevice)
    : IOutStream()
    , CMyUnknownImp()
    , ioDevice_(ioDevice)
{
    if (!ioDevice_->isOpen()) {
        ioDevice_->open(QIODevice::WriteOnly);
    }
}

OutStream::~OutStream()
{

}

LONG OutStream::Write(const void *data, UInt32 size, UInt32 *processedSize)
{
    const char * buffer = reinterpret_cast<const char*>(data);
    if (processedSize)
        *processedSize = 0U;
    qint64 bytesWritten = ioDevice_->write(buffer, qint64(size));
    if (-1 == bytesWritten)
        return E_FAIL;
    else {
        if (processedSize)
            *processedSize = UInt32(bytesWritten);
        return S_OK;
    }
}

LONG OutStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition)
{
    if (newPosition)
        *newPosition = UInt64(ioDevice_->pos());
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

LONG OutStream::SetSize(UInt64 size)
{
    ioDevice_->buffer().resize(int(size));
    return S_OK;
}

} // namespace Q7Zip
