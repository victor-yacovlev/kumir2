#include "kumfile.h"
//#if !defined(HAS_QCA) && defined(Q_OS_LINUX)
//#   define HAS_QCA
//#endif
#ifdef HAS_QCA
#   include <QtCrypto/qca.h>
#endif


Shared::Analizer::SourceFileInterface::Data KumFile::insertTeacherMark(Shared::Analizer::SourceFileInterface::Data & data)//Inserts |#%% if needed.
{
  static const QRegExp teacherMark("^\\|#%%");
    if(data.visibleText.indexOf(teacherMark)>-1)
    {
        //qDebug()<<" TM POS:"<<data.visibleText.indexOf("\n|#%%");
        return data;
    };
    if(data.hiddenText.indexOf(teacherMark)>-1)
    { // qDebug()<<" TM POS:"<<data.visibleText.indexOf("\n|#%%");
        return data;
    };   
   // static const QRegExp speclAlg(QString::fromUtf8("^\\s*алг\\s*(\\S\\S\\S)?\\s+@"));
    static const QRegExp speclAlg(QString::fromUtf8("^|\\s*алг\\s*(\\S\\S\\S)?\\s*@"));

    int pos=data.hiddenText.indexOf(speclAlg);
    qDebug()<<data.hiddenText;
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

void KumFile::signHiddenText(Shared::Analizer::SourceFileInterface::Data &data,
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

KumFile::VerifyResult KumFile::verifyHiddenText(const Shared::Analizer::SourceFileInterface::Data &data,
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
