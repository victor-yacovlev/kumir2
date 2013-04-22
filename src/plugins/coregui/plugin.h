#ifndef COREGUI_PLUGIN_H
#define COREGUI_PLUGIN_H

#include <QtCore>
#include <QtGui>
#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"
#include "interfaces/browserinterface.h"
#include "interfaces/generatorinterface.h"
#include "interfaces/actorinterface.h"
#include "interfaces/runinterface.h"
#include "widgets/secondarywindow.h"
#include "terminal.h"
#include "kumirprogram.h"



namespace CoreGUI {

using namespace Terminal;
using namespace Shared;

class Plugin
        : public ExtensionSystem::KPlugin
{
    friend class MainWindow;
    Q_OBJECT
public:
    explicit Plugin();
    ~Plugin();
    static QString InitialTextKey;
    static QString SessionFilesListKey;
    static QString SessionTabIndexKey;
    static QString RecentFileKey;
    static QString RecentFilesKey;
    static QString MainWindowGeometryKey;
    static QString MainWindowStateKey;
    static QString MainWindowSplitterStateKey;
    static QString DockVisibleKey;
    static QString DockFloatingKey;
    static QString DockGeometryKey;
    static QString DockSideKey;
protected slots:
    void prepareKumirProgramToRun();

protected:
    QString initialize(const QStringList &arguments);
    void saveSession() const;
    void restoreSession();
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void start();
    void stop();
    void updateSettings();

    class MainWindow * mainWindow_;
    class QLabel * m_kumirStateLabel;
    class QLabel * m_genericCounterLabel;
    EditorInterface * plugin_editor;
    GeneratorInterface * plugin_NativeGenerator;
    GeneratorInterface * plugin_BytecodeGenerator;
    BrowserInterface * plugin_browser;
    RunInterface * plugin_kumirCodeRun;
    QList<ActorInterface*> l_plugin_actors;
    BrowserComponent startPage_;
    QList<Widgets::SecondaryWindow*> secondaryWindows_;
    Term * m_terminal;
    QMap<QString,QObject*> m_browserObjects;
    KumirProgram * kumirProgram_;
    class DebuggerWindow * debugger_;
    BrowserComponent helpBrowser_;
    QSplitter * bottomSplitter_;

    bool sessionsDisableFlag_;

};

} // namespace CoreGUI

#endif // COREGUI_PLUGIN_H
