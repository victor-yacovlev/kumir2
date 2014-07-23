#include <QBuffer>
#include <QDir>

#include "archiveimpl.h"

#include "Windows/PropVariant.h"
#include "Windows/PropVariantConversions.h"
#include "Windows/PropVariantUtils.h"
#include "7zip/Common/RegisterArc.h"

#include "extractcallback.h"
#include "outstream.h"

// dummy method for p7zip
extern void RegisterArc(const CArcInfo *) {
    // Do nothing
}

namespace Q7Zip {

ArchiveImpl::ArchiveImpl(QIODevice* ioDevice)
{
    inArchive_ = 0;
    outArchive_ = 0;
    inArchiveStream_ = 0;
    outArchiveStream_ = 0;
    archiveIODevice_ = ioDevice;    
}


void ArchiveImpl::close()
{
    if (inArchive_ && inArchiveStream_) {
        inArchive_->Close();
        inArchive_ = 0;
    }
}

QStringList ArchiveImpl::allFilesList(const QString &password) const
{
    if (archiveContents_.isEmpty()) {
        archiveContents_ = readArchiveContents(password);
    }
    return archiveContents_.keys();
}

QIODevice* ArchiveImpl::openFileEntry(const QString & entryPath, QIODevice::OpenMode openMode, const QString &password, QObject * entryParentObject)
{
    QIODevice * result = 0;
    if (QIODevice::ReadOnly == openMode && archiveContents_.contains(entryPath)) {
        result = extractItem(archiveContents_[entryPath], password, entryParentObject);
    }
    return result;
}

QMap<QString,quint32> ArchiveImpl::readArchiveContents(const QString &password) const
{
    QMap<QString,quint32> result;
    UInt32 count = 0;
    if (inArchive_) {
        if (S_OK == inArchive_->GetNumberOfItems(&count)) {
            for (UInt32 i=0u; i<count; i++) {
                NWindows::NCOM::CPropVariant nameValue;
                NWindows::NCOM::CPropVariant isDirValue;
                NWindows::NCOM::CPropVariant sizeDirValue;
                PROPID namePropId = kpidPath;
                PROPID isDirPropId = kpidIsDir;
                PROPID sizePropId = kpidSize;
                inArchive_->GetProperty(i, namePropId, &nameValue);
                inArchive_->GetProperty(i, isDirPropId, &isDirValue);
                inArchive_->GetProperty(i, sizePropId, &sizeDirValue);
                if (VT_BSTR == nameValue.vt && VT_BOOL == isDirValue.vt) {
                    bool isDir = isDirValue.bVal;
                    BSTR nameStr = nameValue.bstrVal;
                    const QString path =
                            QDir::fromNativeSeparators(
                                QString::fromWCharArray(nameStr
                                                        )
                                );
                    if (!isDir) {
                        result.insert(path, i);
                    }
                }

            }
        }
    }
    return result;
}

QIODevice* ArchiveImpl::extractItem(quint32 index, const QString &password, QObject * entryParentObject)
{
    if (archiveContents_.isEmpty()) {
        archiveContents_ = readArchiveContents(password);
    }
    const UInt32 indeces[1] = { index };
    QBuffer * ioBuffer = new QBuffer(entryParentObject);
    OutStream * outStream = new OutStream(ioBuffer);
    outStream->AddRef();
    ExtractCallback * extractCallback = new ExtractCallback(outStream, password);
    extractCallback->AddRef();
    inArchive_->Extract(indeces, 1u, 0, extractCallback);
    extractCallback->Release();
    outStream->Release();
    return ioBuffer;
}

ArchiveImpl::~ArchiveImpl()
{
    close();
}

} // namespace Q7Zip
