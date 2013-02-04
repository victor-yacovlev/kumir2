
#include "kumiranalizerplugin.h"
#include "analizer.h"
#include "errormessages/errormessages.h"

#include <QtCore>


using namespace KumirAnalizer;


KumirAnalizerPlugin::KumirAnalizerPlugin()
{
    m_analizers = QVector<Analizer*> (128, NULL);
}


KumirAnalizerPlugin::~KumirAnalizerPlugin()
{
}


QString KumirAnalizerPlugin::initialize(const QStringList &arguments)
{
    QLocale::Language language = QLocale::Russian;

    Analizer::setModuleAlwaysAvailable(QString::fromUtf8("Стандартные функции"));

    Q_FOREACH (const QString &arg, arguments) {
        if (arg.startsWith("language=")) {
            const QString lang = arg.mid(9);
            const QLocale loc(lang);
            if (loc.language()!=QLocale::C) {
                language = loc.language();
                break;
            }
        }
        else if (arg.startsWith("preload=")) {
            const QString modname = arg.mid(8);
            Analizer::setModuleAlwaysAvailable(modname);
        }
    }

    Analizer::setSourceLanguage(language);

    Shared::ErrorMessages::loadMessages("KumirAnalizer");

    return "";
}

void KumirAnalizerPlugin::start()
{

}

void KumirAnalizerPlugin::stop()
{

}


int KumirAnalizerPlugin::newDocument()
{
    int id = 0;
    for (int i=0 ; i < m_analizers.size(); i++) {
        if (m_analizers[i]==NULL) {
            id = i;
            break;
        }
    }
    m_analizers[id] = new Analizer(this);
    return id;
}

void KumirAnalizerPlugin::dropDocument(int documentId)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->deleteLater();
    m_analizers[documentId] = NULL;
}

void KumirAnalizerPlugin::setSourceText(int documentId, const QString &text)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    Shared::ChangeTextTransaction change;
    change.newLines = text.split("\n");
    change.removedLineNumbers << 999999; // Flag: remove all
    if (!text.trimmed().isEmpty())
        m_analizers[documentId]->changeSourceText(QList<Shared::ChangeTextTransaction>() << change);
}

QList<Shared::Suggestion> KumirAnalizerPlugin::suggestAutoComplete(int documentId, int lineNo, const QString &before, const QString &after) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->suggestAutoComplete(lineNo, before, after);
}

void KumirAnalizerPlugin::setSourceDirName(int documentId, const QString &dirPath)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->setSourceDirName(dirPath);
}

void KumirAnalizerPlugin::setHiddenText(int documentId, const QString &text, int baseLine)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->setHiddenText(text, baseLine);
}

void KumirAnalizerPlugin::setHiddenTextBaseLine(int documentId, int baseLine)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->setHiddenBaseLine(baseLine);
}

void KumirAnalizerPlugin::changeSourceText(int documentId, const QList<Shared::ChangeTextTransaction> & changes)
{
    Q_CHECK_PTR(m_analizers[documentId]);
    m_analizers[documentId]->changeSourceText(changes);
}

QList<Shared::Error> KumirAnalizerPlugin::errors(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->errors();
}

QList<Shared::LineProp> KumirAnalizerPlugin::lineProperties(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->lineProperties();
}

QList<QPoint> KumirAnalizerPlugin::lineRanks(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->lineRanks();
}

QStringList KumirAnalizerPlugin::imports(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->imports();
}

const AST::Data * KumirAnalizerPlugin::abstractSyntaxTree(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->abstractSyntaxTree();
}


Shared::LineProp KumirAnalizerPlugin::lineProp(int documentId, const QString &text) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->lineProp(text);
}

std::string KumirAnalizerPlugin::rawSourceData(int documentId) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    QString s = m_analizers[documentId]->sourceText();
    QByteArray ba;
    QTextStream ts(&ba);
    ts.setGenerateByteOrderMark(true);
    ts.setCodec("UTF-8");
    ts << s;
    return std::string(ba.constData());
}

QStringList KumirAnalizerPlugin::algorhitmsAvailableFor(int documentId, int lineNo) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->algorhitmsAvailableFor(lineNo);
}

QStringList KumirAnalizerPlugin::globalsAvailableFor(int documentId, int lineNo) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->globalsAvailableFor(lineNo);
}

QStringList KumirAnalizerPlugin::localsAvailableFor(int documentId, int lineNo) const
{
    Q_CHECK_PTR(m_analizers[documentId]);
    return m_analizers[documentId]->localsAvailableFor(lineNo);
}



Q_EXPORT_PLUGIN2(KumirAnalizerPlugin, KumirAnalizer::KumirAnalizerPlugin)
