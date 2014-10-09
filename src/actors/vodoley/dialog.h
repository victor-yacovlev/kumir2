//****************************************************************************
//**
//** Copyright (C) 2004-2008 IMPB RAS. All rights reserved.
//**
//** This file is part of the Vodoley.
//**
//** This file may be used under the terms of the GNU General Public
//** License version 2.0 as published by the Free Software Foundation
//** and appearing in the file LICENSE.GPL included in the packaging of
//** this file.
//**
//** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
//** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//**
//****************************************************************************/


#include "ui_dialog.h"
#include <QtCore>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif


class Dialog : public QDialog, Ui::Dialog
{
	Q_OBJECT

	public:
        Dialog ( QWidget* parent = 0, Qt::WindowFlags fl = 0 );
		~Dialog(){};
	int ASize(){return BaseA->value();};
	int BSize(){return BaseB->value();};
	int CSize(){return BaseC->value();};

	int AFill(){return ValueA->value();};
	int BFill(){return ValueB->value();};
	int CFill(){return ValueC->value();};
	
	int ANeed(){return NeedA->value();};
//	int BNeed(){return NeedB->value();};
//	int CNeed(){return NeedC->value();};

	void setSizes(int A,int B,int C)
		{
		BaseA->setValue(A);
		BaseB->setValue(B);
		BaseC->setValue(C);
		};
	void setFill(int A,int B,int C)
		{
		qDebug()<<A<<B<<C;
		ValueA->setValue(A);
		ValueB->setValue(B);
		ValueC->setValue(C);
		};
	void setNeed(int A)
		{
		NeedA->setValue(A);
		//NeedB->setValue(B);
		//NeedC->setValue(C);
		};
	void setMaximum()
		{

		};
    void translateButtons()
    {
        QList<QAbstractButton *>btns = buttonBox->buttons();
        for(int i=0;i<btns.count();i++)
        {
            if(buttonBox->buttonRole(btns[i])==QDialogButtonBox::RejectRole)btns[i]->setText(trUtf8("Отмена"));
        }
    }
public slots:
 void setMax();
	
};
