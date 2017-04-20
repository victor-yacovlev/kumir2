#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore>
#include <QtGui>

namespace DocBookViewer {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(fileOpen()));
    connect(ui->actionPrint, SIGNAL(triggered()),
            ui->docbookview->viewerAction(DocBookView::ShowPrintDialog),
            SLOT(trigger()));
}


void MainWindow::fileOpen()
{
    QSettings settings;
    const QString prevFileName = settings.value(
                "MainWindow/RecentFileName",
                QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
                ).toString();
    const QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open DocBook document"),
                prevFileName,
                tr("DocBook XML (*.xml);;All files (*)")
                );
    if (filename.length() > 0) {
        settings.setValue("MainWindow/RecentFileName", filename);
        QString error;
        ui->docbookview->addDocument(QUrl::fromLocalFile(filename), &error);
        if (error.length() > 0) {
            QMessageBox::critical(
                        this,
                        tr("Can't open file"),
                        error
                        );
        }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

} // namespace DocBookViewer
