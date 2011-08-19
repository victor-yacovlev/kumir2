#ifndef KUMFILE_KUMFILE_H
#define KUMFILE_KUMFILE_H

#include <QtCore>

#ifdef DATAFORMATS_LIBRARY
#define KUMFILE_EXPORT Q_DECL_EXPORT
#else
#define KUMFILE_EXPORT Q_DECL_IMPORT
#endif

namespace KumFile {

struct Data
{
    QString visibleText;
    QSet<int> protectedLineNumbers;
    QString hiddenText;
    bool hasHiddenText;

};

KUMFILE_EXPORT QString toString(const Data & data);
KUMFILE_EXPORT Data fromString(const QString &s);

} // namespace KumFile

KUMFILE_EXPORT QTextStream & operator<<(QTextStream & stream, const KumFile::Data & data);
KUMFILE_EXPORT QTextStream & operator>>(QTextStream & stream, KumFile::Data & data);

#endif // KUMFILE_KUMFILE_H
