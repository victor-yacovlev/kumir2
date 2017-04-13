#ifndef Q7ZIP_OUTSTREAM_H
#define Q7ZIP_OUTSTREAM_H

#include <QBuffer>
#include "Common/MyCom.h"
#include "7zip/IStream.h"

namespace Q7Zip {

class OutStream
        : public IOutStream
        , public CMyUnknownImp
{
public:    
    explicit OutStream(QBuffer * ioDevice);
    STDMETHOD( Write (const void *data, UInt32 size, UInt32 *processedSize) ) override;
    STDMETHOD( Seek (Int64 offset, UInt32 seekOrigin, UInt64 *newPosition) ) override;
    STDMETHOD( SetSize (UInt64 size) ) override;

    virtual ~OutStream();

    MY_UNKNOWN_IMP1(IOutStream)

private:
    QBuffer * ioDevice_;
};

} // namespace Q7Zip

#endif // Q7ZIP_OUTSTREAM_H
