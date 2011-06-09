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

public:
    explicit MainWindow(Plugin * p);
    void addCentralComponent(const QString & title, VisualComponent * c);
    ~MainWindow();
public slots:
    void loadFromUrl(const QUrl & url);
    void restoreSession();
    void newProgram();
    void newText();
    void fileOpen();

private slots:
    void handleMenuAccess();
    void updateToolBar();


private:
    QString suggestNewFileName(const QString &suffix) const;
    void createTopLevelMenus(VisualComponent * c, bool tabDependent);
    bool checkForSaved(VisualComponent * c);
    static QIcon actionIcon(const QString & name);

    QActionGroup * gr_fileActions;
    QActionGroup * gr_kumirActions;
    QActionGroup * gr_pascalActions;
    QActionGroup * gr_otherActions;


    Ui::MainWindow *ui;
    Plugin *m_plugin;
};


} // namespace CoreGUI
#endif // COREGUI_MAINWINDOW_H
