/********************************************************************************
** Form generated from reading UI file 'painterwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAINTERWINDOW_H
#define UI_PAINTERWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <painterruler.h>
#include <painterview.h>
#include <scrollarea.h>

namespace ActorPainter {

class Ui_PainterWindow
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    PainterRuler *horizontalRuler;
    PainterRuler *verticalRuler;
    ActorPainter::ScrollArea *scrollArea;
    PainterView *view;
    QHBoxLayout *horizontalLayout;
    QLabel *statusBar;
    QLabel *zoomLabel;

    void setupUi(QWidget *ActorPainter__PainterWindow)
    {
        if (ActorPainter__PainterWindow->objectName().isEmpty())
            ActorPainter__PainterWindow->setObjectName(QStringLiteral("ActorPainter__PainterWindow"));
        ActorPainter__PainterWindow->resize(681, 555);
        verticalLayout = new QVBoxLayout(ActorPainter__PainterWindow);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
#ifndef Q_OS_MAC
        gridLayout->setContentsMargins(0, 0, 0, 0);
#endif
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalRuler = new PainterRuler(ActorPainter__PainterWindow);
        horizontalRuler->setObjectName(QStringLiteral("horizontalRuler"));
        horizontalRuler->setMinimumSize(QSize(0, 24));

        gridLayout->addWidget(horizontalRuler, 0, 1, 1, 1);

        verticalRuler = new PainterRuler(ActorPainter__PainterWindow);
        verticalRuler->setObjectName(QStringLiteral("verticalRuler"));
        verticalRuler->setMinimumSize(QSize(24, 0));

        gridLayout->addWidget(verticalRuler, 1, 0, 1, 1);

        scrollArea = new ActorPainter::ScrollArea(ActorPainter__PainterWindow);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollArea->setAlignment(Qt::AlignCenter);
        view = new PainterView();
        view->setObjectName(QStringLiteral("view"));
        view->setGeometry(QRect(0, 0, 641, 488));
        scrollArea->setWidget(view);

        gridLayout->addWidget(scrollArea, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        statusBar = new QLabel(ActorPainter__PainterWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        statusBar->setFrameShape(QFrame::StyledPanel);

        horizontalLayout->addWidget(statusBar);

        zoomLabel = new QLabel(ActorPainter__PainterWindow);
        zoomLabel->setObjectName(QStringLiteral("zoomLabel"));
        zoomLabel->setMaximumSize(QSize(50, 16777215));
        zoomLabel->setFrameShape(QFrame::StyledPanel);

        horizontalLayout->addWidget(zoomLabel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ActorPainter__PainterWindow);

        QMetaObject::connectSlotsByName(ActorPainter__PainterWindow);
    } // setupUi

    void retranslateUi(QWidget *ActorPainter__PainterWindow)
    {
        ActorPainter__PainterWindow->setWindowTitle(QApplication::translate("ActorPainter::PainterWindow", "MainWindow", 0));
        statusBar->setText(QString());
        zoomLabel->setText(QApplication::translate("ActorPainter::PainterWindow", "100%", 0));
    } // retranslateUi

};

} // namespace ActorPainter

namespace ActorPainter {
namespace Ui {
    class PainterWindow: public Ui_PainterWindow {};
} // namespace Ui
} // namespace ActorPainter

#endif // UI_PAINTERWINDOW_H
