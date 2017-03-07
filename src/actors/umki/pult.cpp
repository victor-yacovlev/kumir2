#include "pult.h"
#include "umkimodule.h"


UmkiPult::UmkiPult ( QWidget* parent, Qt::WindowFlags fl )
	: QWidget ( parent, fl ), Ui::UmkiPult()
{
	
	setupUi ( this );
	Logger=new pultLogger(ActorUmki::UmkiModule::self->myResourcesDir(),this);
	Logger->setSizes(164,150);
	Logger->Move(40,5);	
        greenLight=new linkLight(this);
        greenLight->move(15,29);
        greenLight->resize(12,104);
    ClearLog->setIcon(QIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("edit-delete.png")));
    ToKumir->setIcon(QIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("kumir.png")));
        UpB->hide();
        buttUp=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        buttUp->move(90,170);
        buttUp->setQPos(QPoint(buttUp->size().width()/2-2,20));
    
        DownB->hide();
        buttDown=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        buttDown->move(DownB->pos());
        buttDown->setQu(true);
    buttDown->setQPos(QPoint(buttDown->size().width()/2-2,buttDown->size().height()-10));
	buttDown->setDirection(DOWN);

        LeftB->hide();
        buttLeft=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        buttLeft->move(LeftB->pos());
        buttLeft->setQu(true);
        buttLeft->setQPos(QPoint(15,buttLeft->size().height()/2+3));
        buttLeft->setDirection(LEFT);
        RightB->hide();
        buttRight=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        buttRight->move(RightB->pos());
        buttRight->setQPos(QPoint(buttRight->size().width()-15,buttRight->size().height()/2+3));
        buttRight->setDirection(RIGHT);
        
        StenaB->hide();
        askStena=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        askStena->move(StenaB->pos());
        askStena->setCheckable(true);
	askStena->setText(trUtf8("  "));
     askStena->setIconOffset(5);
	askStena->loadIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("stena.png"));
	askStena->setCheckable(true);

        SvobodnoB->hide();
        askFree=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        askFree->move(SvobodnoB->pos());
        askFree->setCheckable(true);
	askFree->setText(trUtf8(" "));
	askFree->loadIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("svobodno.png"));
     askFree->setIconOffset(10);
	askFree->setCheckable(true);

        RadB->hide();
        buttRad=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        buttRad->move(RadB->pos());
  
	buttRad->setText(trUtf8(" "));
     buttRad->setIconOffset(10);
	if(!buttRad->loadIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("radiation.png")))qWarning("Image not loaded!");
	TempB->hide();
        buttTemp=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
        buttTemp->move(TempB->pos());
    
	buttTemp->setText(" ");
    buttTemp->setIconOffset(10);
	buttTemp->loadIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("tC.png"));

    buttCenter=new MainButton(ActorUmki::UmkiModule::self->myResourcesDir(),this);
    CenterB->hide();
    buttCenter->setText(trUtf8(" "));
    buttCenter->move(CenterB->pos());
    buttCenter->loadIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("btn_paint.png"));
    buttCenter->setIconOffset(19);
    buttCenter->setQPos(QPoint(buttCenter->size().width()/2-4,buttCenter->size().height()/2+5));
	//CenterB->setIcon(QIcon(ActorUmki::UmkiModule::self->myResourcesDir().absoluteFilePath("robo_field.png")));
	connect(buttUp,SIGNAL(clicked()),this,SLOT(Up()));
	connect(buttDown,SIGNAL(clicked()),this,SLOT(Down()));
	connect(buttLeft,SIGNAL(clicked()),this,SLOT(Left()));
	connect(buttRight,SIGNAL(clicked()),this,SLOT(Right()));

	connect(buttRad,SIGNAL(clicked()),this,SLOT(RadS()));
	connect(buttTemp,SIGNAL(clicked()),this,SLOT(TempS()));

	connect(askStena,SIGNAL(clicked()),this,SLOT(SwStena()));
	connect(askFree,SIGNAL(clicked()),this,SLOT(SwSvobodno()));
	connect(buttCenter,SIGNAL(clicked()),this,SLOT(CenterButton()));

	connect(ClearLog,SIGNAL(clicked()),Logger,SLOT(ClearLog()));

	connect(ToKumir,SIGNAL(clicked()),Logger,SLOT(CopyLog()));

	link=true;
};

void UmkiPult::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.save();
	p.setPen(Qt::NoPen);
	p.setBrush(QColor("#8C877E"));
	p.drawRect(0,0,width(),height());
	p.restore();
	QWidget::paintEvent(event);
}

void UmkiPult::noLink(){
	link=false;
	greenLight->setLink(link);greenLight->repaint();qWarning("NoLINK");};
void UmkiPult::LinkOK(){link=true;greenLight->setLink(link);greenLight->repaint();};

void UmkiPult::Up()
  {
  if(askStena->isChecked () ){emit hasUpWall(); askStena->setChecked(false);switchButt();return;};
  if(askFree->isChecked () ){emit noUpWall(); askFree->setChecked(false);return;};
  emit goUp();
  };
void UmkiPult::Down()
  {
      if(askStena->isChecked () ){emit hasDownWall(); askStena->setChecked(false);switchButt();return;};
      if(askFree->isChecked () ){emit noDownWall(); askFree->setChecked(false);switchButt();return;};
  emit goDown();
  };
void UmkiPult::Left()
  {
      
      if(askStena->isChecked () ){emit hasLeftWall(); askStena->setChecked(false);switchButt();return;};
      if(askFree->isChecked () ){emit noLeftWall(); askFree->setChecked(false);switchButt();return;};
  emit goLeft();
      switchButt();
  };
void UmkiPult::Right()
  {
     
      if(askStena->isChecked () ){emit hasRightWall(); askStena->setChecked(false);switchButt();return;};
      if(askFree->isChecked () ){emit noRightWall(); askFree->setChecked(false);switchButt();return;};
  emit goRight();
      switchButt();
  };
void UmkiPult::switchButt()
{
  bool  b_mode=askStena->isChecked()||askFree->isChecked ();
    buttLeft->setQmode(b_mode);
    buttRight->setQmode(b_mode);
    buttUp->setQmode(b_mode);
    buttDown->setQmode(b_mode);
    buttCenter->setQmode(b_mode);
};
void UmkiPult::SwStena()
  {
   if(askFree->isChecked () ) askFree->setChecked(false);
      switchButt();
  };
void UmkiPult::SwSvobodno()
  {
   if(askStena->isChecked () )askStena->setChecked(false);
      switchButt();
  };
void UmkiPult::TempS()
  {
   askStena->setChecked(false);	
   askFree->setChecked(false);
      switchButt();
  emit Temp();
  };
void UmkiPult::RadS()
  {
   askStena->setChecked(false);	
   askFree->setChecked(false);
      switchButt();
  emit Rad();
  };

void UmkiPult::CenterButton()
  {
  if(askStena->isChecked () ){emit Colored(); askStena->setChecked(false);switchButt();return;};
  if(askFree->isChecked () ){emit Clean(); askFree->setChecked(false);switchButt();return;};
  emit Color();
  };

