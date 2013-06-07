
#include "kumiranalizerplugin.h"
#include "analizer.h"
#include "errormessages/errormessages.h"
#include "lexer.h"

#include <QtCore>


using namespace KumirAnalizer;


KumirAnalizerPlugin::KumirAnalizerPlugin()
    : ExtensionSystem::KPlugin()
    , teacherMode_(false)
{
    analizers_ = QVector<Analizer*> (128, NULL);
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
    teacherMode_ = arguments.contains("teacher", Qt::CaseInsensitive);
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
    for (int i=0 ; i < analizers_.size(); i++) {
        if (analizers_[i]==NULL) {
            id = i;
            break;
        }
    }
    analizers_[id] = new Analizer(this, teacherMode_);
    return id;
}

void KumirAnalizerPlugin::dropDocument(int documentId)
{
    Q_CHECK_PTR(analizers_[documentId]);
    analizers_[documentId]->deleteLater();
    analizers_[documentId] = NULL;
}

void KumirAnalizerPlugin::setSourceText(int documentId, const QString &text)
{
    Q_CHECK_PTR(analizers_[documentId]);
    analizers_[documentId]->setSourceText(text);
}

QList<Shared::Suggestion> KumirAnalizerPlugin::suggestAutoComplete(int documentId, int lineNo, const QString &before, const QString &after) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->suggestAutoComplete(lineNo, before, after);
}

Shared::TextAppend KumirAnalizerPlugin::closingBracketSuggestion(int documentId, int lineNo) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->closingBracketSuggestion(lineNo);
}

QStringList KumirAnalizerPlugin::importModuleSuggestion(int documentId, int lineNo) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->importModuleSuggestion(lineNo);
}

void KumirAnalizerPlugin::setSourceDirName(int documentId, const QString &dirPath)
{
    Q_CHECK_PTR(analizers_[documentId]);
    analizers_[documentId]->setSourceDirName(dirPath);
}


void KumirAnalizerPlugin::setHiddenTextBaseLine(int documentId, int baseLine)
{
    Q_CHECK_PTR(analizers_[documentId]);
    analizers_[documentId]->setHiddenBaseLine(baseLine);
}

QList<Shared::Error> KumirAnalizerPlugin::errors(int documentId) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->errors();
}

QList<Shared::LineProp> KumirAnalizerPlugin::lineProperties(int documentId) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->lineProperties();
}

QList<QPoint> KumirAnalizerPlugin::lineRanks(int documentId) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->lineRanks();
}

QStringList KumirAnalizerPlugin::imports(int documentId) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->imports();
}

const AST::DataPtr KumirAnalizerPlugin::abstractSyntaxTree(int documentId) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->abstractSyntaxTree();
}


Shared::LineProp KumirAnalizerPlugin::lineProp(int documentId, int line_no, const QString &text) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    return analizers_[documentId]->lineProp(line_no, text);
}

std::string KumirAnalizerPlugin::rawSourceData(int documentId) const
{
    Q_CHECK_PTR(analizers_[documentId]);
    QString s = analizers_[documentId]->sourceText();
    QByteArray ba;
    QTextStream ts(&ba);
    ts.setGenerateByteOrderMark(true);
    ts.setCodec("UTF-8");
    ts << s;
    return std::string(ba.constData());
}

QString KumirAnalizerPlugin::createImportStatementLine(
        const QString &importName
        ) const
{
    return Lexer::importKeyword()+" "+importName;
}



Q_EXPORT_PLUGIN2(KumirAnalizerPlugin, KumirAnalizer::KumirAnalizerPlugin)
