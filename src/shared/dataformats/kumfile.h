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
    QString sourceEncoding;

};

KUMFILE_EXPORT QString toString(const Data & data);
KUMFILE_EXPORT Data fromString(const QString &s);
KUMFILE_EXPORT QString readRawDataAsString(QByteArray rawData, const QString & sourceEncoding);

} // namespace KumFile


KUMFILE_EXPORT QDataStream & operator<<(QDataStream & stream, const KumFile::Data & data);
KUMFILE_EXPORT QDataStream & operator>>(QDataStream & stream, KumFile::Data & data);

#endif // KUMFILE_KUMFILE_H
