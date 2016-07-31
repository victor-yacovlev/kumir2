#ifndef COREGUI_PLUGIN_H
#define COREGUI_PLUGIN_H

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"
#include "interfaces/browserinterface.h"
#include "interfaces/generatorinterface.h"
#include "interfaces/actorinterface.h"
#include "interfaces/runinterface.h"
#include "interfaces/guiinterface.h"
#include "interfaces/coursesinterface.h"
#include "interfaces/startpage_widget_interface.h"
#include "widgets/secondarywindow.h"
#include "widgets/dockwindowplace.h"
#include "docbookviewer/docbookview.h"
#include "terminal.h"
#include "kumirprogram.h"
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    #include <signal.h>
#endif
#if defined(Q_OS_WIN32)
    #include <Windows.h>
#endif



namespace CoreGUI {

using namespace Terminal;
using namespace Shared;
class GUISettingsPage;
class IOSettingsEditorPage;

class Plugin
        : public ExtensionSystem::KPlugin
        , public Shared::GuiInterface
{
    friend class MessageReceiver;
    friend class MainWindow;
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "kumir2.CoreGUI" FILE "")
#endif
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

    QObject * mainWindowObject();
    QObject * pluginObject();

public slots:
    void setProgramSource(const ProgramSourceText &source);
    ProgramSourceText programSource() const;


    void startTesting();
    bool showWorkspaceChooseDialog();

    // for JavaScript

    QString wsName() const;

    QString applicationVersionString() const;

    void showCoursesWindow(const QString & id);
    QStringList coursesList(bool fullPaths) const;

    void showHelpWindow(int index);
    void showHelpWindowFromQuickReference(const int topicType, const QString &name);
    QStringList helpList() const;

protected slots:
    void prepareKumirProgramToRun();
    void handleExternalProcessCommand(const QString & command);
    void updateSettings(const QStringList & keys);
    void showActorWindow(const QByteArray &asciiName);



protected:
    QList<CommandLineParameter> acceptableCommandLineParameters() const;
    QString initialize(const QStringList &configurationArguments,
                       const ExtensionSystem::CommandLine &runtimeArguments);
    void saveSession() const;
    void setStartTabStyle(const QString &tabStyle);
    void createStartPage();
    void createWebKitStartPage();
    void createDefaultStartPage();
    void createSpecializedStartPage(Shared::StartpageWidgetInterface * plugin);
    void restoreSession();
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void start();
    void stop();
    QList<QWidget*> settingsEditorPages() override;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    static void handleSIGUSR1(int, siginfo_t *, void *);
#endif
#if defined(Q_OS_WIN32)
    void timerEvent(QTimerEvent *);
    QSharedMemory * ipcShm_;
#endif

    class MainWindow * mainWindow_;
    class QLabel * m_kumirStateLabel;
    class QLabel * m_genericCounterLabel;
    EditorInterface * plugin_editor;
    GeneratorInterface * plugin_NativeGenerator;
    GeneratorInterface * plugin_BytecodeGenerator;
    BrowserInterface * plugin_browser;
    RunInterface * plugin_kumirCodeRun;
    QList<ActorInterface*> l_plugin_actors;
    QList<Widgets::SecondaryWindow*> secondaryWindows_;
    Widgets::SecondaryWindow* helpWindow_;
    Widgets::SecondaryWindow* coursesWindow_;
    Widgets::SecondaryWindow* quickRefWindow_;
    Term * terminal_;
    QMap<QString,QObject*> m_browserObjects;
    KumirProgram * kumirProgram_;
    class DebuggerView * _debugger;
    DocBookViewer::DocBookView * helpViewer_;  
    Shared::CoursesInterface* courseManager_;
    bool sessionsDisableFlag_;
    static Plugin * instance_;
    QString fileNameToOpenOnReady_;
    QMap<QByteArray,QAction*> showActorActions_;
    GUISettingsPage * guiSettingsPage_;
    IOSettingsEditorPage * ioSettingsPage_;

signals:
    void externalProcessCommandReceived(const QString & command);

};

} // namespace CoreGUI

#endif // COREGUI_PLUGIN_H
