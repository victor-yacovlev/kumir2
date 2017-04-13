#ifndef Q7ZIP_INSTREAM_H
#define Q7ZIP_INSTREAM_H

#include <QIODevice>
#include "Common/MyCom.h"
#include "7zip/IStream.h"

namespace Q7Zip {

class InStream
        : public IInStream
        , public IStreamGetSize
        , public CMyUnknownImp
{
public:
    explicit InStream(QIODevice * ioDevice);
    STDMETHOD( Read (void *data, UInt32 size, UInt32 *processedSize) ) override;
    STDMETHOD( Seek (Int64 offset, UInt32 seekOrigin, UInt64 *newPosition) ) override;
    STDMETHOD( GetSize (UInt64 *size) ) override;
    MY_UNKNOWN_IMP2(IInStream, IStreamGetSize)
private:
    QIODevice* ioDevice_;
};

} // namespace Q7Zip

#endif // Q7ZIP_INSTREAM_H
