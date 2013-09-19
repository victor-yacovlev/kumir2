//
// C++ Implementation: printDialog
//
// Description: 
//
//
// Author: Denis Khachko <mordol@mail.ru>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "dialog.h"




Dialog::Dialog(QWidget* parent, Qt::WFlags fl ): QDialog ( parent, fl ), Ui::Dialog()
{
setupUi(this);
connect(BaseA,SIGNAL(valueChanged ( int  )),this,SLOT(setMax()));
connect(BaseB,SIGNAL(valueChanged ( int  )),this,SLOT(setMax()));
connect(BaseC,SIGNAL(valueChanged ( int  )),this,SLOT(setMax()));
setMax();
};
void Dialog::setMax()
 {
	ValueA->setMaximum(BaseA->value());
        ValueB->setMaximum(BaseB->value());
        ValueC->setMaximum(BaseC->value());

	int max=BaseA->value();
	if(BaseB->value()>max)max=BaseB->value();
	if(BaseC->value()>max)max=BaseC->value();
 
	NeedA->setMaximum(max);
        //NeedB->setMaximum(BaseB->value());
        //NeedC->setMaximum(BaseC->value());
 };