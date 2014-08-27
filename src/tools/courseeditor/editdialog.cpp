#include "editdialog.h"
#include "ui_editdialog.h"
#include "ui_editdialog.h"
EditDialog::EditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDialog)
{
    ui->setupUi(this);
    connect(ui->prgButt,SIGNAL(pressed()),this,SLOT(setPrg()));
      connect(ui->remEnvButt,SIGNAL(pressed()),this,SLOT(removeEnv()));
            connect(ui->ButtonAddEnv,SIGNAL(pressed()),this,SLOT(addEnv()));
}

EditDialog::~EditDialog()
{
    delete ui;
}
void EditDialog::setTitle(QString title)
{
    ui->TitleEdit->setText(title);

};

void EditDialog::setDesc(QString desc)
{
    ui->DescEdit->setText(desc);

};
void EditDialog::setProgram(QString prg)
{
    ui->PrgEdit->setText(prg);

};
void EditDialog::setUseIsps(QString isp)
{
 qDebug()<<"set isp"<<isp;
    if(isp=="")
    {
        ui->IspCombo->setCurrentIndex ( 0 );
        return;
    };
    int index = ui->IspCombo->findText(isp);
    ui->IspCombo->setCurrentIndex ( index );

};
void EditDialog::setEnvs(QStringList envs)
{
   ui->EnvList->clear();
   ui->EnvList->addItems(envs);
};

QString EditDialog::getTitle( )
{
    return ui->TitleEdit->text();
};
QString EditDialog::getDesc( )
{
    return ui->DescEdit->toPlainText();
};
QString EditDialog::getProgram( )
{
    return ui->PrgEdit->text();
};
QStringList EditDialog::getUseIsps( )
{
    qDebug()<<"Combo text"<<ui->IspCombo->currentText()<<"Combo index"<<ui->IspCombo->currentIndex();
    if(ui->IspCombo->currentIndex()==0)return QStringList()<< "";
return  QStringList()<<ui->IspCombo->currentText();
   // return items2StringList(ui->IspList->findItems("*",Qt::MatchWildcard));//TODO Check
};
QStringList EditDialog::getEnvs()
{
    return items2StringList(ui->EnvList->findItems("*",Qt::MatchWildcard));//TODO Check
};
void EditDialog::setPrg()
{
    QString dir=curDir;
    QDir chD(curDir);
    if(!chD.exists())dir=QDir::homePath ();
     QFileDialog dialog(this,trUtf8("Выбор файла программы"),dir, "(*.kum )");
     dialog.setAcceptMode(QFileDialog::AcceptOpen);
     if(!dialog.exec())return;
     QFileInfo fi(dialog.selectedFiles().first());
     ui->PrgEdit->setText(chD.relativeFilePath (fi.absoluteFilePath()));
};
void EditDialog::removeEnv()
{
    QList<QListWidgetItem *> toRem=ui->EnvList->selectedItems ();
    for(int i=0;i < toRem.count();i++)
    {
        int row=ui->EnvList->row(toRem.at(i));
      ui->EnvList->takeItem(row);
    }
};
void EditDialog::addEnv()
{
    QString dir=curDir;
    QDir chD(curDir);
    if(!chD.exists())dir=QDir::homePath ();
     QFileDialog dialog(this,trUtf8("Выбор файла обстановки"),dir, "(*.fil)");
     dialog.setFileMode(QFileDialog::ExistingFiles);
     dialog.setAcceptMode(QFileDialog::AcceptOpen);
     if(!dialog.exec())return;
     for(int i=0;i<dialog.selectedFiles().count();i++ )
     {
     QFileInfo fi(dialog.selectedFiles().at(i));

     ui->EnvList->addItem(chD.relativeFilePath (fi.absoluteFilePath()));
     };
}
