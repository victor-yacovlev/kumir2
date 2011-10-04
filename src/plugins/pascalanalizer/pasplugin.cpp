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
#ifdef Q_OS_UNIX
    s_fpcUnitsDir = probeFPC();
#endif
#ifdef Q_OS_WIN32
    s_fpcUnitsDir = qApp->applicationDirPath()+"/../fpc/units/i386-win32/";
    s_fpcUnitsDir = QDir::cleanPath(s_fpcUnitsDir);
    s_fpcUnitsDir = QDir::toNativeSeparators(s_fpcUnitsDir);
    s_fpcUnitsDir.replace("\\","\\\\");
#endif
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
    QString result = qApp->applicationDirPath()+"/../fpc/units/i386-win32/rtl";
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


QString PasPlugin::getRTLUnitPath()
{
    return s_fpcUnitsDir+"rtl";
}

#endif

QString PasPlugin::s_fpcUnitsDir = "";

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
    d->text = text.split("\n", QString::KeepEmptyParts);
    d->fetchTables = true;
    doAnalisys(documentId);
}

void PasPlugin::updateSourceText(int documentId, const Shared::ChangeTextTransaction &changes)
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    QList<int> removedLineNumbers = changes.removedLineNumbers.toList();
    QStringList newLines = changes.newLines;
    int lineStart = 0;
    int lineEnd = 99999999;
    if (!removedLineNumbers.isEmpty()) {
        if (removedLineNumbers.size()>1 || removedLineNumbers[0]!=999999) {
            // We assume this set is sorted in ascending order
            lineStart = removedLineNumbers.first();
            lineEnd = removedLineNumbers.last();
        }
        else {
            // There was a flag: remove all text
        }
    }
    // Check for changed
    QStringList auchtung;
    auchtung << "uses";
    auchtung << d->localnames.units;
    for (int i=0; i<removedLineNumbers.size(); i++) {
        if (d->fetchTables)
            break;
        foreach (const QString & a, auchtung)
            if (d->text[removedLineNumbers[i]].toLower().contains(a)) {
                d->fetchTables = true;
                break;
            }
    }
    for (int i=0; i<newLines.size(); i++) {
        if (d->fetchTables)
            break;
        foreach (const QString & a, auchtung)
            if (newLines[i].toLower().contains(a)) {
                d->fetchTables = true;
                break;
            }
    }
    QStringList newSourceText;
    if (!removedLineNumbers.isEmpty()) {
        newSourceText = d->text.mid(0, lineStart) + newLines;
        if (lineEnd+1<d->text.size()) {
            newSourceText += d->text.mid(lineEnd+1);
        }
    }
    else {
        newSourceText = d->text + newLines;
    }
    d->text = newSourceText;
}

void PasPlugin::changeSourceText(int documentId, const QList<Shared::ChangeTextTransaction> &changes)
{
    for (int i=0; i<changes.size(); i++) {
        updateSourceText(documentId, changes.at(i));
    }
    doAnalisys(documentId);
}

void PasPlugin::doAnalisys(int documentId)
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    IPPC::CheckResult r = d->ippc->processAnalisys(d->text.join("\n"), d->localnames, d->usednames, d->fetchTables);
    d->lineProps = r.lineProps;
    d->indentRanks = r.ranks;
    d->errors = r.errors;
    d->localnames = r.localnames;
    if (d->fetchTables)
        d->usednames = r.usednames;
    d->fetchTables = false;
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
    return d->ippc->lineSyntax(text, d->localnames, d->usednames);
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
    return d->localnames.units + d->usednames.units;
}

QStringList PasPlugin::globalsAvailableFor(int documentId, int lineNo) const
{
//    Q_CHECK_PTR(v_documents[documentId]);
//    PascalDocument * d = v_documents[documentId];
    Q_UNUSED(lineNo);
    Q_UNUSED(documentId);
    return QStringList();
}

QStringList PasPlugin::localsAvailableFor(int documentId, int lineNo) const
{
    Q_UNUSED(lineNo);
    Q_UNUSED(documentId);
//    Q_CHECK_PTR(v_documents[documentId]);
//    PascalDocument * d = v_documents[documentId];
    return QStringList();
}


QStringList PasPlugin::algorhitmsAvailableFor(int documentId, int lineNo) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    Q_UNUSED(lineNo);
    QSet<QString> locals = QSet<QString>::fromList(d->localnames.procedures);
    QSet<QString> uses = QSet<QString>::fromList(d->usednames.procedures);
    QSet<QString> u = locals.unite(uses);
    return u.toList();
}

std::string PasPlugin::rawSourceData(int documentId) const
{
    Q_CHECK_PTR(v_documents[documentId]);
    PascalDocument * d = v_documents[documentId];
    QTextCodec * c = QTextCodec::codecForName("IBM866");
    return std::string(c->fromUnicode(d->text.join("\n")+"\n").data());
}

} // namespace PascalAnalizer

Q_EXPORT_PLUGIN2(PascalAnalizer, PascalAnalizer::PasPlugin)
