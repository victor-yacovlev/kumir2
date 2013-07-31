#include <QtCore>
#include <QtGui>

#include "editorplugin.h"
#include "editor.h"
#include "settingspage.h"

namespace Editor {

using namespace Shared;


EditorPlugin::EditorPlugin()
{
    editors_ = QVector< Ed > ( 128, Ed(0,0,-1));
    settingsPage_ = 0;
    teacherMode_ = false;
    helpViewer_ = nullptr;
}

EditorPlugin::~EditorPlugin()
{
    foreach (Ed e, editors_) {
        if (e.e)
            delete e.e;
    }
}

void EditorPlugin::appendMarginText(int documentId, int lineNo, const QString & text)
{
    Editor * ed = editors_[documentId].e;
    Q_CHECK_PTR(ed);
    ed->appendMarginText(lineNo, text);
}

void EditorPlugin::setMarginText(int documentId, int lineNo, const QString & text, const QColor & fgColor)
{
    Editor * ed = editors_[documentId].e;
    Q_CHECK_PTR(ed);
    ed->setMarginText(lineNo, text, fgColor);
}

void EditorPlugin::clearMargin(int documentId)
{
    Editor * ed = editors_[documentId].e;
    Q_CHECK_PTR(ed);
    ed->clearMarginText();
}

void EditorPlugin::clearMargin(int documentId, int fromLine, int toLine)
{
    Editor * ed = editors_[documentId].e;
    Q_CHECK_PTR(ed);
    ed->clearMarginText(fromLine, toLine);
}

void EditorPlugin::setDocBookViewer(DocBookViewer::DocBookView *viewer)
{
    helpViewer_ = viewer;
    for (int i=0; i<editors_.size(); i++) {
        if (editors_[i].e)
            editors_[i].e->setHelpViewer(viewer);
    }
}

Shared::EditorComponent EditorPlugin::newDocument(const QString &analizerName, const QString &documentDir, bool initiallyNotSaved)
{
    AnalizerInterface * a = 0;
    int docId = -1;
    if (!analizerName.isEmpty()) {
        QObject * dep = myDependency(analizerName);
        Q_CHECK_PTR(dep);
        a = qobject_cast<AnalizerInterface*>( dep );
        Q_CHECK_PTR(a);
        docId = a->newDocument();
        a->setSourceDirName(docId, documentDir);
    }
    Editor * w = new Editor(this, initiallyNotSaved, mySettings(), a, docId, 0);
    w->setHelpViewer(helpViewer_);
    w->setTeacherMode(teacherMode_);
    if (analizerName.contains("kumir", Qt::CaseInsensitive)) {
         const QString initialTextFileName =
                mySettings()->value(SettingsPage::KeyProgramTemplateFile,
                                    SettingsPage::DefaultProgramTemplateFile)
                .toString();
        QFile f(initialTextFileName);
        if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
            const QByteArray bytes = f.readAll();
            f.close();
            const KumFile::Data data = KumFile::fromString(KumFile::readRawDataAsString(bytes, QString(), a? a->defaultDocumentFileNameSuffix() : "txt"));
            w->setKumFile(data);
        }
    }
    int index = 0;
    for (int i=0; i<editors_.size(); i++) {
        if (editors_[i].e==0 && editors_[i].a==0) {
            index = i;
            break;
        }
    }
    Ed ed(w, a, docId);
    editors_[index] = ed;
    Shared::EditorComponent result;
    result.id = index;
    result.widget = w;
    result.menus = w->menuActions();
    result.toolbarActions = w->toolbarActions();
//    result.statusbarWidgets = w->statusbarWidgets();
    return result;
}

int EditorPlugin::analizerDocumentId(int editorDocumentId) const
{
    Ed ed = editors_[editorDocumentId];
    return ed.id;
}

void EditorPlugin::updateSettings()
{
    for (int i=0; i<editors_.size(); i++) {
        if (editors_[i].e) {
            editors_[i].e->setSettings(mySettings());
        }
        else {
            break;
        }
    }
    if (settingsPage_) {
        settingsPage_->changeSettings(mySettings());
    }
}

quint32 EditorPlugin::errorsLinesCount(int documentId) const
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Ed ed = editors_[documentId];
    if (ed.a) {
        AnalizerInterface * ai = ed.a;
        int id = ed.id;
        QSet<int> lns;
        QList<Error> errors = ai->errors(id);
        foreach (const Error & e, errors) {
            if (e.line>=0)
                lns.insert(e.line);
        }

        return lns.size();

    }
    else {
        return 0;
    }
}

void EditorPlugin::closeDocument(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Q_CHECK_PTR(editors_[documentId].e);
    Ed ed = editors_[documentId];
    ed.e->unsetAnalizer();
    ed.e->deleteLater();
    if (ed.a) {
        AnalizerInterface * a = ed.a;
        a->dropDocument(ed.id);
    }

    ed.e = 0;
    ed.a = 0;
    ed.id = -1;
    editors_[documentId] = ed;
}

QString EditorPlugin::saveDocument(int documentId, const QString &fileName)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Q_CHECK_PTR(editors_[documentId].e);
    Ed ed = editors_[documentId];
    Editor * editor = ed.e;
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QDataStream ts(&f);
        ts << editor->toKumFile();
        f.close();
    }
    else {
        return tr("Can't open file %1 for writing").arg(fileName);
    }
    return "";
}

KumFile::Data EditorPlugin::documentContent(int documentId) const
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Q_CHECK_PTR(editors_[documentId].e);
    Ed ed = editors_[documentId];
    Editor * editor = ed.e;
    return editor->toKumFile();
}

QString EditorPlugin::loadDocument(int documentId, const QString &fileName, bool keepIndents)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Q_CHECK_PTR(editors_[documentId].e);
    Ed ed = editors_[documentId];
    AnalizerInterface * a = editors_[documentId].a;
    Editor * editor = ed.e;
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        const QByteArray bytes = f.readAll();
        editor->setKumFile(
                    KumFile::fromString(
                        KumFile::readRawDataAsString(bytes, QString(), a? a->defaultDocumentFileNameSuffix() : "txt")
                        , keepIndents
                        )
                    );
        f.close();
    }
    else {
        return tr("Can't open file %1 for reading").arg(fileName);
    }
    return "";
}

QString EditorPlugin::loadDocument(int documentId, const KumFile::Data & data)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Q_CHECK_PTR(editors_[documentId].e);
    Ed ed = editors_[documentId];
    Editor * editor = ed.e;
    editor->setKumFile(data);
    return "";
}

QString EditorPlugin::initialize(const QStringList &arguments, const ExtensionSystem::CommandLine &)
{
    if (arguments.contains("teacher"))
        teacherMode_ = true;
    else
        teacherMode_ = false;
    return 0;
}

void EditorPlugin::start()
{

}

QWidget* EditorPlugin::settingsEditorPage()
{
    settingsPage_ = new SettingsPage(mySettings());
    settingsPage_->setWindowTitle(tr("Editor"));
    return settingsPage_;
}

void EditorPlugin::stop()
{

}

bool EditorPlugin::hasUnsavedChanges(int documentId) const
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Q_CHECK_PTR(editors_[documentId].e);
    Editor * e = editors_[documentId].e;
    return e->isModified();
}

void EditorPlugin::setDocumentChangesSaved(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    Q_CHECK_PTR(editors_[documentId].e);
    Editor * e = editors_[documentId].e;
    e->setNotModified();
}

AnalizerInterface * EditorPlugin::analizer(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<editors_.size());
    return editors_[documentId].a;
}

void EditorPlugin::changeGlobalState(ExtensionSystem::GlobalState prev, ExtensionSystem::GlobalState current)
{

    if (current==ExtensionSystem::GS_Unlocked || current==ExtensionSystem::GS_Running) {
        for (int i=0; i<editors_.size(); i++)
            unhighlightLine(i);
    }
    if (prev==ExtensionSystem::GS_Observe && current!=ExtensionSystem::GS_Observe) {
        for (int i=0; i<editors_.size(); i++)
            if (editors_[i].e)
                editors_[i].e->clearMarginText();
            else
                break;
    }
    if (current==ExtensionSystem::GS_Unlocked || current==ExtensionSystem::GS_Observe) {
        for (int i=0; i<editors_.size(); i++) {
            if (editors_[i].e) {
                editors_[i].e->unlock();
            }
            else {
                break;
            }
        }
    }
    else {
        for (int i=0; i<editors_.size(); i++) {
            if (editors_[i].e) {
                editors_[i].e->lock();
            }
            else {
                break;
            }
        }
    }
}

void EditorPlugin::highlightLineGreen(int documentId, int lineNo, quint32 colStart, quint32 colEnd)
{
    if (editors_[documentId].e) {
        const QColor bgColor = editors_[documentId].e->palette().color(QPalette::Base);
        int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
        QColor color;
        if (darkness / 3 <= 127) {
            color = QColor("palegreen");
        }
        else {
            color = QColor(Qt::darkGreen);
        }
        editors_[documentId].e->setLineHighlighted(lineNo, color, colStart, colEnd);
    }
}

void EditorPlugin::highlightLineRed(int documentId, int lineNo, quint32 colStart, quint32 colEnd)
{
    if (editors_[documentId].e) {
        const QColor bgColor = editors_[documentId].e->palette().color(QPalette::Base);
        int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
        QColor color;
        if (darkness / 3 <= 127) {
            color = QColor("lightcoral");
        }
        else {
            color = QColor(Qt::red);
        }
        editors_[documentId].e->setLineHighlighted(lineNo, color, colStart, colEnd);
    }
}

void EditorPlugin::unhighlightLine(int documentId)
{
    if (editors_[documentId].e)
        editors_[documentId].e->setLineHighlighted(-1, QColor::Invalid, 0u, 0u);
}

void EditorPlugin::ensureAnalized(int documentId)
{
    if (editors_[documentId].a && editors_[documentId].e) {
        editors_[documentId].e->ensureAnalized();
    }
}

QByteArray EditorPlugin::saveState(int documentId)
{
    if (editors_[documentId].e)
        return editors_[documentId].e->saveState();
    else
        return "";
}

void EditorPlugin::restoreState(int documentId, const QByteArray &data)
{

    if (editors_[documentId].e)
        editors_[documentId].e->restoreState(data);
}

void EditorPlugin::updateUserMacros(const QString & analizerName, const QList<Macro> &macros, bool rewrite)
{
    if (rewrite) {
        QString fileName = analizerName.length() > 0
                ? QString::fromAscii(".user-macros-%1.xml").arg(analizerName)
                : QString::fromAscii(".user-macros.xml");

        const QString dirName = mySettings()->locationDirectory();
        if (dirName.startsWith(QDir::homePath() + "/."))
            fileName.remove(0, 1);
        QDir dir(dirName);
        if (!dir.exists()) {
            dir.mkpath("");
        }

        const QString filePath = dir.absoluteFilePath(fileName);
        if (macros.size() > 0) {
            saveToFile(filePath, macros);
        }
        else {
            QFile::remove(filePath);
        }
    }

    for (int i=0; i<editors_.size(); i++) {
        if (editors_[i].e) {
            editors_[i].e->updateInsertMenu();
        }
    }
}

}


Q_EXPORT_PLUGIN(Editor::EditorPlugin)
