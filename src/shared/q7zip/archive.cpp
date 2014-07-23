#include <QtGlobal>

#define INITGUID
#include "7zip/IStream.h"
#include "7zip/ICoder.h"
#include "7zip/IPassword.h"
#undef INITGUID

#ifdef Q_OS_WIN32
int g_CodePage = -1;
bool g_IsNT = true;
#endif

#include "archive.h"
#include "archiveimpl.h"


namespace Q7Zip {

Archive::Archive(QIODevice * ioDevice, QObject *parent)
    : QObject(parent)
    , impl(0)
{
}

bool Archive::open(QIODevice::OpenMode openMode)
{
    return impl? impl->open(openMode) : false;
}

void Archive::close()
{
    if (impl) impl->close();
}

QStringList Archive::allFilesList(const QString &password) const
{
    return impl? impl->allFilesList(password) : QStringList();
}

QIODevice* Archive::openFileEntry(const QString & entryPath, QIODevice::OpenMode openMode, const QString &password, QObject * entryParentObject)
{
    if (!entryParentObject)
        entryParentObject = this;
    return impl? impl->openFileEntry(entryPath, openMode, password, entryParentObject) : 0;
}

Archive::~Archive()
{
    if (impl)
        delete impl;
}

} // namespace Q7Zip
