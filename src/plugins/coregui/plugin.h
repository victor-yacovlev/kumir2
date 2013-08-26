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
#include "interfaces/guiinterface.h"
#include "interfaces/coursesinterface.h"
#include "widgets/secondarywindow.h"
#include "widgets/dockwindowplace.h"
#include "docbookviewer/docbookview.h"
#include "terminal.h"
#include "kumirprogram.h"



namespace CoreGUI {

using namespace Terminal;
using namespace Shared;

class Plugin
        : public ExtensionSystem::KPlugin
        , public Shared::GuiInterface
{
    friend class MainWindow;
    Q_OBJECT
    Q_INTERFACES(Shared::GuiInterface)
public:
    explicit Plugin();
    ~Plugin();
    static QString InitialTextKey;
    static QString SessionFilesListKey;
    static QString SessionTabIndexKey;
    static QString RecentFileKey;
    static QString RecentFilesKey;
    static QString MainWindowGeometryKey;
    static QString MainWindowShowConsoleKey;
    static QString MainWindowStateKey;
    static QString MainWindowSplitterStateKey;
    static QString DockVisibleKey;
    static QString DockFloatingKey;
    static QString DockGeometryKey;
    static QString DockSideKey;

public slots:
    void setProgramSource(const ProgramSourceText &source);
    ProgramSourceText programSource() const;


    void startTesting();
    bool showWorkspaceChooseDialog();

protected slots:
    void prepareKumirProgramToRun();



protected:
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &runtimeArguments);
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
    Widgets::SecondaryWindow* helpWindow_;
    Term * terminal_;
    QMap<QString,QObject*> m_browserObjects;
    KumirProgram * kumirProgram_;
    class DebuggerView * debugger_;
    DocBookViewer::DocBookView * helpViewer_;  
    Shared::CoursesInterface* courseManager_;
    bool sessionsDisableFlag_;

};

} // namespace CoreGUI

#endif // COREGUI_PLUGIN_H
