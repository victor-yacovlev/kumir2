#ifndef COREGUI_PLUGIN_H
#define COREGUI_PLUGIN_H

#include <QtCore>
#include <QtGui>
#include "extensionsystem/kplugin.h"
#include "interfaces/editorinterface.h"
#include "interfaces/browserinterface.h"
#include "interfaces/generatorinterface.h"
#include "interfaces/actorinterface.h"
#include "terminal.h"
#include "switchworkspacedialog.h"
#include "kumirprogram.h"

using Terminal::Terminal;

namespace CoreGUI {

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
protected:
    QString initialize(const QStringList &arguments);
    void start();
    void stop();

    class MainWindow * m_mainWindow;
    EditorInterface * plugin_editor;
    GeneratorInterface * plugin_CppGenerator;
    GeneratorInterface * plugin_BytecodeGenerator;
    BrowserInterface * plugin_browser;
    QList<ActorInterface*> l_plugin_actors;
    SwitchWorkspaceDialog * m_workspaceDialog;
    BrowserComponent m_startPage;
    Terminal * m_terminal;
    QMap<QString,QObject*> m_browserObjects;
    KumirProgram * m_kumirProgram;

};

} // namespace CoreGUI

#endif // COREGUI_PLUGIN_H
