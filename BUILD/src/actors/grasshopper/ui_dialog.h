/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout_5;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *BaseA;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_2;
    QSpinBox *BaseB;
    QSpacerItem *verticalSpacer_3;
    QCheckBox *taskNeeded;
    QFrame *frame_3;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_4;
    QSpinBox *StartPos;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_3;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *NeedA;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_3;
    QListWidget *listWidget;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_2;
    QCheckBox *Borders;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QSpinBox *spinBox;
    QLabel *label_7;
    QSpinBox *spinBox_2;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(289, 513);
        Dialog->setMinimumSize(QSize(0, 0));
        Dialog->setMaximumSize(QSize(600, 600));
        verticalLayout_5 = new QVBoxLayout(Dialog);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        frame = new QFrame(Dialog);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setMinimumSize(QSize(0, 0));
        frame->setMaximumSize(QSize(16777215, 80));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));

        verticalLayout->addWidget(label_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        BaseA = new QSpinBox(frame);
        BaseA->setObjectName(QStringLiteral("BaseA"));
        BaseA->setMinimumSize(QSize(50, 20));
        BaseA->setMaximum(999);
        BaseA->setValue(3);

        horizontalLayout->addWidget(BaseA);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout->addItem(verticalSpacer_2);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        BaseB = new QSpinBox(frame);
        BaseB->setObjectName(QStringLiteral("BaseB"));
        BaseB->setMaximum(999);
        BaseB->setValue(2);

        horizontalLayout->addWidget(BaseB);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_5->addWidget(frame);

        verticalSpacer_3 = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_5->addItem(verticalSpacer_3);

        taskNeeded = new QCheckBox(Dialog);
        taskNeeded->setObjectName(QStringLiteral("taskNeeded"));
        taskNeeded->setChecked(false);

        verticalLayout_5->addWidget(taskNeeded);

        frame_3 = new QFrame(Dialog);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setEnabled(false);
        frame_3->setMinimumSize(QSize(0, 300));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_4 = new QLabel(frame_3);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_5->addWidget(label_4);

        StartPos = new QSpinBox(frame_3);
        StartPos->setObjectName(QStringLiteral("StartPos"));
        StartPos->setMinimumSize(QSize(70, 0));
        StartPos->setMinimum(-999);
        StartPos->setMaximum(999);

        horizontalLayout_5->addWidget(StartPos);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayout_5);

        label_3 = new QLabel(frame_3);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_2->addWidget(label_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        NeedA = new QSpinBox(frame_3);
        NeedA->setObjectName(QStringLiteral("NeedA"));
        NeedA->setEnabled(true);
        NeedA->setMinimumSize(QSize(150, 0));
        NeedA->setFrame(true);
        NeedA->setMinimum(-999);
        NeedA->setMaximum(999);
        NeedA->setValue(1);

        horizontalLayout_2->addWidget(NeedA);

        horizontalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        pushButton = new QPushButton(frame_3);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        listWidget = new QListWidget(frame_3);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        horizontalLayout_3->addWidget(listWidget);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        pushButton_2 = new QPushButton(frame_3);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setEnabled(false);

        horizontalLayout_3->addWidget(pushButton_2);


        verticalLayout_2->addLayout(horizontalLayout_3);

        Borders = new QCheckBox(frame_3);
        Borders->setObjectName(QStringLiteral("Borders"));

        verticalLayout_2->addWidget(Borders);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_6 = new QLabel(frame_3);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setMaximumSize(QSize(40, 30));

        horizontalLayout_4->addWidget(label_6);

        spinBox = new QSpinBox(frame_3);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setEnabled(false);
        spinBox->setMinimum(-999);
        spinBox->setMaximum(999);

        horizontalLayout_4->addWidget(spinBox);

        label_7 = new QLabel(frame_3);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_4->addWidget(label_7);

        spinBox_2 = new QSpinBox(frame_3);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));
        spinBox_2->setEnabled(false);
        spinBox_2->setMinimum(-998);
        spinBox_2->setMaximum(999);
        spinBox_2->setValue(1);

        horizontalLayout_4->addWidget(spinBox_2);


        verticalLayout_2->addLayout(horizontalLayout_4);


        gridLayout->addLayout(verticalLayout_2, 0, 0, 1, 1);


        verticalLayout_5->addWidget(frame_3);

        verticalSpacer = new QSpacerItem(9, 9, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_5->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_5->addWidget(buttonBox);

        frame->raise();
        buttonBox->raise();
        frame_3->raise();
        taskNeeded->raise();

        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));
        QObject::connect(taskNeeded, SIGNAL(clicked(bool)), frame_3, SLOT(setEnabled(bool)));
        QObject::connect(Borders, SIGNAL(clicked(bool)), spinBox, SLOT(setEnabled(bool)));
        QObject::connect(Borders, SIGNAL(clicked(bool)), spinBox_2, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "\320\227\320\260\320\264\320\260\320\275\320\270\320\265", 0));
        label_5->setText(QApplication::translate("Dialog", "\320\240\320\260\320\267\320\274\320\265\321\200 \320\277\321\200\321\213\320\266\320\272\320\260:", 0));
        label->setText(QApplication::translate("Dialog", "\320\222\320\277\320\265\321\200\320\265\320\264:", 0));
        label_2->setText(QApplication::translate("Dialog", "\320\235\320\260\320\267\320\260\320\264:", 0));
        taskNeeded->setText(QApplication::translate("Dialog", "\320\227\320\260\320\264\320\260\320\275\320\270\320\265", 0));
        label_4->setText(QApplication::translate("Dialog", "\320\241\321\202\320\260\321\200\321\202:", 0));
        label_3->setText(QApplication::translate("Dialog", "\320\244\320\273\320\260\320\266\320\272\320\270", 0));
        pushButton->setText(QApplication::translate("Dialog", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", 0));
        pushButton_2->setText(QApplication::translate("Dialog", "\320\243\320\261\321\200\320\260\321\202\321\214", 0));
        Borders->setText(QApplication::translate("Dialog", "\320\223\321\200\320\260\320\275\320\270\321\206\321\213", 0));
        label_6->setText(QApplication::translate("Dialog", "\320\236\321\202:", 0));
        label_7->setText(QApplication::translate("Dialog", "\320\224\320\276:", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
