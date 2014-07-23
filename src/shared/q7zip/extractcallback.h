#ifndef Q7ZIP_EXTRACTCALLBACK_H
#define Q7ZIP_EXTRACTCALLBACK_H

#include <QString>

#include "Common/MyCom.h"
#include "7zip/IPassword.h"
#include "7zip/Archive/IArchive.h"
#include "7zip/IPassword.h"

namespace Q7Zip {

class ArchiveImpl;
class OutStream;

class ExtractCallback
        : public IArchiveExtractCallback
        , public ICryptoGetTextPassword
        , public CMyUnknownImp
{
public:
    explicit ExtractCallback(OutStream * outStream, const QString & password);
    STDMETHOD( GetStream (UInt32, ISequentialOutStream* *outStream, Int32) ) override;
    STDMETHOD( PrepareOperation(Int32) ) override;
    STDMETHOD( SetOperationResult(Int32 status) ) override;
    STDMETHOD( SetTotal(UInt64) ) override;
    STDMETHOD( SetCompleted(const UInt64*) ) override;
    STDMETHOD( CryptoGetTextPassword(BSTR *password) ) override;

    MY_UNKNOWN_IMP2(IArchiveExtractCallback, ICryptoGetTextPassword)

    ~ExtractCallback();

private:
    OutStream * outStream_;
    QString password_;
};

} // namespace Q7Zip

#endif // Q7ZIP_EXTRACTCALLBACK_H
