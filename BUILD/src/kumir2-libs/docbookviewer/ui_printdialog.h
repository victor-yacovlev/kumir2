/********************************************************************************
** Form generated from reading UI file 'printdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRINTDIALOG_H
#define UI_PRINTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>

namespace DocBookViewer {

class Ui_PrintDialog
{
public:
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QTreeWidget *itemsChooser;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QComboBox *paperSize;
    QLabel *pagesCountLabel;
    QPushButton *estimatePagesButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DocBookViewer__PrintDialog)
    {
        if (DocBookViewer__PrintDialog->objectName().isEmpty())
            DocBookViewer__PrintDialog->setObjectName(QStringLiteral("DocBookViewer__PrintDialog"));
        DocBookViewer__PrintDialog->resize(400, 434);
        gridLayout_2 = new QGridLayout(DocBookViewer__PrintDialog);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        groupBox = new QGroupBox(DocBookViewer__PrintDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        itemsChooser = new QTreeWidget(groupBox);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        itemsChooser->setHeaderItem(__qtreewidgetitem);
        itemsChooser->setObjectName(QStringLiteral("itemsChooser"));
        itemsChooser->header()->setVisible(false);

        gridLayout->addWidget(itemsChooser, 0, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_2 = new QGroupBox(DocBookViewer__PrintDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        paperSize = new QComboBox(groupBox_2);
        paperSize->setObjectName(QStringLiteral("paperSize"));

        gridLayout_3->addWidget(paperSize, 0, 1, 1, 1);

        pagesCountLabel = new QLabel(groupBox_2);
        pagesCountLabel->setObjectName(QStringLiteral("pagesCountLabel"));

        gridLayout_3->addWidget(pagesCountLabel, 1, 0, 1, 1);

        estimatePagesButton = new QPushButton(groupBox_2);
        estimatePagesButton->setObjectName(QStringLiteral("estimatePagesButton"));

        gridLayout_3->addWidget(estimatePagesButton, 1, 1, 1, 1);


        gridLayout_2->addWidget(groupBox_2, 1, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DocBookViewer__PrintDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_2->addWidget(buttonBox, 2, 0, 1, 1);

#ifndef QT_NO_SHORTCUT
        pagesCountLabel->setBuddy(estimatePagesButton);
#endif // QT_NO_SHORTCUT

        retranslateUi(DocBookViewer__PrintDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), DocBookViewer__PrintDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DocBookViewer__PrintDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DocBookViewer__PrintDialog);
    } // setupUi

    void retranslateUi(QDialog *DocBookViewer__PrintDialog)
    {
        DocBookViewer__PrintDialog->setWindowTitle(QApplication::translate("DocBookViewer::PrintDialog", "Dialog", 0));
        groupBox->setTitle(QApplication::translate("DocBookViewer::PrintDialog", "Items to print", 0));
        groupBox_2->setTitle(QApplication::translate("DocBookViewer::PrintDialog", "Pages options", 0));
        label->setText(QApplication::translate("DocBookViewer::PrintDialog", "Page size:", 0));
        paperSize->clear();
        paperSize->insertItems(0, QStringList()
         << QApplication::translate("DocBookViewer::PrintDialog", "A4", 0)
         << QApplication::translate("DocBookViewer::PrintDialog", "A5", 0)
        );
        pagesCountLabel->setText(QApplication::translate("DocBookViewer::PrintDialog", "Pages count: unknown", 0));
        estimatePagesButton->setText(QApplication::translate("DocBookViewer::PrintDialog", "Estimate pages count", 0));
    } // retranslateUi

};

} // namespace DocBookViewer

namespace DocBookViewer {
namespace Ui {
    class PrintDialog: public Ui_PrintDialog {};
} // namespace Ui
} // namespace DocBookViewer

#endif // UI_PRINTDIALOG_H
