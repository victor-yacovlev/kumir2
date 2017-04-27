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
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RoboPult
{
public:
    QPushButton *TempB;
    QPushButton *StenaB;
    QPushButton *UpB;
    QPushButton *RightB;
    QPushButton *LeftB;
    QPushButton *DownB;
    QPushButton *RadB;
    QPushButton *SvobodnoB;
    QToolButton *ClearLog;
    QToolButton *CenterB;
    QToolButton *ToKumir;

    void setupUi(QWidget *RoboPult)
    {
        if (RoboPult->objectName().isEmpty())
            RoboPult->setObjectName(QStringLiteral("RoboPult"));
        RoboPult->resize(254, 416);
        RoboPult->setMinimumSize(QSize(254, 416));
        RoboPult->setMaximumSize(QSize(254, 416));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(148, 143, 133, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(222, 215, 200, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(185, 179, 166, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(74, 71, 66, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(99, 95, 89, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        QBrush brush6(QColor(255, 255, 255, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        QBrush brush7(QColor(201, 199, 194, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush7);
        QBrush brush8(QColor(255, 255, 220, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        RoboPult->setPalette(palette);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        RoboPult->setFont(font);
        TempB = new QPushButton(RoboPult);
        TempB->setObjectName(QStringLiteral("TempB"));
        TempB->setGeometry(QRect(10, 170, 71, 71));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        TempB->setFont(font1);
        StenaB = new QPushButton(RoboPult);
        StenaB->setObjectName(QStringLiteral("StenaB"));
        StenaB->setGeometry(QRect(170, 170, 71, 71));
        StenaB->setFont(font1);
        StenaB->setCheckable(true);
        StenaB->setChecked(false);
        UpB = new QPushButton(RoboPult);
        UpB->setObjectName(QStringLiteral("UpB"));
        UpB->setGeometry(QRect(90, 170, 71, 71));
        RightB = new QPushButton(RoboPult);
        RightB->setObjectName(QStringLiteral("RightB"));
        RightB->setGeometry(QRect(170, 250, 71, 71));
        LeftB = new QPushButton(RoboPult);
        LeftB->setObjectName(QStringLiteral("LeftB"));
        LeftB->setGeometry(QRect(10, 250, 71, 71));
        DownB = new QPushButton(RoboPult);
        DownB->setObjectName(QStringLiteral("DownB"));
        DownB->setGeometry(QRect(90, 330, 71, 71));
        RadB = new QPushButton(RoboPult);
        RadB->setObjectName(QStringLiteral("RadB"));
        RadB->setGeometry(QRect(10, 330, 71, 71));
        RadB->setFont(font1);
        SvobodnoB = new QPushButton(RoboPult);
        SvobodnoB->setObjectName(QStringLiteral("SvobodnoB"));
        SvobodnoB->setGeometry(QRect(170, 330, 71, 71));
        SvobodnoB->setFont(font1);
        SvobodnoB->setCheckable(true);
        ClearLog = new QToolButton(RoboPult);
        ClearLog->setObjectName(QStringLiteral("ClearLog"));
        ClearLog->setGeometry(QRect(210, 30, 31, 41));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/stop.png"), QSize(), QIcon::Normal, QIcon::On);
        ClearLog->setIcon(icon);
        CenterB = new QToolButton(RoboPult);
        CenterB->setObjectName(QStringLiteral("CenterB"));
        CenterB->setGeometry(QRect(90, 250, 71, 71));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/robo_field.png"), QSize(), QIcon::Normal, QIcon::On);
        CenterB->setIcon(icon1);
        CenterB->setIconSize(QSize(61, 61));
        ToKumir = new QToolButton(RoboPult);
        ToKumir->setObjectName(QStringLiteral("ToKumir"));
        ToKumir->setGeometry(QRect(210, 90, 31, 41));
        ToKumir->setIcon(icon);

        retranslateUi(RoboPult);

        QMetaObject::connectSlotsByName(RoboPult);
    } // setupUi

    void retranslateUi(QWidget *RoboPult)
    {
        RoboPult->setWindowTitle(QApplication::translate("RoboPult", "\320\237\321\203\320\273\321\214\321\202", 0));
        TempB->setText(QApplication::translate("RoboPult", "Temp", 0));
        StenaB->setText(QApplication::translate("RoboPult", "\320\241\321\202\320\265\320\275\320\260\n"
"\320\227\320\260\320\272\321\200", 0));
        UpB->setText(QApplication::translate("RoboPult", "Up", 0));
        RightB->setText(QApplication::translate("RoboPult", "Right", 0));
        LeftB->setText(QApplication::translate("RoboPult", "Left", 0));
        DownB->setText(QApplication::translate("RoboPult", "Down", 0));
        RadB->setText(QApplication::translate("RoboPult", "Rad", 0));
        SvobodnoB->setText(QApplication::translate("RoboPult", "\320\241\320\262\320\276\320\261\320\276\320\264\320\275\320\276\n"
"\320\247\320\270\321\201\321\202\320\276", 0));
#ifndef QT_NO_TOOLTIP
        ClearLog->setToolTip(QApplication::translate("RoboPult", "\320\241\320\261\321\200\320\276\321\201\320\270\321\202\321\214", 0));
#endif // QT_NO_TOOLTIP
        ClearLog->setText(QApplication::translate("RoboPult", "...", 0));
        CenterB->setText(QString());
#ifndef QT_NO_TOOLTIP
        ToKumir->setToolTip(QApplication::translate("RoboPult", "\320\241\320\272\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0));
#endif // QT_NO_TOOLTIP
        ToKumir->setText(QApplication::translate("RoboPult", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class RoboPult: public Ui_RoboPult {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PULT_H
