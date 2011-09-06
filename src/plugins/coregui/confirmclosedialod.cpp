#include "confirmclosedialod.h"
#include "ui_confirmclosedialod.h"

namespace CoreGUI {

ConfirmCloseDialod::ConfirmCloseDialod(bool sessionMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmCloseDialod)
{
    ui->setupUi(this);
    if (sessionMode) {
        ui->btnSaveOneFile->setVisible(false);
    }
    else {
        ui->btnSaveFiles->setVisible(false);
        ui->btnSaveSession->setVisible(false);
    }
    e_result = Cancel;
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));
    connect(ui->btnDontSave, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));
    connect(ui->btnSaveOneFile, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));
    connect(ui->btnSaveFiles, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));
    connect(ui->btnSaveSession, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));
    connect(ui->btnDontSave, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));
}

ConfirmCloseDialod::~ConfirmCloseDialod()
{
    delete ui;
}

void ConfirmCloseDialod::setMainText(const QString &text)
{
    ui->label->setText(text);
}

void ConfirmCloseDialod::setCancelText(const QString &text)
{
    ui->btnCancel->setText(text);
}

void ConfirmCloseDialod::setDiscardText(const QString &text)
{
    ui->btnDontSave->setText(text);
}

void ConfirmCloseDialod::handleButtonClicked()
{
    if (sender()==ui->btnSaveFiles || sender()==ui->btnSaveOneFile)
        e_result = SaveFiles;
    else if (sender()==ui->btnDontSave)
        e_result = SaveNothing;
    else if (sender()==ui->btnSaveSession)
        e_result = SaveSession;
    accept();
}

} // namespace CoreGUI
