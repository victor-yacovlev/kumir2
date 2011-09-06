#ifndef COREGUI_CONFIRMCLOSEDIALOD_H
#define COREGUI_CONFIRMCLOSEDIALOD_H

#include <QDialog>

namespace CoreGUI {

namespace Ui {
    class ConfirmCloseDialod;
}

class ConfirmCloseDialod : public QDialog
{
    Q_OBJECT
public:
    enum Result { Cancel, SaveFiles, SaveSession, SaveNothing };
    explicit ConfirmCloseDialod(bool sessionMode, QWidget *parent = 0);
    ~ConfirmCloseDialod();
    inline Result result() const { return e_result; }
    void setMainText(const QString & text);
    void setCancelText(const QString & text);
    void setDiscardText(const QString & text);
private slots:
    void handleButtonClicked();
private:
    Ui::ConfirmCloseDialod *ui;
    Result e_result;
};


} // namespace CoreGUI
#endif // COREGUI_CONFIRMCLOSEDIALOD_H
