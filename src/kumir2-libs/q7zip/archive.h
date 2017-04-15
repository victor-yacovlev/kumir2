#ifndef Q7ZIP_ARCHIVE_H
#define Q7ZIP_ARCHIVE_H

#include <QtGlobal>
#include <QObject>
#include <QIODevice>
#include <QStringList>

#if defined(Q7ZIP_LIBRARY) && !defined(DISABLE_EXPORT_LIBQ7Z)
#   define EXPORT Q_DECL_EXPORT
#else
#   define EXPORT Q_DECL_IMPORT
#endif

namespace Q7Zip {

struct ArchiveImpl;

class EXPORT Archive
        : public QObject
{
    Q_OBJECT
public:   
    bool open(QIODevice::OpenMode openMode);
    void close();
    QStringList allFilesList(const QString & password = QString()) const;
    QIODevice * openFileEntry(const QString & entryPath,
                              QIODevice::OpenMode openMode,
                              const QString & password = QString(),
                              QObject * entryParentObject = 0);
    ~Archive();

protected:
    explicit Archive(QIODevice * ioDevice, QObject *parent = 0);
    ArchiveImpl * impl;
};

} // namespace Q7Zip

#endif // Q7ZIP_ARCHIVE_H
