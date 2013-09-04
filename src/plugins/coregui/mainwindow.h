#ifndef COREGUI_MAINWINDOW_H
#define COREGUI_MAINWINDOW_H

#include "plugin.h"
#include "widgets/dockwindowplace.h"
#include "widgets/multipagedialog.h"

#include <QtCore>
#include <QtGui>

namespace ExtensionSystem {
    class VisualComponent;
}

namespace CoreGUI {


using namespace Shared;
using namespace ExtensionSystem;

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_ENUMS(DocumentType DockWindowType)
    friend class Plugin;

public:
    enum DocumentType { Text, Program, WWW };
    enum DockWindowType { Terminal, Help, StandardActor, WorldActor, Control, SubControl, Other };
    explicit MainWindow(Plugin * p);

    class TabWidgetElement * addCentralComponent(const QString &title
                             , QWidget *c
                             , const QList<QAction*> & toolbarActions
                             , const QList<QMenu*> & menus
                             , DocumentType type);


    ~MainWindow();
    class TabWidgetElement * currentTab();
    void disableTabs();
    QSize minimumSizeHint() const;
public slots:
    void lockActions();
    void unlockActions();
    void ensureBottomVisible();
    void setBottomVisible(bool v);
    QStringList recentFiles(bool fullPaths) const;
    void loadRecentFile(const QString & fullPath);
    inline void loadFromUrl(const QString &s) { loadFromUrl(QUrl(s), true); }
    class TabWidgetElement * loadFromUrl(const QUrl & url, bool addToRecentFiles);
    class TabWidgetElement * loadFromCourseManager(
            const Shared::GuiInterface::ProgramSourceText &data
            );
    Shared::GuiInterface::ProgramSourceText courseManagerProgramSource() const;
    bool saveCurrentFile();
    bool saveCurrentFileAs();
    bool saveCurrentFileTo(const QString & fileName);
    void restoreSession();
    void saveSession() const;
    void loadSettings(const QStringList & keys);
    void saveSettings();
    void newProgram();
    void newText();
    void newText(const QString &fileName, const QString &text);
    void fileOpen();
    void closeCurrentTab();
    bool closeTab(int index);
    void showPreferences();
    void showMessage(const QString & text);
    void clearMessage();
    void setFocusOnCentralWidget();
    void changeFocusOnMenubar();
    void switchWorkspace();
    void showAbout();
    void showUserManual();
    void showHelp();
    void updateSettings(ExtensionSystem::SettingsPtr settings, const QStringList & keys);
    void updateBrowserTitle(const QString & title, const Shared::Browser::InstanceInterface * sender);

private slots:
    void loadRecentFile();
    void prepareRecentFilesMenu();
    void prepareRunMenu();
    void prepareEditMenu();
    void prepareInsertMenu();
    void setupActionsForTab();
    void setupContentForTab();
    void setupStatusbarForTab();
    void setTitleForTab(int index);
    void checkCounterValue();
    void showAlgorithmHelp(const QString & name);

    void addToRecent(const QString &fileName);
    void handleDocumentCleanChanged(bool v);
    void handleSplitterMoved(int, int);


private:
    void createSettingsDialog();        


    void timerEvent(QTimerEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void resizeEvent(QResizeEvent *);

    void closeEvent(QCloseEvent *e);
    int i_timerId;
    bool b_workspaceSwitching;

    QAction * a_notAvailable;

    QList<QWidget*> l_tabDependentStatusbarWidgets;

    QMenu * menu_empty;

    QString suggestNewFileName(const QString &suffix,
                               Shared::Analizer::InstanceInterface * analizer,
                               const QString & dirName = "") const;
    void createTopLevelMenus(const QList<QMenu*> & c, bool tabDependent);
    bool checkForSaved(VisualComponent * c);
    static QIcon actionIcon(const QString & name);

    QActionGroup * gr_fileActions;
    QActionGroup * gr_otherActions;

    bool tabsDisabledFlag_;

//    static QString StatusbarWidgetCSS;

    QLabel * m_message;

    Ui::MainWindow *ui;
    Plugin *m_plugin;
    class StatusBar * statusBar_;
    class Row * bottomRow_;
    class Row * centralRow_;
    class TabWidget * tabWidget_;
    Widgets::DockWindowPlace * helpPlace_;
    Widgets::DockWindowPlace * debuggerPlace_;
    Widgets::DockWindowPlace * actorsPlace_;
    Widgets::DockWindowPlace * consoleAndCourcesPlace_;
    Widgets::MultiPageDialog * settingsDialog_;
    ExtensionSystem::SettingsPtr settings_;
    int prevBottomSize_;
};


} // namespace CoreGUI
#endif // COREGUI_MAINWINDOW_H
