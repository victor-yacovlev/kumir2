/****************************************************************************
**
** Copyright (C) 2004-2008 NIISI RAS. All rights reserved.
**
** This file is part of the KuMir.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include "pult.h"
#include "robotmodule.h"
#include "kumir2-libs/widgets/pultlogger.h"

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

RoboPult::RoboPult(QWidget *parent, Qt::WindowFlags fl) :
	QWidget(parent, fl), Ui::RoboPult()
{
	setupUi(this);
	QDir resDir = ActorRobot::RobotModule::self->myResourcesDir();
	Logger = new pultLogger(resDir, this);
	Logger->setSizes(164, 150);
	Logger->Move(40, 5);
	greenLight = new linkLight(this);
	greenLight->move(15, 29);
	greenLight->resize(12, 104);
	ClearLog->setIcon(QIcon(resDir.absoluteFilePath("edit-delete.png")));
	ToKumir->setIcon(QIcon(resDir.absoluteFilePath("kumir.png")));
	UpB->hide();
	buttUp = new MainButton(resDir, this);
	buttUp->move(90, 170);
	buttUp->setQPos(QPoint(buttUp->size().width() / 2 - 2, 20));

	DownB->hide();
	buttDown = new MainButton(resDir, this);
	buttDown->move(DownB->pos());
	buttDown->setQu(true);
	buttDown->setQPos(QPoint(buttDown->size().width() / 2 - 2, buttDown->size().height() - 10));
	buttDown->setDirection(DOWN);

	LeftB->hide();
	buttLeft = new MainButton(resDir, this);
	buttLeft->move(LeftB->pos());
	buttLeft->setQu(true);
	buttLeft->setQPos(QPoint(15, buttLeft->size().height() / 2 + 3));
	buttLeft->setDirection(LEFT);
	RightB->hide();
	buttRight = new MainButton(resDir, this);
	buttRight->move(RightB->pos());
	buttRight->setQPos(QPoint(buttRight->size().width() - 15, buttRight->size().height() / 2 + 3));
	buttRight->setDirection(RIGHT);

	StenaB->hide();
	askStena = new MainButton(resDir, this);
	askStena->move(StenaB->pos());
	askStena->setCheckable(true);
	askStena->setText(trUtf8("  "));
	askStena->setIconOffset(5);
	askStena->loadIcon(resDir.absoluteFilePath("stena.png"));
	askStena->setCheckable(true);

	SvobodnoB->hide();
	askFree = new MainButton(resDir, this);
	askFree->move(SvobodnoB->pos());
	askFree->setCheckable(true);
	askFree->setText(trUtf8(" "));
	askFree->loadIcon(resDir.absoluteFilePath("svobodno.png"));
	askFree->setIconOffset(10);
	askFree->setCheckable(true);

	RadB->hide();
	buttRad = new MainButton(resDir, this);
	buttRad->move(RadB->pos());

	buttRad->setText(trUtf8(" "));
	buttRad->setIconOffset(10);
	if (!buttRad->loadIcon(resDir.absoluteFilePath("radiation.png"))) {
		qWarning("Image not loaded!");
	}
	TempB->hide();
	buttTemp = new MainButton(resDir, this);
	buttTemp->move(TempB->pos());

	buttTemp->setText(" ");
	buttTemp->setIconOffset(10);
	buttTemp->loadIcon(resDir.absoluteFilePath("tC.png"));

	buttCenter = new MainButton(resDir, this);
	CenterB->hide();
	buttCenter->setText(trUtf8(" "));
	buttCenter->move(CenterB->pos());
	buttCenter->loadIcon(resDir.absoluteFilePath("btn_paint.png"));
	buttCenter->setIconOffset(19);
	buttCenter->setQPos(QPoint(buttCenter->size().width() / 2 - 4, buttCenter->size().height() / 2 + 5));
	//CenterB->setIcon(QIcon(resDir.absoluteFilePath("robo_field.png")));
	connect(buttUp, SIGNAL(clicked()), this, SLOT(Up()));
	connect(buttDown, SIGNAL(clicked()), this, SLOT(Down()));
	connect(buttLeft, SIGNAL(clicked()), this, SLOT(Left()));
	connect(buttRight, SIGNAL(clicked()), this, SLOT(Right()));

	connect(buttRad, SIGNAL(clicked()), this, SLOT(RadS()));
	connect(buttTemp, SIGNAL(clicked()), this, SLOT(TempS()));

	connect(askStena, SIGNAL(clicked()), this, SLOT(SwStena()));
	connect(askFree, SIGNAL(clicked()), this, SLOT(SwSvobodno()));
	connect(buttCenter, SIGNAL(clicked()), this, SLOT(CenterButton()));

	connect(ClearLog, SIGNAL(clicked()), this, SLOT(clearLog()));

	connect(ToKumir, SIGNAL(clicked()), Logger, SLOT(CopyLog()));

	link = true;
}

RoboPult::~RoboPult() {};

void RoboPult::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.save();
	p.setPen(Qt::NoPen);
	p.setBrush(QColor("#8C877E"));
	p.drawRect(0, 0, width(), height());
	p.restore();
	QWidget::paintEvent(event);
}

void RoboPult::noLink()
{
	link = false;
	greenLight->setLink(link);
	greenLight->repaint();
	qWarning("NoLINK");
}

void RoboPult::LinkOK()
{
	link = true;
	greenLight->setLink(link);
	greenLight->repaint();
}

void RoboPult::Up()
{
	if (askStena->isChecked()) {
		emit hasUpWall();
		askStena->setChecked(false);
		switchButt();
		return;
	};
	if (askFree->isChecked()) {
		emit noUpWall();
		askFree->setChecked(false);
		return;
	};
	emit goUp();
}

void RoboPult::Down()
{
	if (askStena->isChecked()) {
		emit hasDownWall();
		askStena->setChecked(false);
		switchButt();
		return;
	}

	if (askFree->isChecked()) {
		emit noDownWall();
		askFree->setChecked(false);
		switchButt();
		return;
	}

	emit goDown();
}

void RoboPult::Left()
{
	if (askStena->isChecked()) {
		emit hasLeftWall();
		askStena->setChecked(false);
		switchButt();
		return;
	}

	if (askFree->isChecked()) {
		emit noLeftWall();
		askFree->setChecked(false);
		switchButt();
		return;
	}

	emit goLeft();
	switchButt();
}

void RoboPult::Right()
{
	if (askStena->isChecked()) {
		emit hasRightWall();
		askStena->setChecked(false);
		switchButt();
		return;
	}

	if (askFree->isChecked()) {
		emit noRightWall();
		askFree->setChecked(false);
		switchButt();
		return;
	}
	emit goRight();
	switchButt();
}

void RoboPult::switchButt()
{
	bool  b_mode = askStena->isChecked() || askFree->isChecked();
	buttLeft->setQmode(b_mode);
	buttRight->setQmode(b_mode);
	buttUp->setQmode(b_mode);
	buttDown->setQmode(b_mode);
	buttCenter->setQmode(b_mode);
}

void RoboPult::SwStena()
{
	if (askFree->isChecked()) {
		askFree->setChecked(false);
	}
	switchButt();
}

void RoboPult::SwSvobodno()
{
	if (askStena->isChecked()) {
		askStena->setChecked(false);
	}
	switchButt();
}

void RoboPult::TempS()
{
	askStena->setChecked(false);
	askFree->setChecked(false);
	switchButt();
	emit Temp();
}

void RoboPult::RadS()
{
	askStena->setChecked(false);
	askFree->setChecked(false);
	switchButt();
	emit Rad();
}

void RoboPult::CenterButton()
{
	if (askStena->isChecked()) {
		emit Colored();
		askStena->setChecked(false);
		switchButt();
		return;
	}

	if (askFree->isChecked()) {
		emit Clean();
		askFree->setChecked(false);
		switchButt();
		return;
	}

	emit Color();
}

void RoboPult::clearLog()
{
	Logger->ClearLog();
	emit robReset();
}

