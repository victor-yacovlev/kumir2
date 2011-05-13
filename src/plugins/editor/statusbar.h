#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QWidget>

namespace Editor {

namespace Ui {
    class StatusBar;
}

class StatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = 0);
    ~StatusBar();
public slots:
    void handleCursorPositionChanged(int row, int col);
    void handleClipboardChanged(int cnt);
protected:
    void timerEvent(QTimerEvent *e);
private:
    Ui::StatusBar *ui;
    int i_timerId;
    QString s_gotoLineTemplateText;
    QString s_keyboardLayoutTemplateText;
    QString s_capsLockTemplateText;
    QString s_clipboardTemplateText;
};


} // namespace Editor
#endif // STATUSBAR_H
