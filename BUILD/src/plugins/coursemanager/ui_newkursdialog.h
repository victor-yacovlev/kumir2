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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
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
    QLabel *label_2;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout;
    QLineEdit *fileEdit;
    QPushButton *fileButt;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *newKursDialog)
    {
        if (newKursDialog->objectName().isEmpty())
            newKursDialog->setObjectName(QStringLiteral("newKursDialog"));
        newKursDialog->resize(429, 267);
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

        label_2 = new QLabel(newKursDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(label_2);

        frame_2 = new QFrame(newKursDialog);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_2);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        fileEdit = new QLineEdit(frame_2);
        fileEdit->setObjectName(QStringLiteral("fileEdit"));

        horizontalLayout->addWidget(fileEdit);

        fileButt = new QPushButton(frame_2);
        fileButt->setObjectName(QStringLiteral("fileButt"));

        horizontalLayout->addWidget(fileButt);


        verticalLayout->addWidget(frame_2);

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
        label_2->setText(QApplication::translate("newKursDialog", "\320\244\320\260\320\271\320\273:", 0));
        fileButt->setText(QApplication::translate("newKursDialog", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class newKursDialog: public Ui_newKursDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWKURSDIALOG_H
