/********************************************************************************
** Form generated from reading UI file 'painternewimagedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PAINTERNEWIMAGEDIALOG_H
#define UI_PAINTERNEWIMAGEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace ActorPainter {

class Ui_PainterNewImageDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButtonCustom;
    QRadioButton *radioButtonTemplate;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    QWidget *pageCustom;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label;
    QSpinBox *w;
    QLabel *label_2;
    QSpinBox *h;
    QLabel *label_3;
    QComboBox *colorBox;
    QSpacerItem *verticalSpacer;
    QWidget *pageTemplate;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_4;
    QScrollArea *scrollArea;
    QWidget *scrollable;
    QLabel *preview;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PainterNewImageDialog)
    {
        if (PainterNewImageDialog->objectName().isEmpty())
            PainterNewImageDialog->setObjectName(QStringLiteral("PainterNewImageDialog"));
        PainterNewImageDialog->resize(683, 402);
        verticalLayout_3 = new QVBoxLayout(PainterNewImageDialog);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        frame = new QFrame(PainterNewImageDialog);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Sunken);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        radioButtonCustom = new QRadioButton(frame);
        radioButtonCustom->setObjectName(QStringLiteral("radioButtonCustom"));
        radioButtonCustom->setChecked(true);

        verticalLayout->addWidget(radioButtonCustom);

        radioButtonTemplate = new QRadioButton(frame);
        radioButtonTemplate->setObjectName(QStringLiteral("radioButtonTemplate"));

        verticalLayout->addWidget(radioButtonTemplate);

        listWidget = new QListWidget(frame);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        verticalLayout->addWidget(listWidget);


        horizontalLayout_2->addWidget(frame);

        stackedWidget = new QStackedWidget(PainterNewImageDialog);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setMinimumSize(QSize(240, 0));
        stackedWidget->setFrameShape(QFrame::StyledPanel);
        stackedWidget->setFrameShadow(QFrame::Sunken);
        pageCustom = new QWidget();
        pageCustom->setObjectName(QStringLiteral("pageCustom"));
        verticalLayout_2 = new QVBoxLayout(pageCustom);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        widget = new QWidget(pageCustom);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        widget->setMinimumSize(QSize(240, 1));
        widget->setMaximumSize(QSize(240, 16777215));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        w = new QSpinBox(widget);
        w->setObjectName(QStringLiteral("w"));
        w->setMinimum(1);
        w->setMaximum(3000);
        w->setValue(640);

        gridLayout->addWidget(w, 0, 1, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        h = new QSpinBox(widget);
        h->setObjectName(QStringLiteral("h"));
        h->setMinimum(1);
        h->setMaximum(3000);
        h->setValue(480);

        gridLayout->addWidget(h, 1, 1, 1, 1);

        label_3 = new QLabel(widget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        colorBox = new QComboBox(widget);
        colorBox->setObjectName(QStringLiteral("colorBox"));

        gridLayout->addWidget(colorBox, 2, 1, 1, 1);


        verticalLayout_2->addWidget(widget);

        verticalSpacer = new QSpacerItem(20, 201, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        stackedWidget->addWidget(pageCustom);
        pageTemplate = new QWidget();
        pageTemplate->setObjectName(QStringLiteral("pageTemplate"));
        verticalLayout_4 = new QVBoxLayout(pageTemplate);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_4 = new QLabel(pageTemplate);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_4->addWidget(label_4);

        scrollArea = new QScrollArea(pageTemplate);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setWidgetResizable(true);
        scrollable = new QWidget();
        scrollable->setObjectName(QStringLiteral("scrollable"));
        scrollable->setGeometry(QRect(0, 0, 284, 280));
        preview = new QLabel(scrollable);
        preview->setObjectName(QStringLiteral("preview"));
        preview->setGeometry(QRect(30, 56, 240, 151));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(preview->sizePolicy().hasHeightForWidth());
        preview->setSizePolicy(sizePolicy1);
        preview->setScaledContents(true);
        scrollArea->setWidget(scrollable);

        verticalLayout_4->addWidget(scrollArea);

        stackedWidget->addWidget(pageTemplate);

        horizontalLayout_2->addWidget(stackedWidget);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonBox = new QDialogButtonBox(PainterNewImageDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout_3->addLayout(horizontalLayout);


        retranslateUi(PainterNewImageDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), PainterNewImageDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PainterNewImageDialog, SLOT(reject()));
        QObject::connect(radioButtonTemplate, SIGNAL(toggled(bool)), listWidget, SLOT(setEnabled(bool)));

        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(PainterNewImageDialog);
    } // setupUi

    void retranslateUi(QDialog *PainterNewImageDialog)
    {
        PainterNewImageDialog->setWindowTitle(QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\235\320\276\320\262\321\213\320\271 \320\273\320\270\321\201\321\202...", 0));
        radioButtonCustom->setText(QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\241 \320\277\320\260\321\200\320\260\320\274\320\265\321\202\321\200\320\260\320\274\320\270", 0));
        radioButtonTemplate->setText(QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\230\320\267 \321\210\320\260\320\261\320\273\320\276\320\275\320\260", 0));
        label->setText(QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\250\320\270\321\200\320\270\320\275\320\260:", 0));
        label_2->setText(QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\222\321\213\321\201\320\276\321\202\320\260:", 0));
        label_3->setText(QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260:", 0));
        colorBox->clear();
        colorBox->insertItems(0, QStringList()
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\261\320\265\320\273\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\321\207\320\265\321\200\320\275\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\321\201\320\265\321\200\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\321\204\320\270\320\276\320\273\320\265\321\202\320\276\320\262\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\321\201\320\270\320\275\320\270\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\263\320\276\320\273\321\203\320\261\320\276\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\267\320\265\320\273\320\265\320\275\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\266\320\265\320\273\321\202\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\276\321\200\320\260\320\275\320\266\320\265\320\262\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\272\321\200\320\260\321\201\320\275\321\213\320\271", 0)
         << QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\264\321\200\321\203\320\263\320\276\320\271...", 0)
        );
        label_4->setText(QApplication::translate("ActorPainter::PainterNewImageDialog", "\320\237\321\200\320\265\320\264\320\262\320\260\321\200\320\270\321\202\320\265\320\273\321\214\320\275\321\213\320\271 \320\277\321\200\320\276\321\201\320\274\320\276\321\202\321\200 \321\210\320\260\320\261\320\273\320\276\320\275\320\260:", 0));
        preview->setText(QString());
    } // retranslateUi

};

} // namespace ActorPainter

namespace ActorPainter {
namespace Ui {
    class PainterNewImageDialog: public Ui_PainterNewImageDialog {};
} // namespace Ui
} // namespace ActorPainter

#endif // UI_PAINTERNEWIMAGEDIALOG_H
