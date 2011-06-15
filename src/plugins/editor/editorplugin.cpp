#include <QtCore>
#include <QtGui>

#include "editorplugin.h"
#include "editor.h"
#include "editorstandalonewindow.h"
#include "settingspage.h"

namespace Editor {

using namespace Shared;

typedef QPair<AnalizerInterface*, Editor*> Ed;

struct EditorPluginPrivate {
    QVector< Ed > editors;
};

EditorPlugin::EditorPlugin()
{
    d = new EditorPluginPrivate;
    d->editors = QVector< Ed > ( 128, Ed(0,0));
}

EditorPlugin::~EditorPlugin()
{
    foreach (Ed e, d->editors) {
        if (e.first)
            delete e.first;
        if (e.second)
            delete e.second;
    }
    delete d;
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
        if (d->editors[i].first==0 && d->editors[i].second==0) {
            index = i;
            break;
        }
    }
    Ed ed(a, w);
    d->editors[index] = ed;
    Shared::EditorComponent result;
    result.id = index;
    result.widget = w;
    result.menus = w->menuActions();
    result.toolbarActions = w->toolbarActions();
    return result;
}

void EditorPlugin::closeDocument(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].first);
    Q_CHECK_PTR(d->editors[documentId].second);
    Ed ed = d->editors[documentId];
//    delete ed.first;
    ed.second->deleteLater();
    ed.first = 0;
    ed.second = 0;
    d->editors[documentId] = ed;
}

QString EditorPlugin::saveDocument(int documentId, const QString &fileName)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].first);
    Q_CHECK_PTR(d->editors[documentId].second);
    Ed ed = d->editors[documentId];
    Editor * editor = ed.second;
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts;
        ts.setCodec("UTF-8");
        ts.setGenerateByteOrderMark(true);
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
    ExtensionSystem::SettingsEditorPage page;
    page.settingsGroupName = tr("Editor");
    page.settingsPage = new SettingsPage(mySettings());
    return page;
}

void EditorPlugin::stop()
{
    for (int i=0; i<d->editors.size(); i++) {
        if (d->editors[i].first || d->editors[i].second)
            closeDocument(i);
    }
}

bool EditorPlugin::hasUnsavedChanges(int documentId) const
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].second);
    Editor * e = d->editors[documentId].second;
    return e->isModified();
}

void EditorPlugin::setDocumentChangesSaved(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    Q_CHECK_PTR(d->editors[documentId].second);
    Editor * e = d->editors[documentId].second;
    e->setNotModified();
}

AnalizerInterface * EditorPlugin::analizer(int documentId)
{
    Q_ASSERT(documentId>=0);
    Q_ASSERT(documentId<d->editors.size());
    return d->editors[documentId].first;
}

}

Q_EXPORT_PLUGIN(Editor::EditorPlugin);
