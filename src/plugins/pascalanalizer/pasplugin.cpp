#include "pasplugin.h"

namespace PascalAnalizer {

PasPlugin::PasPlugin() :
    KPlugin()
{
    v_documents.resize(128);
    for (unsigned int i=0; i<v_documents.size(); i++) { v_documents.at(i)=0; }

}

QString PasPlugin::initialize(const QStringList &)
{
    s_fpcUnitsDir = probeFPC();
    return "";
}

int PasPlugin::newDocument()
{
    unsigned int index = 0;
    for ( ; index < v_documents.size(); index++ ) {
        if (!v_documents[index])
            break;
    }
    v_documents[index] = new PascalDocument;
    v_documents[index]->ippc = new IPPC(QStringList()
                                        << getRTLUnitPath()
                                        , this);
    return index;
}

#ifdef Q_OS_WIN32
QString PasPlugin::getRTLUnitPath()
{
    QString result = qApp->applicationDirPath()+"/../fpc-ippc/units/i386-win32/rtl";
    result = QDir::cleanPath(result);
    result = QDir::toNativeSeparators(result);
    result.replace("\\","\\\\");
    return result;
}
#endif

#ifdef Q_OS_UNIX
QString PasPlugin::probeFPC()
{
    QString fpcVersion;
    QString fpcOS;
    QString fpcArch;
    QProcess fpc;
    QString prefix = QString("/usr/")+IDE_LIBRARY_BASENAME+"/fpc/";

    fpc.start("fpc -iV");
    fpc.waitForStarted();
    fpc.waitForReadyRead();
    fpcVersion = QString::fromLocal8Bit(fpc.readAll()).trimmed();
    fpc.waitForFinished();

    fpc.start("fpc -iTO");
    fpc.waitForStarted();
    fpc.waitForReadyRead();
    fpcOS = QString::fromLocal8Bit(fpc.readAll()).trimmed();
    fpc.waitForFinished();

    fpc.start("fpc -iTP");
    fpc.waitForStarted();
    fpc.waitForReadyRead();
    fpcArch = QString::fromLocal8Bit(fpc.readAll()).trimmed();
    fpc.waitForFinished();

    return prefix+fpcVersion+"/units/"+fpcArch+"-"+fpcOS+"/";
}

QString PasPlugin::s_fpcUnitsDir = "";

QString PasPlugin::getRTLUnitPath()
{
    return s_fpcUnitsDir+"rtl";
}

#endif

void PasPlugin::dropDocument(int documentId)
{
    Q_CHECK_PTR(v_documents[documentId]);
    v_documents[documentId]->ippc->deleteLater();
    delete v_documents[documentId];
    v_documents[documentId] = 0;
}

void PasPlugin::setSourceText(int documentId, const QString &text)
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    d->text = text;
    doAnalisys(documentId);
}

void PasPlugin::changeSourceText(int documentId, const QList<Shared::ChangeTextTransaction> &changes)
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    for (int i=0; i<changes.size(); i++) {
        Shared::ChangeTextTransaction tr = changes[i];

    }
    doAnalisys(documentId);
}

void PasPlugin::doAnalisys(int documentId)
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    IPPC::CheckResult r = d->ippc->processAnalisys(d->text, d->names);
    d->lineProps = r.lineProps;
    d->indentRanks = r.ranks;
    d->errors = r.errors;
    d->names = r.names;
}

QList<Shared::Error> PasPlugin::errors(int documentId) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return d->errors;
}

QList<Shared::LineProp> PasPlugin::lineProperties(int documentId) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return d->lineProps;
}

Shared::LineProp PasPlugin::lineProp(int documentId, const QString &text) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return d->ippc->lineSyntax(text, d->names);
}

QList<QPoint> PasPlugin::lineRanks(int documentId) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return d->indentRanks;
}

QStringList PasPlugin::imports(int documentId) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return QStringList();
}

QStringList PasPlugin::globalsAvailableFor(int documentId, int lineNo) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return QStringList();
}

QStringList PasPlugin::localsAvailableFor(int documentId, int lineNo) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return QStringList();
}


QStringList PasPlugin::algorhitmsAvailableFor(int documentId, int lineNo) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    return QStringList();
}

} // namespace PascalAnalizer

Q_EXPORT_PLUGIN2(PascalAnalizer, PascalAnalizer::PasPlugin)
