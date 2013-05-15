#ifndef DOCBOOKVIEWER_MAINWINDOW_H
#define DOCBOOKVIEWER_MAINWINDOW_H

#include <QMainWindow>

namespace DocBookViewer {

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void fileOpen();
    
private:
    Ui::MainWindow *ui;
};


} // namespace DocBookViewer
#endif // DOCBOOKVIEWER_MAINWINDOW_H
