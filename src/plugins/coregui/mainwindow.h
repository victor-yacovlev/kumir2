#ifndef COREGUI_MAINWINDOW_H
#define COREGUI_MAINWINDOW_H

#include <QtCore>
#include <QtGui>
#include "plugin.h"

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

public:
    enum DocumentType { Text, Kumir, Pascal, WWW };
    enum DockWindowType { Terminal, StandardActor, WorldActor, Control, SubControl, Other };
    explicit MainWindow(Plugin * p);

    class TabWidgetElement * addCentralComponent(const QString &title
                             , QWidget *c
                             , const QList<QAction*> & toolbarActions
                             , const QList<QMenu*> & menus
                             , const QList<QWidget*> & statusbarWidgets
                             , DocumentType type
                             , bool enableToolBar);

    QDockWidget * addSecondaryComponent(const QString & title
                               , QWidget * c
                               , const QList<QAction*> & toolbarActions
                               , const QList<QAction*> & menuActions
                               , DockWindowType type);
    ~MainWindow();
    void disableTabs();
public slots:
    QStringList recentFiles(bool fullPaths) const;
    void loadRecentFile(int index);
    inline void loadFromUrl(const QString &s) { loadFromUrl(QUrl(s), true); }
    class TabWidgetElement * loadFromUrl(const QUrl & url, bool addToRecentFiles);
    bool saveCurrentFile();
    bool saveCurrentFileAs();
    bool saveCurrentFileTo(const QString & fileName);
    void restoreSession();
    void saveSession() const;
    void loadSettings();
    void saveSettings();
    void newProgram();
    void newText();
    void newText(const QString &fileName, const QString &text);
    void fileOpen();
    void closeCurrentTab();
    void closeTab(int index);
    void showPreferences();
    void showMessage(const QString & text);
    void clearMessage();
    void setFocusOnCentralWidget();
    void changeFocusOnMenubar();
    void switchWorkspace();
    void showAbout();
    void showUserManual();

private slots:
    void prepareRunMenu();
    void prepareEditMenu();
    void prepareInsertMenu();
    void setupActionsForTab();
    void setupContentForTab();
    void setupStatusbarForTab();
    void checkCounterValue();
    void addToRecent(const QString &fileName);
    void handleDocumentCleanChanged(bool v);
    void handleTabTitleChange(const QString & title);


private:
    void timerEvent(QTimerEvent *e);
    bool eventFilter(QObject *o, QEvent *e);

    void closeEvent(QCloseEvent *e);
    int i_timerId;
    bool b_workspaceSwitching;

    QAction * a_notAvailable;
    QList<class QDockWidget*> l_dockWindows;

    QList<QWidget*> l_tabDependentStatusbarWidgets;
    QMenu * menu_empty;

    QString suggestNewFileName(const QString &suffix) const;
    void createTopLevelMenus(const QList<QMenu*> & c, bool tabDependent);
    bool checkForSaved(VisualComponent * c);
    static QIcon actionIcon(const QString & name);

    QActionGroup * gr_fileActions;
    QActionGroup * gr_otherActions;



    static QString StatusbarWidgetCSS;

    QLabel * m_message;

    Ui::MainWindow *ui;
    Plugin *m_plugin;
};


} // namespace CoreGUI
#endif // COREGUI_MAINWINDOW_H
