/********************************************************************************
** Form generated from reading UI file 'macroeditor.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MACROEDITOR_H
#define UI_MACROEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

namespace Editor {

class Ui_MacroEditor
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QTableWidget *tableWidget;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btnSave;
    QPushButton *btnCancel;

    void setupUi(QDialog *Editor__MacroEditor)
    {
        if (Editor__MacroEditor->objectName().isEmpty())
            Editor__MacroEditor->setObjectName(QStringLiteral("Editor__MacroEditor"));
        Editor__MacroEditor->resize(495, 306);
        verticalLayout_2 = new QVBoxLayout(Editor__MacroEditor);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(Editor__MacroEditor);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit = new QLineEdit(Editor__MacroEditor);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout_2->addLayout(horizontalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(Editor__MacroEditor);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        tableWidget = new QTableWidget(Editor__MacroEditor);
        if (tableWidget->columnCount() < 13)
            tableWidget->setColumnCount(13);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(11, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(12, __qtablewidgetitem12);
        if (tableWidget->rowCount() < 5)
            tableWidget->setRowCount(5);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(0, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(1, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(2, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(3, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tableWidget->setVerticalHeaderItem(4, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        __qtablewidgetitem18->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 0, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        __qtablewidgetitem19->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 1, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        __qtablewidgetitem20->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 2, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        __qtablewidgetitem21->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 3, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        __qtablewidgetitem22->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 4, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        __qtablewidgetitem23->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 5, __qtablewidgetitem23);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        __qtablewidgetitem24->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 6, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        __qtablewidgetitem25->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 7, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        __qtablewidgetitem26->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 8, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        __qtablewidgetitem27->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 9, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        __qtablewidgetitem28->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(0, 10, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        tableWidget->setItem(0, 11, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        tableWidget->setItem(0, 12, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        tableWidget->setItem(1, 0, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        __qtablewidgetitem32->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 1, __qtablewidgetitem32);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        __qtablewidgetitem33->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 2, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        __qtablewidgetitem34->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 3, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        __qtablewidgetitem35->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 4, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        __qtablewidgetitem36->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 5, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        __qtablewidgetitem37->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 6, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        __qtablewidgetitem38->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 7, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        __qtablewidgetitem39->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 8, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        __qtablewidgetitem40->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 9, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        __qtablewidgetitem41->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 10, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        __qtablewidgetitem42->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 11, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        __qtablewidgetitem43->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(1, 12, __qtablewidgetitem43);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        tableWidget->setItem(2, 0, __qtablewidgetitem44);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        __qtablewidgetitem45->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 1, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        __qtablewidgetitem46->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 2, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        __qtablewidgetitem47->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 3, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        __qtablewidgetitem48->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 4, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        __qtablewidgetitem49->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 5, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        __qtablewidgetitem50->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 6, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        __qtablewidgetitem51->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 7, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        __qtablewidgetitem52->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 8, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        __qtablewidgetitem53->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 9, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        __qtablewidgetitem54->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 10, __qtablewidgetitem54);
        QTableWidgetItem *__qtablewidgetitem55 = new QTableWidgetItem();
        __qtablewidgetitem55->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(2, 11, __qtablewidgetitem55);
        QTableWidgetItem *__qtablewidgetitem56 = new QTableWidgetItem();
        tableWidget->setItem(2, 12, __qtablewidgetitem56);
        QTableWidgetItem *__qtablewidgetitem57 = new QTableWidgetItem();
        __qtablewidgetitem57->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 1, __qtablewidgetitem57);
        QTableWidgetItem *__qtablewidgetitem58 = new QTableWidgetItem();
        __qtablewidgetitem58->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 2, __qtablewidgetitem58);
        QTableWidgetItem *__qtablewidgetitem59 = new QTableWidgetItem();
        __qtablewidgetitem59->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 3, __qtablewidgetitem59);
        QTableWidgetItem *__qtablewidgetitem60 = new QTableWidgetItem();
        __qtablewidgetitem60->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 4, __qtablewidgetitem60);
        QTableWidgetItem *__qtablewidgetitem61 = new QTableWidgetItem();
        __qtablewidgetitem61->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 5, __qtablewidgetitem61);
        QTableWidgetItem *__qtablewidgetitem62 = new QTableWidgetItem();
        __qtablewidgetitem62->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 6, __qtablewidgetitem62);
        QTableWidgetItem *__qtablewidgetitem63 = new QTableWidgetItem();
        __qtablewidgetitem63->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 7, __qtablewidgetitem63);
        QTableWidgetItem *__qtablewidgetitem64 = new QTableWidgetItem();
        __qtablewidgetitem64->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 8, __qtablewidgetitem64);
        QTableWidgetItem *__qtablewidgetitem65 = new QTableWidgetItem();
        __qtablewidgetitem65->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tableWidget->setItem(3, 9, __qtablewidgetitem65);
        QTableWidgetItem *__qtablewidgetitem66 = new QTableWidgetItem();
        tableWidget->setItem(3, 10, __qtablewidgetitem66);
        QTableWidgetItem *__qtablewidgetitem67 = new QTableWidgetItem();
        tableWidget->setItem(3, 11, __qtablewidgetitem67);
        QTableWidgetItem *__qtablewidgetitem68 = new QTableWidgetItem();
        tableWidget->setItem(3, 12, __qtablewidgetitem68);
        QFont font;
        font.setItalic(true);
        QTableWidgetItem *__qtablewidgetitem69 = new QTableWidgetItem();
        __qtablewidgetitem69->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        __qtablewidgetitem69->setFont(font);
        tableWidget->setItem(4, 0, __qtablewidgetitem69);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tableWidget->sizePolicy().hasHeightForWidth());
        tableWidget->setSizePolicy(sizePolicy);
        tableWidget->setMinimumSize(QSize(461, 190));
        tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setShowGrid(true);
        tableWidget->setGridStyle(Qt::DashDotLine);
        tableWidget->setWordWrap(false);
        tableWidget->horizontalHeader()->setVisible(false);
        tableWidget->horizontalHeader()->setDefaultSectionSize(35);
        tableWidget->horizontalHeader()->setMinimumSectionSize(35);
        tableWidget->verticalHeader()->setVisible(false);
        tableWidget->verticalHeader()->setDefaultSectionSize(35);
        tableWidget->verticalHeader()->setHighlightSections(true);
        tableWidget->verticalHeader()->setMinimumSectionSize(35);

        horizontalLayout->addWidget(tableWidget);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        verticalLayout_2->addLayout(verticalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        btnSave = new QPushButton(Editor__MacroEditor);
        btnSave->setObjectName(QStringLiteral("btnSave"));

        horizontalLayout_3->addWidget(btnSave);

        btnCancel = new QPushButton(Editor__MacroEditor);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));

        horizontalLayout_3->addWidget(btnCancel);


        verticalLayout_2->addLayout(horizontalLayout_3);

#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(lineEdit);
        label->setBuddy(tableWidget);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(lineEdit, tableWidget);
        QWidget::setTabOrder(tableWidget, btnSave);
        QWidget::setTabOrder(btnSave, btnCancel);

        retranslateUi(Editor__MacroEditor);
        QObject::connect(btnCancel, SIGNAL(clicked()), Editor__MacroEditor, SLOT(reject()));
        QObject::connect(btnSave, SIGNAL(clicked()), Editor__MacroEditor, SLOT(accept()));

        QMetaObject::connectSlotsByName(Editor__MacroEditor);
    } // setupUi

    void retranslateUi(QDialog *Editor__MacroEditor)
    {
        Editor__MacroEditor->setWindowTitle(QApplication::translate("Editor::MacroEditor", "Form", 0));
        label_2->setText(QApplication::translate("Editor::MacroEditor", "Title:", 0));
        lineEdit->setPlaceholderText(QApplication::translate("Editor::MacroEditor", "Type sequence menu title", 0));
        label->setText(QApplication::translate("Editor::MacroEditor", "Assigned key:", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidget->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem9 = tableWidget->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem10 = tableWidget->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem11 = tableWidget->horizontalHeaderItem(11);
        ___qtablewidgetitem11->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem12 = tableWidget->horizontalHeaderItem(12);
        ___qtablewidgetitem12->setText(QApplication::translate("Editor::MacroEditor", "New Column", 0));
        QTableWidgetItem *___qtablewidgetitem13 = tableWidget->verticalHeaderItem(0);
        ___qtablewidgetitem13->setText(QApplication::translate("Editor::MacroEditor", "New Row", 0));
        QTableWidgetItem *___qtablewidgetitem14 = tableWidget->verticalHeaderItem(1);
        ___qtablewidgetitem14->setText(QApplication::translate("Editor::MacroEditor", "New Row", 0));
        QTableWidgetItem *___qtablewidgetitem15 = tableWidget->verticalHeaderItem(2);
        ___qtablewidgetitem15->setText(QApplication::translate("Editor::MacroEditor", "New Row", 0));
        QTableWidgetItem *___qtablewidgetitem16 = tableWidget->verticalHeaderItem(3);
        ___qtablewidgetitem16->setText(QApplication::translate("Editor::MacroEditor", "New Row", 0));
        QTableWidgetItem *___qtablewidgetitem17 = tableWidget->verticalHeaderItem(4);
        ___qtablewidgetitem17->setText(QApplication::translate("Editor::MacroEditor", "New Row", 0));

        const bool __sortingEnabled = tableWidget->isSortingEnabled();
        tableWidget->setSortingEnabled(false);
        QTableWidgetItem *___qtablewidgetitem18 = tableWidget->item(0, 0);
        ___qtablewidgetitem18->setText(QApplication::translate("Editor::MacroEditor", "`", 0));
        QTableWidgetItem *___qtablewidgetitem19 = tableWidget->item(0, 1);
        ___qtablewidgetitem19->setText(QApplication::translate("Editor::MacroEditor", "1", 0));
        QTableWidgetItem *___qtablewidgetitem20 = tableWidget->item(0, 2);
        ___qtablewidgetitem20->setText(QApplication::translate("Editor::MacroEditor", "2", 0));
        QTableWidgetItem *___qtablewidgetitem21 = tableWidget->item(0, 3);
        ___qtablewidgetitem21->setText(QApplication::translate("Editor::MacroEditor", "3", 0));
        QTableWidgetItem *___qtablewidgetitem22 = tableWidget->item(0, 4);
        ___qtablewidgetitem22->setText(QApplication::translate("Editor::MacroEditor", "4", 0));
        QTableWidgetItem *___qtablewidgetitem23 = tableWidget->item(0, 5);
        ___qtablewidgetitem23->setText(QApplication::translate("Editor::MacroEditor", "5", 0));
        QTableWidgetItem *___qtablewidgetitem24 = tableWidget->item(0, 6);
        ___qtablewidgetitem24->setText(QApplication::translate("Editor::MacroEditor", "6", 0));
        QTableWidgetItem *___qtablewidgetitem25 = tableWidget->item(0, 7);
        ___qtablewidgetitem25->setText(QApplication::translate("Editor::MacroEditor", "7", 0));
        QTableWidgetItem *___qtablewidgetitem26 = tableWidget->item(0, 8);
        ___qtablewidgetitem26->setText(QApplication::translate("Editor::MacroEditor", "8", 0));
        QTableWidgetItem *___qtablewidgetitem27 = tableWidget->item(0, 9);
        ___qtablewidgetitem27->setText(QApplication::translate("Editor::MacroEditor", "9", 0));
        QTableWidgetItem *___qtablewidgetitem28 = tableWidget->item(0, 10);
        ___qtablewidgetitem28->setText(QApplication::translate("Editor::MacroEditor", "0", 0));
        QTableWidgetItem *___qtablewidgetitem29 = tableWidget->item(1, 1);
        ___qtablewidgetitem29->setText(QApplication::translate("Editor::MacroEditor", "Q", 0));
        QTableWidgetItem *___qtablewidgetitem30 = tableWidget->item(1, 2);
        ___qtablewidgetitem30->setText(QApplication::translate("Editor::MacroEditor", "W", 0));
        QTableWidgetItem *___qtablewidgetitem31 = tableWidget->item(1, 3);
        ___qtablewidgetitem31->setText(QApplication::translate("Editor::MacroEditor", "E", 0));
        QTableWidgetItem *___qtablewidgetitem32 = tableWidget->item(1, 4);
        ___qtablewidgetitem32->setText(QApplication::translate("Editor::MacroEditor", "R", 0));
        QTableWidgetItem *___qtablewidgetitem33 = tableWidget->item(1, 5);
        ___qtablewidgetitem33->setText(QApplication::translate("Editor::MacroEditor", "T", 0));
        QTableWidgetItem *___qtablewidgetitem34 = tableWidget->item(1, 6);
        ___qtablewidgetitem34->setText(QApplication::translate("Editor::MacroEditor", "Y", 0));
        QTableWidgetItem *___qtablewidgetitem35 = tableWidget->item(1, 7);
        ___qtablewidgetitem35->setText(QApplication::translate("Editor::MacroEditor", "U", 0));
        QTableWidgetItem *___qtablewidgetitem36 = tableWidget->item(1, 8);
        ___qtablewidgetitem36->setText(QApplication::translate("Editor::MacroEditor", "I", 0));
        QTableWidgetItem *___qtablewidgetitem37 = tableWidget->item(1, 9);
        ___qtablewidgetitem37->setText(QApplication::translate("Editor::MacroEditor", "O", 0));
        QTableWidgetItem *___qtablewidgetitem38 = tableWidget->item(1, 10);
        ___qtablewidgetitem38->setText(QApplication::translate("Editor::MacroEditor", "P", 0));
        QTableWidgetItem *___qtablewidgetitem39 = tableWidget->item(1, 11);
        ___qtablewidgetitem39->setText(QApplication::translate("Editor::MacroEditor", "[", 0));
        QTableWidgetItem *___qtablewidgetitem40 = tableWidget->item(1, 12);
        ___qtablewidgetitem40->setText(QApplication::translate("Editor::MacroEditor", "]", 0));
        QTableWidgetItem *___qtablewidgetitem41 = tableWidget->item(2, 1);
        ___qtablewidgetitem41->setText(QApplication::translate("Editor::MacroEditor", "A", 0));
        QTableWidgetItem *___qtablewidgetitem42 = tableWidget->item(2, 2);
        ___qtablewidgetitem42->setText(QApplication::translate("Editor::MacroEditor", "S", 0));
        QTableWidgetItem *___qtablewidgetitem43 = tableWidget->item(2, 3);
        ___qtablewidgetitem43->setText(QApplication::translate("Editor::MacroEditor", "D", 0));
        QTableWidgetItem *___qtablewidgetitem44 = tableWidget->item(2, 4);
        ___qtablewidgetitem44->setText(QApplication::translate("Editor::MacroEditor", "F", 0));
        QTableWidgetItem *___qtablewidgetitem45 = tableWidget->item(2, 5);
        ___qtablewidgetitem45->setText(QApplication::translate("Editor::MacroEditor", "G", 0));
        QTableWidgetItem *___qtablewidgetitem46 = tableWidget->item(2, 6);
        ___qtablewidgetitem46->setText(QApplication::translate("Editor::MacroEditor", "H", 0));
        QTableWidgetItem *___qtablewidgetitem47 = tableWidget->item(2, 7);
        ___qtablewidgetitem47->setText(QApplication::translate("Editor::MacroEditor", "J", 0));
        QTableWidgetItem *___qtablewidgetitem48 = tableWidget->item(2, 8);
        ___qtablewidgetitem48->setText(QApplication::translate("Editor::MacroEditor", "K", 0));
        QTableWidgetItem *___qtablewidgetitem49 = tableWidget->item(2, 9);
        ___qtablewidgetitem49->setText(QApplication::translate("Editor::MacroEditor", "L", 0));
        QTableWidgetItem *___qtablewidgetitem50 = tableWidget->item(2, 10);
        ___qtablewidgetitem50->setText(QApplication::translate("Editor::MacroEditor", ";", 0));
        QTableWidgetItem *___qtablewidgetitem51 = tableWidget->item(2, 11);
        ___qtablewidgetitem51->setText(QApplication::translate("Editor::MacroEditor", "'", 0));
        QTableWidgetItem *___qtablewidgetitem52 = tableWidget->item(3, 1);
        ___qtablewidgetitem52->setText(QApplication::translate("Editor::MacroEditor", "Z", 0));
        QTableWidgetItem *___qtablewidgetitem53 = tableWidget->item(3, 2);
        ___qtablewidgetitem53->setText(QApplication::translate("Editor::MacroEditor", "X", 0));
        QTableWidgetItem *___qtablewidgetitem54 = tableWidget->item(3, 3);
        ___qtablewidgetitem54->setText(QApplication::translate("Editor::MacroEditor", "C", 0));
        QTableWidgetItem *___qtablewidgetitem55 = tableWidget->item(3, 4);
        ___qtablewidgetitem55->setText(QApplication::translate("Editor::MacroEditor", "V", 0));
        QTableWidgetItem *___qtablewidgetitem56 = tableWidget->item(3, 5);
        ___qtablewidgetitem56->setText(QApplication::translate("Editor::MacroEditor", "B", 0));
        QTableWidgetItem *___qtablewidgetitem57 = tableWidget->item(3, 6);
        ___qtablewidgetitem57->setText(QApplication::translate("Editor::MacroEditor", "N", 0));
        QTableWidgetItem *___qtablewidgetitem58 = tableWidget->item(3, 7);
        ___qtablewidgetitem58->setText(QApplication::translate("Editor::MacroEditor", "M", 0));
        QTableWidgetItem *___qtablewidgetitem59 = tableWidget->item(3, 8);
        ___qtablewidgetitem59->setText(QApplication::translate("Editor::MacroEditor", ",", 0));
        QTableWidgetItem *___qtablewidgetitem60 = tableWidget->item(3, 9);
        ___qtablewidgetitem60->setText(QApplication::translate("Editor::MacroEditor", ".", 0));
        QTableWidgetItem *___qtablewidgetitem61 = tableWidget->item(4, 0);
        ___qtablewidgetitem61->setText(QApplication::translate("Editor::MacroEditor", "Do not assign to any key", 0));
        tableWidget->setSortingEnabled(__sortingEnabled);

        btnSave->setText(QApplication::translate("Editor::MacroEditor", "Save", 0));
        btnCancel->setText(QApplication::translate("Editor::MacroEditor", "Cancel", 0));
    } // retranslateUi

};

} // namespace Editor

namespace Editor {
namespace Ui {
    class MacroEditor: public Ui_MacroEditor {};
} // namespace Ui
} // namespace Editor

#endif // UI_MACROEDITOR_H
