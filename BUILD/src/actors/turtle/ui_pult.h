/********************************************************************************
** Form generated from reading UI file 'pult.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PULT_H
#define UI_PULT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TurtlePult
{
public:
    QPushButton *TempB;
    QPushButton *UpB;
    QPushButton *RightB;
    QPushButton *LeftB;
    QPushButton *DownB;
    QPushButton *RadB;
    QToolButton *ClearLog;
    QSpinBox *stepVal;
    QSpinBox *gradVal;
    QFrame *gradProto;
    QFrame *frame;
    QLabel *label;
    QToolButton *toKumir;

    void setupUi(QWidget *TurtlePult)
    {
        if (TurtlePult->objectName().isEmpty())
            TurtlePult->setObjectName(QStringLiteral("TurtlePult"));
        TurtlePult->resize(254, 434);
        TurtlePult->setMinimumSize(QSize(254, 434));
        TurtlePult->setMaximumSize(QSize(254, 434));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(223, 196, 153, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(255, 255, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(239, 225, 204, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(111, 98, 76, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(149, 131, 102, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush3);
        QBrush brush6(QColor(255, 255, 220, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        TurtlePult->setPalette(palette);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        TurtlePult->setFont(font);
        TempB = new QPushButton(TurtlePult);
        TempB->setObjectName(QStringLiteral("TempB"));
        TempB->setGeometry(QRect(140, 210, 101, 41));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        TempB->setFont(font1);
        UpB = new QPushButton(TurtlePult);
        UpB->setObjectName(QStringLiteral("UpB"));
        UpB->setGeometry(QRect(140, 310, 101, 41));
        RightB = new QPushButton(TurtlePult);
        RightB->setObjectName(QStringLiteral("RightB"));
        RightB->setGeometry(QRect(10, 360, 101, 41));
        LeftB = new QPushButton(TurtlePult);
        LeftB->setObjectName(QStringLiteral("LeftB"));
        LeftB->setGeometry(QRect(10, 310, 101, 41));
        DownB = new QPushButton(TurtlePult);
        DownB->setObjectName(QStringLiteral("DownB"));
        DownB->setGeometry(QRect(140, 360, 101, 41));
        RadB = new QPushButton(TurtlePult);
        RadB->setObjectName(QStringLiteral("RadB"));
        RadB->setGeometry(QRect(140, 160, 101, 41));
        RadB->setFont(font1);
        ClearLog = new QToolButton(TurtlePult);
        ClearLog->setObjectName(QStringLiteral("ClearLog"));
        ClearLog->setGeometry(QRect(210, 30, 31, 45));
        QIcon icon;
        icon.addFile(QStringLiteral(":/clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        ClearLog->setIcon(icon);
        stepVal = new QSpinBox(TurtlePult);
        stepVal->setObjectName(QStringLiteral("stepVal"));
        stepVal->setGeometry(QRect(140, 270, 101, 30));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush7(QColor(216, 255, 197, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Light, brush7);
        QBrush brush8(QColor(194, 167, 239, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Midlight, brush8);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush2);
        QBrush brush9(QColor(100, 100, 100, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush9);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Light, brush7);
        palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush8);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush2);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush9);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Light, brush7);
        palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush8);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        stepVal->setPalette(palette1);
        stepVal->setMinimum(0);
        stepVal->setMaximum(500);
        stepVal->setValue(50);
        gradVal = new QSpinBox(TurtlePult);
        gradVal->setObjectName(QStringLiteral("gradVal"));
        gradVal->setGeometry(QRect(10, 270, 101, 30));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Light, brush7);
        palette2.setBrush(QPalette::Active, QPalette::Midlight, brush8);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush2);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush2);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush9);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Light, brush7);
        palette2.setBrush(QPalette::Inactive, QPalette::Midlight, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush2);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush9);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Light, brush7);
        palette2.setBrush(QPalette::Disabled, QPalette::Midlight, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        gradVal->setPalette(palette2);
        gradVal->setMaximum(360);
        gradVal->setValue(45);
        gradProto = new QFrame(TurtlePult);
        gradProto->setObjectName(QStringLiteral("gradProto"));
        gradProto->setGeometry(QRect(10, 160, 101, 91));
        gradProto->setFrameShape(QFrame::StyledPanel);
        gradProto->setFrameShadow(QFrame::Raised);
        frame = new QFrame(TurtlePult);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(10, 410, 231, 21));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 2, 211, 20));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(3);
        sizePolicy.setVerticalStretch(60);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        toKumir = new QToolButton(TurtlePult);
        toKumir->setObjectName(QStringLiteral("toKumir"));
        toKumir->setEnabled(false);
        toKumir->setGeometry(QRect(210, 85, 31, 45));
        QPalette palette3;
        QBrush brush10(QColor(170, 170, 0, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette3.setBrush(QPalette::Active, QPalette::Light, brush10);
        palette3.setBrush(QPalette::Inactive, QPalette::Light, brush10);
        palette3.setBrush(QPalette::Disabled, QPalette::Light, brush10);
        toKumir->setPalette(palette3);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/img/kumir.png"), QSize(), QIcon::Normal, QIcon::Off);
        toKumir->setIcon(icon1);

        retranslateUi(TurtlePult);

        QMetaObject::connectSlotsByName(TurtlePult);
    } // setupUi

    void retranslateUi(QWidget *TurtlePult)
    {
        TurtlePult->setWindowTitle(QApplication::translate("TurtlePult", "\320\237\321\203\320\273\321\214\321\202", 0));
        TempB->setText(QApplication::translate("TurtlePult", "Temp", 0));
        UpB->setText(QApplication::translate("TurtlePult", "Up", 0));
        RightB->setText(QApplication::translate("TurtlePult", "Right", 0));
        LeftB->setText(QApplication::translate("TurtlePult", "Left", 0));
        DownB->setText(QApplication::translate("TurtlePult", "Down", 0));
        RadB->setText(QApplication::translate("TurtlePult", "Rad", 0));
        ClearLog->setText(QApplication::translate("TurtlePult", "...", 0));
        label->setText(QString());
        toKumir->setText(QApplication::translate("TurtlePult", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class TurtlePult: public Ui_TurtlePult {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PULT_H
