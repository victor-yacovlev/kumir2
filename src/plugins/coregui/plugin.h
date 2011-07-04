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
    static QString MainWindowStateKey;
    static QString DockVisibleKey;
    static QString DockFloatingKey;
    static QString DockGeometryKey;
    static QString DockSideKey;
protected slots:
    void prepareKumirProgramToRun();
protected:
    QString initialize(const QStringList &arguments);
    QByteArray saveSession() const;
    void restoreSession(const QByteArray &);
    void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
    void start();
    void stop();

    class MainWindow * m_mainWindow;
    class QLabel * m_kumirStateLabel;
    class QLabel * m_genericCounterLabel;
    EditorInterface * plugin_editor;
    GeneratorInterface * plugin_NativeGenerator;
    GeneratorInterface * plugin_BytecodeGenerator;
    BrowserInterface * plugin_browser;
    QList<ActorInterface*> l_plugin_actors;
    BrowserComponent m_startPage;
    Terminal * m_terminal;
    QMap<QString,QObject*> m_browserObjects;
    KumirProgram * m_kumirProgram;

};

} // namespace CoreGUI

#endif // COREGUI_PLUGIN_H
