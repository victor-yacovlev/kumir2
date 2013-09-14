/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created: Fri Mar 2 16:59:05 2012
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

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
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QSpinBox *BaseB;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QSpinBox *BaseC;
    QSpacerItem *verticalSpacer_2;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_11;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QSpinBox *ValueA;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label_7;
    QSpinBox *ValueB;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_6;
    QSpinBox *ValueC;
    QSpacerItem *verticalSpacer_3;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *NeedA;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(289, 345);
        Dialog->setMinimumSize(QSize(0, 0));
        Dialog->setMaximumSize(QSize(600, 400));
        verticalLayout_5 = new QVBoxLayout(Dialog);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        frame = new QFrame(Dialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setMinimumSize(QSize(0, 0));
        frame->setMaximumSize(QSize(16777215, 100));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_5 = new QLabel(frame);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        verticalLayout->addWidget(label_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        BaseA = new QSpinBox(frame);
        BaseA->setObjectName(QString::fromUtf8("BaseA"));
        BaseA->setMinimumSize(QSize(50, 20));
        BaseA->setMinimum(-1);
        BaseA->setMaximum(999);
        BaseA->setValue(8);

        horizontalLayout->addWidget(BaseA);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout->addWidget(label_2);

        BaseB = new QSpinBox(frame);
        BaseB->setObjectName(QString::fromUtf8("BaseB"));
        BaseB->setMinimum(1);
        BaseB->setMaximum(999);
        BaseB->setValue(5);

        horizontalLayout->addWidget(BaseB);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        label_4 = new QLabel(frame);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        BaseC = new QSpinBox(frame);
        BaseC->setObjectName(QString::fromUtf8("BaseC"));
        BaseC->setMaximum(999);
        BaseC->setValue(3);

        horizontalLayout->addWidget(BaseC);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_5->addWidget(frame);

        verticalSpacer_2 = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_5->addItem(verticalSpacer_2);

        frame_2 = new QFrame(Dialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(frame_2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_11 = new QLabel(frame_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        verticalLayout_4->addWidget(label_11);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMaximumSize);
        label_3 = new QLabel(frame_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        ValueA = new QSpinBox(frame_2);
        ValueA->setObjectName(QString::fromUtf8("ValueA"));
        ValueA->setMaximum(999);
        ValueA->setValue(8);

        horizontalLayout_3->addWidget(ValueA);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        label_7 = new QLabel(frame_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_3->addWidget(label_7);

        ValueB = new QSpinBox(frame_2);
        ValueB->setObjectName(QString::fromUtf8("ValueB"));
        ValueB->setMaximum(999);

        horizontalLayout_3->addWidget(ValueB);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        label_6 = new QLabel(frame_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_3->addWidget(label_6);

        ValueC = new QSpinBox(frame_2);
        ValueC->setObjectName(QString::fromUtf8("ValueC"));
        ValueC->setMaximum(999);

        horizontalLayout_3->addWidget(ValueC);


        verticalLayout_4->addLayout(horizontalLayout_3);


        verticalLayout_5->addWidget(frame_2);

        verticalSpacer_3 = new QSpacerItem(5, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_5->addItem(verticalSpacer_3);

        frame_3 = new QFrame(Dialog);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame_3);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_12 = new QLabel(frame_3);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        verticalLayout_3->addWidget(label_12);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        NeedA = new QSpinBox(frame_3);
        NeedA->setObjectName(QString::fromUtf8("NeedA"));
        NeedA->setValue(4);

        horizontalLayout_2->addWidget(NeedA);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout_5->addWidget(frame_3);

        verticalSpacer = new QSpacerItem(9, 9, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_5->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_5->addWidget(buttonBox);


        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "\320\227\320\260\320\264\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("Dialog", "\320\240\320\260\320\267\320\274\320\265\321\200:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "A:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "B:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("Dialog", "C:", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("Dialog", "\320\235\320\260\320\273\320\270\321\202\320\276:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Dialog", "A:", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("Dialog", "B:", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("Dialog", "C:", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("Dialog", "\320\236\321\202\320\274\320\265\321\200\320\270\321\202\321\214:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
