//
// C++ Implementation: new task dialog
//
// Description: 
//
//
// Author: Denis Khachko <mordol@lpm.org.ru>
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dialog.h"




Dialog::Dialog(QWidget* parent, Qt::WindowFlags fl ): QDialog ( parent, fl ), Ui::Dialog()
{
setupUi(this);
connect(BaseA,SIGNAL(valueChanged ( int  )),this,SLOT(setMax()));
connect(BaseB,SIGNAL(valueChanged ( int  )),this,SLOT(setMax()));
connect(pushButton,SIGNAL(clicked(   )),this,SLOT(addFlag()));
connect(pushButton_2,SIGNAL(clicked(   )),this,SLOT(removeFlag()));
connect(listWidget,SIGNAL(itemClicked (QListWidgetItem*)),this,SLOT(enableDelete()));
    translateButtons();
};
void Dialog::setMax()
 {


	int max=BaseA->value();
	if(BaseB->value()>max)max=BaseB->value();


     
 };

void Dialog::addFlag()
  {
    QList<QListWidgetItem *> flags=listWidget->findItems(NeedA->cleanText(),Qt::MatchExactly);
    if(flags.count()>0)return;
    listWidget->addItem(NeedA->cleanText()); 
  };
void Dialog::enableDelete()
 {
	pushButton_2->setEnabled(true);
 };
void Dialog::removeFlag()
 {
 QList<QListWidgetItem *>  selectedItems=listWidget->selectedItems ();
	qDebug()<<"Item Count"<<selectedItems.count();
	for(int i=0;i<selectedItems.count();i++)listWidget->takeItem (listWidget->row(selectedItems[i]));
 if(listWidget->selectedItems ().count()==0)pushButton_2->setEnabled(false);
 };
