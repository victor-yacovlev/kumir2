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

	
	int ANeed(){return NeedA->value();};
//	int BNeed(){return NeedB->value();};
//	int CNeed(){return NeedC->value();};

	void setSizes(int A,int B)
		{
		BaseA->setValue(A);
		BaseB->setValue(B);
		};


	int getStart() 
		{
		if(!taskNeeded->isChecked())return 0;
		return StartPos->value();
		};
	QList<int> getFlags()
		{
		
		QList<int> toRet;
		if(!taskNeeded->isChecked())return toRet;
		 for(int i=0;i<listWidget->count ();i++)
				toRet.append(listWidget->item(i)->text().toInt());
		return toRet;
		};
	int getLeftBorder()
		{
		if(!taskNeeded->isChecked())return 0;
		return spinBox->value();
		};
	int getRightBorder()
		{
		if(!taskNeeded->isChecked())return 0;
		return spinBox_2->value();
		};
	void setLeftBorder(int pos)
		{
		spinBox->setValue(pos);
		};
	void setRightBorder(int pos)
		{
		spinBox_2->setValue(pos);
		};
	void setStart(int pos)
		{
		StartPos->setValue(pos);
		};
	void setFlags(QList<int> flags)
		{
		listWidget->clear();
		for(int i=0;i<flags.count();i++)
		listWidget->addItem(QString::number(flags[i]));
		};
	bool borders()
		{
		return Borders->isChecked();
		};
	void setBordersChB(bool brd)
		{
		Borders->setChecked(brd);
		if(!brd)return;
		spinBox->setEnabled(true);
		spinBox_2->setEnabled(true);
		};
        bool isTaskNeeded()
		{
		return taskNeeded->isChecked();
		};
        void setTaskNeeded(bool task)
		{
		taskNeeded->setChecked(task);
		if(!task)return;
		frame_3->setEnabled(true);
		qDebug()<<"frame_3->ENABLED!";
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
 void addFlag();
 void enableDelete();
 void removeFlag();

	
};
