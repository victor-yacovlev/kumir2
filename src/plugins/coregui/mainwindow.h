#ifndef COREGUI_MAINWINDOW_H
#define COREGUI_MAINWINDOW_H

#include "plugin.h"
#include "widgets/dockwindowplace.h"
#include "widgets/secondarywindow.h"
#include "widgets/multipagedialog.h"

#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

namespace ExtensionSystem {
    class VisualComponent;
}

namespace CoreGUI {


using namespace Shared;
using namespace ExtensionSystem;

namespace Ui {
    class MainWindow;
}


class MakeNativeExecutableWorker
        : public QThread
{
    Q_OBJECT
public:
    Shared::GeneratorInterface * generator;
    Shared::Analizer::InstanceInterface * analizer;
    QString fileName;
    QString fileSuffix;
    QByteArray buffer;
    QMessageBox * progressDialog;
    bool canceled;
    mutable QMutex canceledMutex;
    bool isCanceled() const;
public slots:
    void cancel();
private:
    void run();

};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_ENUMS(DocumentType DockWindowType)
    friend class Plugin;

public:
    enum DocumentType { Text, Program, StartPage };
    enum DockWindowType { Terminal, Help, StandardActor, WorldActor, Control, SubControl, Other };
    explicit MainWindow(Plugin * p);
    bool isColumnFirstLayout() const;

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

    void switchToRowFirstLayout();
    void switchToColumnFirstLayout();

    void lockActions();
    void unlockActions();
    void ensureSeconrarySideVisible();
    void setConsoleVisible(bool v);
    QStringList recentFiles(bool fullPaths) const;
    void loadRecentFile(const QString & fullPath);
    inline void loadFromUrl(const QString &s) { loadFromUrl(QUrl::fromLocalFile(s), true); }
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
    void updateStartPageTitle(const QString & title, const Shared::Browser::InstanceInterface * sender);
    void makeNativeExecutable();
    void saveNativeExecutable();

private slots:
    void setFirstTimeWindowLayout();
    void setFirstTimeWindowLayout_stage2();
    void setFirstTimeWindowLayout_stage3();
    void loadRecentFile();
    void prepareRecentFilesMenu();
    void prepareRunMenu();
    void prepareEditMenu();
    void prepareInsertMenu();
    void setupActionsForTab();
    void setupContentForTab();
    void setupStatusbarForTab();
    void setTitleForTab(int index);
    void handleTabTitleChanged();
    void checkCounterValue();
    void showAlgorithmHelp(const QString & package, const QString & function);

    void addToRecent(const QString &fileName);
    void handleDocumentCleanChanged(bool v);
    void checkForConsoleHiddenBySplitter(int, int);
    void setupMenuBarContextMenu();
    void addMenuBeforeHelp(QMenu * menu);

    void updateVisibleMenus();
    void updateVisibleMenus(int index);
    void updateVisibleMenus(class TabWidgetElement*);

    void handleBreakpointCnagedOrInserted(bool enabled, quint32 lineNo, quint32 ignoreCount, const QString & condition);
    void handleBreakpointRemoved(quint32 lineNo);


private:

    void createSettingsDialog();

    void prepareLayoutChange();
    QMap<QWidget*,QSize> saveSizes() const;
    void restoreSizes(const QMap<QWidget*,QSize> &sizes, const Qt::Orientation o);


    void timerEvent(QTimerEvent *e);
    bool eventFilter(QObject *o, QEvent *e);
    void resizeEvent(QResizeEvent *);

    void closeEvent(QCloseEvent *e);
    int i_timerId;
    bool b_workspaceSwitching;

    QAction * a_notAvailable;
    QAction * a_notAvailable2;
    QAction * a_notAvailable3;

    QMenu * menuNA1_;
    QMenu * menuNA2_;
    QMenu * menuNA3_;

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
    class Side * secondarySide_;
    class Side * centralSide_;
    class TabWidget * tabWidget_;
    Widgets::DockWindowPlace * helpAndCoursesPlace_;
    Widgets::DockWindowPlace * debuggerPlace_;
    Widgets::DockWindowPlace * actorsPlace_;
    Widgets::DockWindowPlace * consolePlace_;
    Widgets::MultiPageDialog * settingsDialog_;
    ExtensionSystem::SettingsPtr settings_;
    int prevBottomSize_;
    Widgets::SecondaryWindow * debuggerWindow_; // changes place on layout change
    class ToolbarContextMenu * menubarContextMenu_;
    QList<QMenu*> topLevelMenus_;

    int afterShowTimerId2_;
    int afterShowTimerId3_;
};


} // namespace CoreGUI
#endif // COREGUI_MAINWINDOW_H

