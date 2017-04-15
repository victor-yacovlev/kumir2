#include "extractcallback.h"
#include "outstream.h"

namespace Q7Zip {

ExtractCallback::ExtractCallback(OutStream *outStream, const QString &password)
    : IArchiveExtractCallback()
    , outStream_(outStream)
    , password_(password)
{
}

LONG ExtractCallback::GetStream(UInt32, ISequentialOutStream* *outStream, Int32)
{
    *outStream = outStream_;
    outStream_->AddRef();
    return S_OK;
}

LONG ExtractCallback::PrepareOperation(Int32)
{
    outStream_->SetSize(0);
    outStream_->Seek(0, SEEK_SET, 0);
    return S_OK;
}

LONG ExtractCallback::SetOperationResult(Int32 status)
{
    return S_OK;
}

LONG ExtractCallback::CryptoGetTextPassword(BSTR *password)
{
    *password = ::SysAllocString(password_.toStdWString().c_str());
    return S_OK;
}

ExtractCallback::~ExtractCallback()
{

}

// not used
LONG ExtractCallback::SetCompleted(const UInt64 *)
{
    return S_OK;
}

// not used
LONG ExtractCallback::SetTotal(UInt64)
{
    return S_OK;
}


} // namespace Q7Zip
