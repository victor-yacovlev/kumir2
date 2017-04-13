#include "docbookview.h"

#include <QtCore>
#include <QtGui>

#include "mainwindow.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationDomain("ru.org.lpm.kumir2");
    app.setOrganizationName("NIISI RAS");
    app.setApplicationName("DocBook Viewer");
    QApplication::setQuitOnLastWindowClosed(true);

    DocBookViewer::MainWindow * mainWindow = new DocBookViewer::MainWindow();
    mainWindow->show();
    return app.exec();
}
