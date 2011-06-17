#include "statusbar.h"
#include "ui_statusbar.h"
#include "utils.h"

namespace Editor {

StatusBar::StatusBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatusBar)
{
    ui->setupUi(this);
    s_gotoLineTemplateText = ui->gotoLineButton->text();
    s_clipboardTemplateText = ui->selectBufferButton->text();
    s_keyboardLayoutTemplateText = ui->keyboardLayout->text();
    s_capsLockTemplateText = ui->capsLock->text();
    i_timerId = startTimer(500);
    setMaximumHeight(32);
    ui->selectBufferButton->setVisible(false);
}


void StatusBar::handleClipboardChanged(int cnt)
{
//    QString v;
//    if (cnt==0)
//        v = tr("Empty");
//    else if (cnt==1)
//        v = tr("%1 text", "One").arg(cnt);
//    else if ( cnt%10==1 && cnt!=11 && cnt!=1)
//        v = tr("%1 texts", "1, 21, 31, etc").arg(cnt);
//    else if ( ( cnt%10==2 && cnt!=12 ) ||
//             ( cnt%10==3 && cnt!=13 ) ||
//             ( cnt%10==4 && cnt!=14 ) )
//        v = tr("%1 texts", "2, 3, 4, 22, 23, 24, etc").arg(cnt);
//    else
//        v = tr("%1 texts", "Generic numerical").arg(cnt);
//    ui->selectBufferButton->setText(s_clipboardTemplateText.arg(v));
}

void StatusBar::timerEvent(QTimerEvent *e)
{
    bool russian = Utils::isRussianLayout();
    if (Utils::temporaryLayoutSwitch)
        russian = !russian;
    bool capsLock = Utils::isCapsLock();
    ui->capsLock->setText(s_capsLockTemplateText.arg(capsLock? tr("On") : tr("Off")));
    ui->keyboardLayout->setText(s_keyboardLayoutTemplateText.arg(russian? tr("Rus") : tr("Lat")));
    e->accept();
}

void StatusBar::handleCursorPositionChanged(int row, int col)
{
    ui->gotoLineButton->setText(s_gotoLineTemplateText.arg(row+1).arg(col+1));
}

StatusBar::~StatusBar()
{
    if (i_timerId>=0)
        killTimer(i_timerId);
    delete ui;
}

} // namespace Editor
