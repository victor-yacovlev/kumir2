#ifndef Q7ZIP_ARCHIVEIMPL_H
#define Q7ZIP_ARCHIVEIMPL_H

#include <wchar.h>
#include <QtGlobal>
#include <QStringList>
#include <QIODevice>
#include <QString>
#include <QMap>

#ifndef Q_OS_WIN32
#   define LPSTR char*
#   define WINAPI
#endif

#include "7zip/Archive/IArchive.h"

#include "archive.h"

namespace Q7Zip {

struct ArchiveImpl
{
public:
    explicit ArchiveImpl(QIODevice* ioDevice);
    virtual bool open(QIODevice::OpenMode openMode) = 0;
    void close();
    QStringList allFilesList(const QString &password) const;
    QIODevice * openFileEntry(const QString & entryPath, QIODevice::OpenMode openMode, const QString & password, QObject * entryParentObject);

    virtual QMap<QString,quint32> readArchiveContents(const QString & password) const;
    virtual QIODevice * extractItem(quint32 index, const QString & password, QObject * entryParentObject);
    virtual ~ArchiveImpl();

protected:
    QIODevice* archiveIODevice_;
    IInArchive * inArchive_;
    IOutArchive * outArchive_;
    QIODevice::OpenMode openMode_;
    IInStream * inArchiveStream_;
    IOutStream * outArchiveStream_;

private:
    mutable QMap<QString,quint32> archiveContents_;
};

} // namespace Q7Zip

#endif // Q7ZIP_ARCHIVEIMPL_H
