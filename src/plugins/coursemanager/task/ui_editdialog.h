/********************************************************************************
** Form generated from reading UI file 'editdialog.ui'
**
** Created: Thu Apr 26 15:27:56 2012
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITDIALOG_H
#define UI_EDITDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EditDialog
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QLineEdit *TitleEdit;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QTextEdit *DescEdit;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label_5;
    QLineEdit *PrgEdit;
    QPushButton *prgButt;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_2;
    QFrame *frame_5;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_3;
    QComboBox *IspCombo;
    QFrame *frame_6;
    QVBoxLayout *verticalLayout_5;
    QFrame *frame_7;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QPushButton *ButtonAddEnv;
    QPushButton *remEnvButt;
    QListWidget *EnvList;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *EditDialog)
    {
        if (EditDialog->objectName().isEmpty())
            EditDialog->setObjectName(QString::fromUtf8("EditDialog"));
        EditDialog->resize(634, 590);
        horizontalLayout_3 = new QHBoxLayout(EditDialog);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(EditDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_3->addWidget(label);

        TitleEdit = new QLineEdit(frame);
        TitleEdit->setObjectName(QString::fromUtf8("TitleEdit"));

        verticalLayout_3->addWidget(TitleEdit);


        verticalLayout->addWidget(frame);

        frame_2 = new QFrame(EditDialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);

        DescEdit = new QTextEdit(frame_2);
        DescEdit->setObjectName(QString::fromUtf8("DescEdit"));

        verticalLayout_2->addWidget(DescEdit);

        frame_4 = new QFrame(frame_2);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_4);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_5 = new QLabel(frame_4);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout->addWidget(label_5);

        PrgEdit = new QLineEdit(frame_4);
        PrgEdit->setObjectName(QString::fromUtf8("PrgEdit"));

        horizontalLayout->addWidget(PrgEdit);

        prgButt = new QPushButton(frame_4);
        prgButt->setObjectName(QString::fromUtf8("prgButt"));

        horizontalLayout->addWidget(prgButt);


        verticalLayout_2->addWidget(frame_4);


        verticalLayout->addWidget(frame_2);

        frame_3 = new QFrame(EditDialog);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setMaximumSize(QSize(16777215, 200));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_3);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        frame_5 = new QFrame(frame_3);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(frame_5);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_3 = new QLabel(frame_5);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);

        IspCombo = new QComboBox(frame_5);
        IspCombo->setObjectName(QString::fromUtf8("IspCombo"));
        IspCombo->setMinimumSize(QSize(200, 0));

        verticalLayout_4->addWidget(IspCombo);


        horizontalLayout_2->addWidget(frame_5);

        frame_6 = new QFrame(frame_3);
        frame_6->setObjectName(QString::fromUtf8("frame_6"));
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(frame_6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        frame_7 = new QFrame(frame_6);
        frame_7->setObjectName(QString::fromUtf8("frame_7"));
        frame_7->setFrameShape(QFrame::StyledPanel);
        frame_7->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_7);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(frame_7);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        ButtonAddEnv = new QPushButton(frame_7);
        ButtonAddEnv->setObjectName(QString::fromUtf8("ButtonAddEnv"));

        horizontalLayout_4->addWidget(ButtonAddEnv);

        remEnvButt = new QPushButton(frame_7);
        remEnvButt->setObjectName(QString::fromUtf8("remEnvButt"));

        horizontalLayout_4->addWidget(remEnvButt);


        verticalLayout_5->addWidget(frame_7);

        EnvList = new QListWidget(frame_6);
        EnvList->setObjectName(QString::fromUtf8("EnvList"));

        verticalLayout_5->addWidget(EnvList);


        horizontalLayout_2->addWidget(frame_6);


        verticalLayout->addWidget(frame_3);

        buttonBox = new QDialogButtonBox(EditDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        horizontalLayout_3->addLayout(verticalLayout);


        retranslateUi(EditDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), EditDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), EditDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(EditDialog);
    } // setupUi

    void retranslateUi(QDialog *EditDialog)
    {
        EditDialog->setWindowTitle(QApplication::translate("EditDialog", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("EditDialog", "\320\227\320\260\320\263\320\276\320\273\320\276\320\262\320\276\320\272", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("EditDialog", "\320\242\320\265\320\272\321\201\321\202 \320\267\320\260\320\264\320\260\320\275\320\270\321\217:", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("EditDialog", "\320\237\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\260:", 0, QApplication::UnicodeUTF8));
        prgButt->setText(QApplication::translate("EditDialog", "...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("EditDialog", "\320\230\321\201\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\214", 0, QApplication::UnicodeUTF8));
        IspCombo->clear();
        IspCombo->insertItems(0, QStringList()
         << QApplication::translate("EditDialog", "\320\235\320\265\321\202 \320\270\321\201\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\217", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("EditDialog", "Robot", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("EditDialog", "\320\222\320\276\320\264\320\276\320\273\320\265\320\271", 0, QApplication::UnicodeUTF8)
        );
        label_4->setText(QApplication::translate("EditDialog", "\320\236\320\261\321\201\321\202\320\260\320\275\320\276\320\262\320\272\320\270", 0, QApplication::UnicodeUTF8));
        ButtonAddEnv->setText(QApplication::translate("EditDialog", "+", 0, QApplication::UnicodeUTF8));
        remEnvButt->setText(QApplication::translate("EditDialog", "-", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EditDialog: public Ui_EditDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITDIALOG_H
