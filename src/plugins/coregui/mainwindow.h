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
    enum DockWindowType { Terminal, StandardActor, WorldActor, Control, Other };
    explicit MainWindow(Plugin * p);

    void addCentralComponent(const QString &title
                             , QWidget *c
                             , const QList<QAction*> & toolbarActions
                             , const QList<QMenu*> & menus
                             , DocumentType type
                             , bool enableToolBar);

    void addSecondaryComponent(const QString & title
                               , QWidget * c
                               , const QList<QAction*> & toolbarActions
                               , const QList<QMenu*> & menus
                               , DockWindowType type);
    ~MainWindow();
public slots:
    QStringList recentFiles(bool fullPaths) const;
    void loadRecentFile(int index);
    inline void loadFromUrl(const QString &s) { loadFromUrl(QUrl(s)); }
    void loadFromUrl(const QUrl & url);
    void restoreSession();
    void newProgram();
    void newText();
    void fileOpen();
    void closeCurrentTab();
    void closeTab(int index);

private slots:
    void setupMenus();
    void setupProgram();
    void addToRecent(const QString &fileName);
    void handleTabTitleChange(const QString & title);


private:
    void showEvent(QShowEvent *);
    void closeEvent(QCloseEvent *);
    QString suggestNewFileName(const QString &suffix) const;
    void createTopLevelMenus(const QList<QMenu*> & c, bool tabDependent);
    bool checkForSaved(VisualComponent * c);
    static QIcon actionIcon(const QString & name);

    QActionGroup * gr_fileActions;
    QActionGroup * gr_otherActions;


    Ui::MainWindow *ui;
    Plugin *m_plugin;
};


} // namespace CoreGUI
#endif // COREGUI_MAINWINDOW_H
