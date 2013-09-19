/********************************************************************************
** Form generated from reading UI file 'pult.ui'
**
** Created: Fri Mar 2 16:59:05 2012
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PULT_H
#define UI_PULT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VodoleyPult
{
public:
    QPushButton *TempB;
    QPushButton *UpB;
    QPushButton *AoutB;
    QPushButton *AtoCb;
    QPushButton *DownB;
    QPushButton *AtoBb;
    QToolButton *ClearLog;
    QFrame *frame;
    QLabel *label;
    QToolButton *toKumir;
    QPushButton *CtoAb;
    QPushButton *CtoBb;
    QPushButton *CoutB;
    QPushButton *FillA;
    QPushButton *FillB;
    QPushButton *FillC;

    void setupUi(QWidget *VodoleyPult)
    {
        if (VodoleyPult->objectName().isEmpty())
            VodoleyPult->setObjectName(QString::fromUtf8("VodoleyPult"));
        VodoleyPult->resize(254, 434);
        VodoleyPult->setMinimumSize(QSize(254, 434));
        VodoleyPult->setMaximumSize(QSize(254, 434));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(41, 144, 144, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(61, 216, 216, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(51, 180, 180, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(20, 72, 72, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(27, 96, 96, 255));
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
        QBrush brush7(QColor(148, 199, 199, 255));
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
        VodoleyPult->setPalette(palette);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        VodoleyPult->setFont(font);
        TempB = new QPushButton(VodoleyPult);
        TempB->setObjectName(QString::fromUtf8("TempB"));
        TempB->setGeometry(QRect(15, 350, 55, 55));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        TempB->setFont(font1);
        UpB = new QPushButton(VodoleyPult);
        UpB->setObjectName(QString::fromUtf8("UpB"));
        UpB->setGeometry(QRect(100, 290, 55, 55));
        AoutB = new QPushButton(VodoleyPult);
        AoutB->setObjectName(QString::fromUtf8("AoutB"));
        AoutB->setGeometry(QRect(15, 220, 55, 55));
        AtoCb = new QPushButton(VodoleyPult);
        AtoCb->setObjectName(QString::fromUtf8("AtoCb"));
        AtoCb->setGeometry(QRect(180, 350, 55, 55));
        DownB = new QPushButton(VodoleyPult);
        DownB->setObjectName(QString::fromUtf8("DownB"));
        DownB->setGeometry(QRect(100, 220, 55, 55));
        AtoBb = new QPushButton(VodoleyPult);
        AtoBb->setObjectName(QString::fromUtf8("AtoBb"));
        AtoBb->setGeometry(QRect(15, 290, 55, 55));
        AtoBb->setFont(font1);
        ClearLog = new QToolButton(VodoleyPult);
        ClearLog->setObjectName(QString::fromUtf8("ClearLog"));
        ClearLog->setGeometry(QRect(210, 30, 31, 45));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush9(QColor(42, 144, 144, 255));
        brush9.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Button, brush9);
        QBrush brush10(QColor(63, 216, 216, 255));
        brush10.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Light, brush10);
        QBrush brush11(QColor(52, 180, 180, 255));
        brush11.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Midlight, brush11);
        QBrush brush12(QColor(21, 72, 72, 255));
        brush12.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Dark, brush12);
        QBrush brush13(QColor(28, 96, 96, 255));
        brush13.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Mid, brush13);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush);
        palette1.setBrush(QPalette::Active, QPalette::BrightText, brush6);
        palette1.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette1.setBrush(QPalette::Active, QPalette::Window, brush9);
        palette1.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Active, QPalette::AlternateBase, brush7);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipBase, brush8);
        palette1.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Button, brush9);
        palette1.setBrush(QPalette::Inactive, QPalette::Light, brush10);
        palette1.setBrush(QPalette::Inactive, QPalette::Midlight, brush11);
        palette1.setBrush(QPalette::Inactive, QPalette::Dark, brush12);
        palette1.setBrush(QPalette::Inactive, QPalette::Mid, brush13);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::BrightText, brush6);
        palette1.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette1.setBrush(QPalette::Inactive, QPalette::Window, brush9);
        palette1.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush7);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush8);
        palette1.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Button, brush9);
        palette1.setBrush(QPalette::Disabled, QPalette::Light, brush10);
        palette1.setBrush(QPalette::Disabled, QPalette::Midlight, brush11);
        palette1.setBrush(QPalette::Disabled, QPalette::Dark, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Mid, brush13);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::BrightText, brush6);
        palette1.setBrush(QPalette::Disabled, QPalette::ButtonText, brush12);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush9);
        palette1.setBrush(QPalette::Disabled, QPalette::Window, brush9);
        palette1.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette1.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush9);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush8);
        palette1.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        ClearLog->setPalette(palette1);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        ClearLog->setIcon(icon);
        frame = new QFrame(VodoleyPult);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 410, 231, 21));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        label = new QLabel(frame);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 2, 211, 20));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(3);
        sizePolicy.setVerticalStretch(60);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        toKumir = new QToolButton(VodoleyPult);
        toKumir->setObjectName(QString::fromUtf8("toKumir"));
        toKumir->setEnabled(false);
        toKumir->setGeometry(QRect(210, 85, 31, 45));
        QPalette palette2;
        palette2.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Button, brush9);
        palette2.setBrush(QPalette::Active, QPalette::Light, brush10);
        palette2.setBrush(QPalette::Active, QPalette::Midlight, brush11);
        palette2.setBrush(QPalette::Active, QPalette::Dark, brush12);
        palette2.setBrush(QPalette::Active, QPalette::Mid, brush13);
        palette2.setBrush(QPalette::Active, QPalette::Text, brush);
        palette2.setBrush(QPalette::Active, QPalette::BrightText, brush6);
        palette2.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette2.setBrush(QPalette::Active, QPalette::Window, brush9);
        palette2.setBrush(QPalette::Active, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Active, QPalette::AlternateBase, brush7);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipBase, brush8);
        palette2.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Button, brush9);
        palette2.setBrush(QPalette::Inactive, QPalette::Light, brush10);
        palette2.setBrush(QPalette::Inactive, QPalette::Midlight, brush11);
        palette2.setBrush(QPalette::Inactive, QPalette::Dark, brush12);
        palette2.setBrush(QPalette::Inactive, QPalette::Mid, brush13);
        palette2.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::BrightText, brush6);
        palette2.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette2.setBrush(QPalette::Inactive, QPalette::Window, brush9);
        palette2.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush7);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush8);
        palette2.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::WindowText, brush12);
        palette2.setBrush(QPalette::Disabled, QPalette::Button, brush9);
        palette2.setBrush(QPalette::Disabled, QPalette::Light, brush10);
        palette2.setBrush(QPalette::Disabled, QPalette::Midlight, brush11);
        palette2.setBrush(QPalette::Disabled, QPalette::Dark, brush12);
        palette2.setBrush(QPalette::Disabled, QPalette::Mid, brush13);
        palette2.setBrush(QPalette::Disabled, QPalette::Text, brush12);
        palette2.setBrush(QPalette::Disabled, QPalette::BrightText, brush6);
        palette2.setBrush(QPalette::Disabled, QPalette::ButtonText, brush12);
        palette2.setBrush(QPalette::Disabled, QPalette::Base, brush9);
        palette2.setBrush(QPalette::Disabled, QPalette::Window, brush9);
        palette2.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette2.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush9);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush8);
        palette2.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        toKumir->setPalette(palette2);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/img/kumir.png"), QSize(), QIcon::Normal, QIcon::Off);
        toKumir->setIcon(icon1);
        CtoAb = new QPushButton(VodoleyPult);
        CtoAb->setObjectName(QString::fromUtf8("CtoAb"));
        CtoAb->setGeometry(QRect(180, 290, 55, 55));
        CtoAb->setFont(font1);
        CtoBb = new QPushButton(VodoleyPult);
        CtoBb->setObjectName(QString::fromUtf8("CtoBb"));
        CtoBb->setGeometry(QRect(100, 350, 55, 55));
        CtoBb->setFont(font1);
        CoutB = new QPushButton(VodoleyPult);
        CoutB->setObjectName(QString::fromUtf8("CoutB"));
        CoutB->setGeometry(QRect(180, 220, 55, 55));
        CoutB->setFont(font1);
        FillA = new QPushButton(VodoleyPult);
        FillA->setObjectName(QString::fromUtf8("FillA"));
        FillA->setGeometry(QRect(15, 160, 55, 55));
        FillA->setFont(font1);
        FillB = new QPushButton(VodoleyPult);
        FillB->setObjectName(QString::fromUtf8("FillB"));
        FillB->setGeometry(QRect(100, 160, 55, 55));
        FillB->setFont(font1);
        FillC = new QPushButton(VodoleyPult);
        FillC->setObjectName(QString::fromUtf8("FillC"));
        FillC->setGeometry(QRect(180, 160, 55, 55));
        FillC->setFont(font1);

        retranslateUi(VodoleyPult);

        QMetaObject::connectSlotsByName(VodoleyPult);
    } // setupUi

    void retranslateUi(QWidget *VodoleyPult)
    {
        VodoleyPult->setWindowTitle(QApplication::translate("VodoleyPult", "\320\237\321\203\320\273\321\214\321\202", 0, QApplication::UnicodeUTF8));
        TempB->setText(QApplication::translate("VodoleyPult", "B->A", 0, QApplication::UnicodeUTF8));
        UpB->setText(QApplication::translate("VodoleyPult", "B->C", 0, QApplication::UnicodeUTF8));
        AoutB->setText(QApplication::translate("VodoleyPult", "Aout", 0, QApplication::UnicodeUTF8));
        AtoCb->setText(QApplication::translate("VodoleyPult", "A->C", 0, QApplication::UnicodeUTF8));
        DownB->setText(QApplication::translate("VodoleyPult", "Bout", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        AtoBb->setToolTip(QApplication::translate("VodoleyPult", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:600; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\320\237\321\200\320\265\320\273\320\265\320\271 \320\270\320\267 \320\260 \320\262 \320\261</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        AtoBb->setText(QApplication::translate("VodoleyPult", "A->B", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ClearLog->setToolTip(QApplication::translate("VodoleyPult", "\320\222\320\265\321\200\320\275\321\203\321\202\321\214\321\201\321\217 \320\262 \320\275\320\260\321\207\320\260\320\273\321\214\320\275\320\276\320\265 \321\201\320\276\321\201\321\202\320\276\321\217\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ClearLog->setText(QApplication::translate("VodoleyPult", "...", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
#ifndef QT_NO_TOOLTIP
        toKumir->setToolTip(QApplication::translate("VodoleyPult", "\320\237\320\265\321\200\320\265\320\264\320\260\321\202\321\214 \321\202\320\265\320\272\321\201\321\202 \320\262 \320\272\321\203\320\274\320\270\321\200", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toKumir->setText(QApplication::translate("VodoleyPult", "...", 0, QApplication::UnicodeUTF8));
        CtoAb->setText(QApplication::translate("VodoleyPult", "C->A", 0, QApplication::UnicodeUTF8));
        CtoBb->setText(QApplication::translate("VodoleyPult", "C->B", 0, QApplication::UnicodeUTF8));
        CoutB->setText(QApplication::translate("VodoleyPult", "Cout", 0, QApplication::UnicodeUTF8));
        FillA->setText(QApplication::translate("VodoleyPult", "Fa", 0, QApplication::UnicodeUTF8));
        FillB->setText(QApplication::translate("VodoleyPult", "Fb", 0, QApplication::UnicodeUTF8));
        FillC->setText(QApplication::translate("VodoleyPult", "Fc", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class VodoleyPult: public Ui_VodoleyPult {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PULT_H
