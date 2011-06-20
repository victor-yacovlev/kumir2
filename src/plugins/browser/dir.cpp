#include "dir.h"

namespace Browser {

Dir::Dir(QObject *parent) :
    QObject(parent)
{
    m_dir = QDir::current();
}

QStringList Dir::files(const QString &filter)
{
    QStringList result = m_dir.entryList(filter.split(";"), QDir::Files);
    return result;
}

QStringList Dir::dirs()
{
    QStringList result = m_dir.entryList(QStringList(), QDir::Dirs);
    result.removeAll(".");
    if (m_dir==QDir::current())
        result.removeAll("..");
    return result;
}

QString Dir::resolveUrl(const QString &entry)
{
    const QString absolutePath = m_dir.absoluteFilePath(entry);
    return QUrl::fromLocalFile(absolutePath).toString();
}

void Dir::open(const QString &path)
{
    m_dir.cd(path);
}



} // namespace Browser
