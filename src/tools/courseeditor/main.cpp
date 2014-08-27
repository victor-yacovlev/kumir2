#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindowTask w;
    a.setWindowIcon(QIcon(":/taskEdit.ico"));
    w.setWindowIcon(QIcon(":/taskEdit.ico"));
    w.setup();
    w.show();
    return a.exec();
}
