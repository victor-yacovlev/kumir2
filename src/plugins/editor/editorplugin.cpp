#include <QtCore>
#include <QtGui>

#include "editorplugin.h"
#include "editor.h"
#include "editorstandalonewindow.h"
#include "settingspage.h"

namespace Editor {

using namespace Shared;

struct Ed {
    inline Ed() { e = 0; a=0; id =-1; }
    inline Ed(Editor *ee, AnalizerInterface *aa, int i) { a=aa;e=ee;id=i; }
    Editor * e;
    AnalizerInterface * a;
    int id;
};

struct EditorPluginPrivate {
    QVector< Ed > editors;
    SettingsPage * settingsPage;
};

EditorPlugin::EditorPlugin()
{
    d = new EditorPluginPrivate;
    d->editors = QVector< Ed > ( 128, Ed(0,0,-1));
    d->settingsPage = 0;
}

EditorPlugin::~EditorPlugin()
{
    foreach (Ed e, d->editors) {
        if (e.e)
            delete e.e;
    }
    delete d;
}

void EditorPlugin::setMarginText(int documentId, int lineNo, const QString & text)
{
    Editor * ed = d->editors[documentId].e;
    Q_CHECK_PTR(ed);
    ed->setMarginText(lineNo, text);
}

Shared::EditorComponent EditorPlugin::newDocument(const QString &analizerName, const QString &initialText)
{
    AnalizerInterface * a = 0;
    int docId = -1;
    if (!analizerName.isEmpty()) {
        QObject * dep = myDependency(analizerName);
        Q_CHECK_PTR(dep);
        a = dynamic_cast<AnalizerInterface*>( dep );
        Q_CHECK_PTR(a);
        docId = a->newDocument();
    }
    Editor * w = new Editor(mySettings(), a, docId, 0);
    w->setText(initialText);
    int index = 0;
    for (int i=0; i<d->editors.size(); i++) {
        if (d->editors[i].e==0 && d->editors[i].a==0) {
            index = i;
            break;
        }
    }
    Ed ed(w, a, docId);
    d->editors[index] = ed;
    Shared::EditorComponent result;
    result.id = index;
    result.widget = w;
    result.menus = w->menuActions();
    result.toolbarActions = w->toolbarActions();
    result.statusbarWidgets = w->statusbarWidgets();
    return result;
}

void EditorPlugin::updateSettings()
{
    for (int i=0; i<d->editors.size(); i++) {
        if (d->editors[i].e) {
            d->editors[i].e->setSettings(mySettings());
        }
        else {
            break;
        }
    }
    if (d->settingsPage) {
        d->settingsPage->changeSettings(mySettings());
    }
}

quint32 EditorPlugin::errorsCount(int documentId) const
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Ed ed = d->editors[documentId];
    if (ed.a) {
        AnalizerInterface * ai = ed.a;
        int id = ed.id;
        return ai->errors(id).size();

    }
    else {
        return 0;
    }
}

void EditorPlugin::closeDocument(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].e);
    Ed ed = d->editors[documentId];
    ed.e->deleteLater();
    if (ed.a) {
        AnalizerInterface * a = ed.a;
        a->dropDocument(ed.id);
    }

    ed.e = 0;
    ed.a = 0;
    ed.id = -1;
    d->editors[documentId] = ed;
}

QString EditorPlugin::saveDocument(int documentId, const QString &fileName)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].e);
    Ed ed = d->editors[documentId];
    Editor * editor = ed.e;
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QTextStream ts(&f);
        if (analizer(documentId)) {
            ts.setCodec("UTF-8");
            ts.setGenerateByteOrderMark(true);
        }
        ts << editor->text();
        f.close();
    }
    else {
        return tr("Can't open file %1 for writing").arg(fileName);
    }
    return "";
}

QString EditorPlugin::initialize(const QStringList &arguments)
{
    Q_UNUSED(arguments);
    return 0;
}

void EditorPlugin::start()
{
    QString filename;
    for (int i=1; i<qApp->argc(); i++) {
        const QString arg = qApp->arguments()[i];
        if ( !arg.startsWith("-") && !arg.startsWith("[") && arg.endsWith(".kum")) {
            filename = arg;
        }
    }
    EditorStandaloneWindow * w = new EditorStandaloneWindow(this, mySettings(), filename);
    qApp->setQuitOnLastWindowClosed(true);
    w->show();
}

ExtensionSystem::SettingsEditorPage EditorPlugin::settingsEditorPage()
{
    d->settingsPage = new SettingsPage(mySettings());
    ExtensionSystem::SettingsEditorPage page;
    page.settingsGroupName = tr("Editor");
    page.settingsPage = d->settingsPage;
    return page;
}

void EditorPlugin::stop()
{
    for (int i=0; i<d->editors.size(); i++) {
        if (d->editors[i].e || d->editors[i].a)
            closeDocument(i);
    }
}

bool EditorPlugin::hasUnsavedChanges(int documentId) const
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].e);
    Editor * e = d->editors[documentId].e;
    return e->isModified();
}

void EditorPlugin::setDocumentChangesSaved(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].e);
    Editor * e = d->editors[documentId].e;
    e->setNotModified();
}

AnalizerInterface * EditorPlugin::analizer(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    return d->editors[documentId].a;
}

void EditorPlugin::changeGlobalState(ExtensionSystem::GlobalState prev, ExtensionSystem::GlobalState current)
{

    if (current==ExtensionSystem::GS_Unlocked || current==ExtensionSystem::GS_Running) {
        for (int i=0; i<d->editors.size(); i++)
            unhighlightLine(i);
    }
    if (prev==ExtensionSystem::GS_Observe && current!=ExtensionSystem::GS_Observe) {
        for (int i=0; i<d->editors.size(); i++)
            if (d->editors[i].e)
                d->editors[i].e->clearMarginText();
            else
                break;
    }
    if (current==ExtensionSystem::GS_Unlocked || current==ExtensionSystem::GS_Observe) {
        for (int i=0; i<d->editors.size(); i++) {
            if (d->editors[i].e) {
                d->editors[i].e->unlock();
            }
            else {
                break;
            }
        }
    }
    else {
        for (int i=0; i<d->editors.size(); i++) {
            if (d->editors[i].e) {
                d->editors[i].e->lock();
            }
            else {
                break;
            }
        }
    }
}

void EditorPlugin::highlightLineGreen(int documentId, int lineNo)
{
    if (d->editors[documentId].e)
        d->editors[documentId].e->setLineHighlighted(lineNo, QColor(Qt::darkGreen));
}

void EditorPlugin::highlightLineRed(int documentId, int lineNo)
{
    if (d->editors[documentId].e)
        d->editors[documentId].e->setLineHighlighted(lineNo, QColor(Qt::red));
}

void EditorPlugin::unhighlightLine(int documentId)
{
    if (d->editors[documentId].e)
        d->editors[documentId].e->setLineHighlighted(-1, QColor::Invalid);
}

void EditorPlugin::ensureAnalized(int documentId)
{
    if (d->editors[documentId].a && d->editors[documentId].e) {
        d->editors[documentId].e->ensureAnalized();
    }
}

QString EditorPlugin::saveState(int documentId)
{
    if (d->editors[documentId].e)
        return d->editors[documentId].e->saveState();
    else
        return "";
}

void EditorPlugin::restoreState(int documentId, const QString &data)
{
    if (d->editors[documentId].e)
        d->editors[documentId].e->restoreState(data);
}

}


Q_EXPORT_PLUGIN(Editor::EditorPlugin)
