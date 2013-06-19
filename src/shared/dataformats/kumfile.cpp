#include "kumfile.h"
//#if !defined(HAS_QCA) && defined(Q_OS_LINUX)
//#   define HAS_QCA
//#endif
#ifdef HAS_QCA
#   include <QtCrypto/qca.h>
#endif


QString KumFile::toString(const Data &data)
{
    QString result;
    const QStringList lines = data.visibleText.split("\n", QString::KeepEmptyParts);
    for (int i=0; i<lines.count(); i++) {
        result += lines[i];
        if (data.protectedLineNumbers.contains(i)) {
            result += "|@protected";
        }
        if (i<lines.count()-1)
            result += "\n";
    }
    const QStringList hiddenLines = !data.hiddenText.isEmpty()? data.hiddenText.split("\n", QString::KeepEmptyParts) : QStringList();
    if (!result.isEmpty())
        result += "\n";
    for (int i=0; i<hiddenLines.count(); i++) {
        result += hiddenLines[i];
        result += "|@hidden";
        if (i<hiddenLines.count()-1)
            result += "\n";
    }
    if (!data.hiddenTextSignature.isEmpty()) {
        result += QString::fromAscii("|@signature %1|@hidden\n")
                .arg(QString::fromAscii(data.hiddenTextSignature.toBase64()));
    }
    return result;
}


QDataStream & operator <<(QDataStream & ds, const KumFile::Data & data)
{
    QByteArray buffer;
    QTextStream ts(&buffer);
    ts.setCodec("UTF-8");
    ts.setGenerateByteOrderMark(true);
    ts << KumFile::toString(data);
    ts.flush();
    ds.writeRawData(buffer.constData(), buffer.size());
    return ds;
}

KumFile::Data KumFile::fromString(const QString &s, bool keepIndents)
{
    const QStringList lines = s.split("\n", QString::KeepEmptyParts);
    KumFile::Data data;
    data.hasHiddenText = false;
    int lineNo = -1;
    for (int i=0; i<lines.count(); i++) {
        QString line = lines[i];
        if (!keepIndents) {
            while (line.startsWith(" "))
                line.remove(0, 1);
        }
        if (line.startsWith("|@signature ") && line.endsWith("|@hidden"))
        {
            const QString b64 = line.mid(12, line.length()-20);
            data.hiddenTextSignature = QByteArray::fromBase64(b64.toAscii());
        }
        else if (line.endsWith("|@hidden")) {
            data.hasHiddenText = true;
            if (!data.hiddenText.isEmpty() && data.visibleText.isEmpty())
                data.hiddenText += "\n";
            data.hiddenText += line.left(line.length()-8);
            if (i<lines.count()-1 && lines[i+1].endsWith("|@hidden"))
                data.hiddenText += "\n";
        }
        else {
            lineNo ++;
            if (line.endsWith("|@protected")) {
                data.protectedLineNumbers.insert(lineNo);
                data.visibleText += line.left(line.length()-11);
            }
            else {
                data.visibleText += line;
            }
            if (i<lines.count()-1 && !lines[i+1].endsWith("|@hidden"))
                data.visibleText += "\n";
        }
    }
    return data;
}

bool hasWrongSymbols(const QString & s) {
    static const QString CyrillicAlphabet =
            QString::fromUtf8("абвгдеёжзийклмнопрстуфхцчшщъыьэюя");
    unsigned short code = 0;
    foreach (const QChar & ch, s) {
        code = ch.unicode();
        if (code==0) {
            return true;
        }
        if (code>127) {
            if (!CyrillicAlphabet.contains(ch, Qt::CaseInsensitive))
                return true;
        }
    }

    return false;
}
KumFile::Data KumFile::insertTeacherMark(KumFile::Data & data)//Inserts |#%% if needed.
{
  static const QRegExp teacherMark("^\\|#%%");
    if(data.visibleText.indexOf(teacherMark)>-1)
    {
        //qDebug()<<" TM POS:"<<data.visibleText.indexOf("\n|#%%");
        return data;
    };
    if(data.visibleText.indexOf(teacherMark)>-1)
    { // qDebug()<<" TM POS:"<<data.visibleText.indexOf("\n|#%%");
        return data;
    };   
    static const QRegExp speclAlg(QString::fromUtf8("^\\s*алг\\s*(\\S\\S\\S)?\\s*@"));
    int pos=data.hiddenText.indexOf(speclAlg);
    if(pos>-1)
    {
        
        data.hiddenText.insert(pos, "\n|#%%\n");
        return data;
    };
    pos=data.visibleText.indexOf(speclAlg);
    if(pos>1)
    {
     data.visibleText.insert(pos, "\n|#%%\n");
    QStringList strings=data.visibleText.split("\n", QString::KeepEmptyParts);
        for(int i=0;i<strings.count();i++)
        {
         if(strings.at(i).startsWith("|#%%"))
            {
                data.protectedLineNumbers.insert(i);
                break;
            }
        }
     return data;
    };  
    return data;
}
QString KumFile::readRawDataAsString(QByteArray rawData, const QString &sourceEncoding, const QString &fileNameSuffix)
{    
    QTextStream ts(rawData, QIODevice::ReadOnly);
    if (sourceEncoding.isEmpty()) {
        if (fileNameSuffix == "kum"||fileNameSuffix == ".kum") {
            ts.setCodec("UTF-16");
            ts.setAutoDetectUnicode(true);
        }
        else if (fileNameSuffix == "txt") {
#ifdef Q_OS_WIN32
            const char * DefaultCodec = "CP1251";
#else
            const char * DefaultCodec = "UTF-8";
#endif
            ts.setCodec(DefaultCodec);
            ts.setAutoDetectUnicode(true);
        }
        else if (fileNameSuffix == "py") {
            QString codec = "UTF-8";
            QStringList lines = QString::fromAscii(rawData).split("\n");
            // PEP-0263 three variants to declare encoding
            // # coding=<encoding name>
            // # -*- coding: <encoding name -*-
            // # vim: set fileencoding=<encoding name> :
            static const QRegExp rx1("^coding\\s*=\\s*(\\S+)$");
            static const QRegExp rx2("^-\\*-\\s*coding:\\s*(\\S+)\\s*-\\*-$");
            static const QRegExp rx3("^vim:\\s*set\\s+fileencoding\\s*=\\s*(\\S+)\\s*:$");
            foreach (QString line, lines) {
                line = line.trimmed();
                if (line.startsWith("#")) {
                    line.remove(0, 1);
                    line = line.trimmed();
                    if (rx1.exactMatch(line)) {
                        codec = rx1.cap(1);
                        break;
                    }
                    if (rx2.exactMatch(line)) {
                        codec = rx2.cap(1);
                        break;
                    }
                    if (rx3.exactMatch(line)) {
                        codec = rx3.cap(1);
                        break;
                    }
                }
            }
            ts.setCodec(codec.toAscii().data());
            ts.setAutoDetectUnicode(true);
        }
    }
    else {
        ts.setCodec(QTextCodec::codecForName(sourceEncoding.toAscii().constData()));
    }
    QString s = ts.readAll();
    s = s.replace(QChar(13),"");
    if (fileNameSuffix != "txt") {
        s = s.replace(QChar(9), "    ");
    }
    return s;
//    static const QStringList CodecsToTry = QStringList()
//         << "UTF-16" << "UTF-8" << "CP1251" << "KOI8-R" << "IBM866";
//    QTextCodec * currentCodec = 0;
//    QTextDecoder * decoder = 0;
//    QString currentCodecName;
//    QString s;
//    for (int i=0; i<CodecsToTry.size(); i++) {
//        currentCodecName = CodecsToTry[i];
//        currentCodec = QTextCodec::codecForName(currentCodecName.toAscii().constData());
//        decoder = currentCodec->makeDecoder(QTextCodec::ConvertInvalidToNull);
//        s = decoder->toUnicode(rawData);
//        if (!hasWrongSymbols(s)) {
//            return s;
//        }
//    }
//    return "| Incorrect character encoding. I can't load this file\n";
}

QDataStream & operator >>(QDataStream & ds, KumFile::Data & data)
{
    QByteArray buffer;
    char * bb = (char*)calloc(65536, sizeof(char));

    int cnt = 0;
    while (!ds.atEnd()) {
        cnt = ds.readRawData(bb, 65536);
        if (cnt>0) {
            buffer.append(bb, cnt);
        }
    }
    const QString s = KumFile::readRawDataAsString(buffer, data.sourceEncoding, "kum");
    data = KumFile::fromString(s);
    return ds;
}

bool KumFile::hasCryptographicRoutines()
{
#ifdef HAS_QCA
    return true;
#else
    return false;
#endif
}

void KumFile::generateKeyPair(const QString &passPhrase,
                              QString &privateKey,
                              QString &publicKey)
{
    privateKey.clear();
    publicKey.clear();
#ifdef HAS_QCA
    QCA::PrivateKey seckey = QCA::KeyGenerator().createRSA(1024);
    QCA::SecureArray phrase(passPhrase.toUtf8());
    privateKey = seckey.toPEM(phrase);
    QCA::PublicKey pubkey = seckey.toPublicKey();
    publicKey = pubkey.toPEM();
#endif
}

void KumFile::signHiddenText(Data &data,
                             const QString &privateKey,
                             const QString &passPhrase)
{
#ifdef HAS_QCA
    QCA::SecureArray phrase(passPhrase.toUtf8());
    QCA::PrivateKey seckey = QCA::PrivateKey::fromPEM(privateKey, phrase);
    seckey.startSign(QCA::EMSA3_MD5);
    seckey.update(data.hiddenText.trimmed().toUtf8());
    data.hiddenTextSignature = seckey.signature();
#else
    data.hiddenTextSignature.clear();
#endif
}

KumFile::VerifyResult KumFile::verifyHiddenText(const Data &data,
                                                const QString &publicKey)
{
#ifdef HAS_QCA
    if (data.hiddenTextSignature.isEmpty())
        return NoSignature;
    QCA::PublicKey pubkey = QCA::PublicKey::fromPEM(publicKey);
    pubkey.startVerify(QCA::EMSA3_MD5);
    pubkey.update(data.hiddenText.trimmed().toUtf8());
    return pubkey.validSignature(data.hiddenTextSignature)
            ? SignatureMatch : SignatureMismatch;
#else
    return CryptographyNotSupported;
#endif
}
