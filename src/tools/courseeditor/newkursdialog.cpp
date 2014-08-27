#include "newkursdialog.h"
#include "ui_newkursdialog.h"

newKursDialog::newKursDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newKursDialog)
{
    ui->setupUi(this);
    //connect(ui->fileButt,SIGNAL(pressed()),this,SLOT(showFileSelect()));
}

newKursDialog::~newKursDialog()
{
    delete ui;
}

//QString newKursDialog::fileName()
//{
    //return ui->fileEdit->text();
//}

QString newKursDialog::name()
{
 return    ui->nameEdit->text();
}

void newKursDialog::showFileSelect()
{
    QFileDialog dialog(this,trUtf8("Cохранить файл курса"),"", "(*.kurs.xml )");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if(!dialog.exec())return;

    QString fname=dialog.selectedFiles().first();
    if(fname.right(9)!=".kurs.xml")fname=fname+".kurs.xml";
    //ui->fileEdit->setText(fname);
};
