/********************************************************************************
** Form generated from reading UI file 'newkursdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWKURSDIALOG_H
#define UI_NEWKURSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_newKursDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLineEdit *nameEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *newKursDialog)
    {
        if (newKursDialog->objectName().isEmpty())
            newKursDialog->setObjectName(QStringLiteral("newKursDialog"));
        newKursDialog->resize(426, 131);
        verticalLayout = new QVBoxLayout(newKursDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(newKursDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_2->addWidget(label);

        nameEdit = new QLineEdit(frame);
        nameEdit->setObjectName(QStringLiteral("nameEdit"));

        verticalLayout_2->addWidget(nameEdit);


        verticalLayout->addWidget(frame);

        buttonBox = new QDialogButtonBox(newKursDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(newKursDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), newKursDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), newKursDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(newKursDialog);
    } // setupUi

    void retranslateUi(QDialog *newKursDialog)
    {
        newKursDialog->setWindowTitle(QApplication::translate("newKursDialog", "\320\235\320\276\320\262\321\213\320\271 \320\272\321\203\321\200\321\201", 0));
        label->setText(QApplication::translate("newKursDialog", "\320\235\320\260\320\267\320\262\320\260\320\275\320\270\320\265 \320\272\321\203\321\200\321\201\320\260:", 0));
    } // retranslateUi

};

namespace Ui {
    class newKursDialog: public Ui_newKursDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWKURSDIALOG_H
