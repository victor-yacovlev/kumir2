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
    QByteArray hiddenTextSignature;
};

KUMFILE_EXPORT QString toString(const Data & data);
KUMFILE_EXPORT Data fromString(const QString &s, bool keepIndents = false);
KUMFILE_EXPORT KumFile::Data insertTeacherMark(KumFile::Data & data);
KUMFILE_EXPORT QString readRawDataAsString(QByteArray rawData, const QString & sourceEncoding, const QString & fileNameSuffix);

KUMFILE_EXPORT bool hasCryptographicRoutines();
KUMFILE_EXPORT void generateKeyPair(
        const QString & passPhrase,
        QString& privateKey,
        QString& publicKey
        );

KUMFILE_EXPORT void signHiddenText(
        Data& data,
        const QString & privateKey,
        const QString & passPhrase
        );

enum VerifyResult {
    SignatureMatch,
    SignatureMismatch,
    NoSignature,
    CryptographyNotSupported
};

KUMFILE_EXPORT VerifyResult verifyHiddenText(
        const Data& data,
        const QString& publicKey
        );

} // namespace KumFile


KUMFILE_EXPORT QDataStream & operator<<(QDataStream & stream, const KumFile::Data & data);
KUMFILE_EXPORT QDataStream & operator>>(QDataStream & stream, KumFile::Data & data);

#endif // KUMFILE_KUMFILE_H
