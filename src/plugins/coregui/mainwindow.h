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
    void loadFromFile(const QString &fileName);
    void restoreSession();
    void newProgram();
    void newText();

private slots:
    void handleMenuAccess();

private:
    QString suggestNewFileName(const QString &suffix) const;
    void createTopLevelMenus(VisualComponent * c, bool tabDependent);

    Ui::MainWindow *ui;
    Plugin *m_plugin;
};


} // namespace CoreGUI
#endif // COREGUI_MAINWINDOW_H
