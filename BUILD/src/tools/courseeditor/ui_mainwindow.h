/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowTask
{
public:
    QAction *loadCurs;
    QAction *checkTask;
    QAction *action_3;
    QAction *action_4;
    QAction *actionClose;
    QAction *do_task;
    QAction *actionSave;
    QAction *actionReset;
    QAction *actionTested;
    QAction *actionAdd;
    QAction *actionRemove;
    QAction *actionEdit;
    QAction *addDeep;
    QAction *actionSaveK;
    QAction *actionSaveKas;
    QAction *actionNewK;
    QAction *actionup;
    QAction *actionDown;
    QAction *actionS;
    QAction *actionMakeSection;
    QAction *actionMove;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QSplitter *splitter;
    QTreeView *treeView;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QVBoxLayout *verticalLayout;
    QTextEdit *descEdit;
    QFrame *typeGroup;
    QHBoxLayout *horizontalLayout_6;
    QRadioButton *zadRb;
    QRadioButton *nodeRb;
    QFrame *frame_7;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QLineEdit *prgEdit;
    QPushButton *selFileButt;
    QPushButton *editFButt;
    QLabel *label_3;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_4;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_3;
    QFrame *frame_5;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *ispSel;
    QPushButton *addIspButt;
    QListWidget *ispList;
    QPushButton *remIspButt;
    QFrame *frame_4;
    QVBoxLayout *verticalLayout_4;
    QListWidget *fieldsList;
    QFrame *frame_6;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *addFieldButt;
    QPushButton *remFieldButt;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *rescentMenu;
    QMenu *menu_2;
    QMenu *menuMove;

    void setupUi(QMainWindow *MainWindowTask)
    {
        if (MainWindowTask->objectName().isEmpty())
            MainWindowTask->setObjectName(QStringLiteral("MainWindowTask"));
        MainWindowTask->resize(734, 528);
        QIcon icon;
        icon.addFile(QStringLiteral(":/taskEdit.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindowTask->setWindowIcon(icon);
        loadCurs = new QAction(MainWindowTask);
        loadCurs->setObjectName(QStringLiteral("loadCurs"));
        checkTask = new QAction(MainWindowTask);
        checkTask->setObjectName(QStringLiteral("checkTask"));
        checkTask->setEnabled(false);
        action_3 = new QAction(MainWindowTask);
        action_3->setObjectName(QStringLiteral("action_3"));
        action_3->setEnabled(false);
        action_4 = new QAction(MainWindowTask);
        action_4->setObjectName(QStringLiteral("action_4"));
        action_4->setEnabled(false);
        actionClose = new QAction(MainWindowTask);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        do_task = new QAction(MainWindowTask);
        do_task->setObjectName(QStringLiteral("do_task"));
        do_task->setEnabled(false);
        do_task->setVisible(false);
        actionSave = new QAction(MainWindowTask);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionSave->setEnabled(false);
        actionReset = new QAction(MainWindowTask);
        actionReset->setObjectName(QStringLiteral("actionReset"));
        actionReset->setEnabled(false);
        actionTested = new QAction(MainWindowTask);
        actionTested->setObjectName(QStringLiteral("actionTested"));
        actionTested->setEnabled(false);
        actionAdd = new QAction(MainWindowTask);
        actionAdd->setObjectName(QStringLiteral("actionAdd"));
        actionAdd->setCheckable(false);
        actionRemove = new QAction(MainWindowTask);
        actionRemove->setObjectName(QStringLiteral("actionRemove"));
        actionEdit = new QAction(MainWindowTask);
        actionEdit->setObjectName(QStringLiteral("actionEdit"));
        actionEdit->setEnabled(false);
        addDeep = new QAction(MainWindowTask);
        addDeep->setObjectName(QStringLiteral("addDeep"));
        actionSaveK = new QAction(MainWindowTask);
        actionSaveK->setObjectName(QStringLiteral("actionSaveK"));
        actionSaveK->setVisible(false);
        actionSaveKas = new QAction(MainWindowTask);
        actionSaveKas->setObjectName(QStringLiteral("actionSaveKas"));
        actionNewK = new QAction(MainWindowTask);
        actionNewK->setObjectName(QStringLiteral("actionNewK"));
        actionup = new QAction(MainWindowTask);
        actionup->setObjectName(QStringLiteral("actionup"));
        actionup->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/arrow_up.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QStringLiteral(":/arrow_upDIS.svg"), QSize(), QIcon::Disabled, QIcon::Off);
        actionup->setIcon(icon1);
        actionDown = new QAction(MainWindowTask);
        actionDown->setObjectName(QStringLiteral("actionDown"));
        actionDown->setEnabled(false);
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/arrow_down.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QStringLiteral(":/arrow_downDIS.svg"), QSize(), QIcon::Disabled, QIcon::Off);
        actionDown->setIcon(icon2);
        actionS = new QAction(MainWindowTask);
        actionS->setObjectName(QStringLiteral("actionS"));
        actionS->setEnabled(false);
        actionMakeSection = new QAction(MainWindowTask);
        actionMakeSection->setObjectName(QStringLiteral("actionMakeSection"));
        actionMakeSection->setEnabled(false);
        actionMove = new QAction(MainWindowTask);
        actionMove->setObjectName(QStringLiteral("actionMove"));
        centralWidget = new QWidget(MainWindowTask);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        treeView = new QTreeView(splitter);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setMinimumSize(QSize(200, 100));
        treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        treeView->setProperty("showDropIndicator", QVariant(false));
        treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        treeView->setSelectionBehavior(QAbstractItemView::SelectItems);
        treeView->setIndentation(16);
        treeView->setUniformRowHeights(true);
        splitter->addWidget(treeView);
        horizontalLayoutWidget = new QWidget(splitter);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(horizontalLayoutWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);

        verticalLayout_2->addWidget(label);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        descEdit = new QTextEdit(frame);
        descEdit->setObjectName(QStringLiteral("descEdit"));
        descEdit->setEnabled(false);
        descEdit->setMinimumSize(QSize(0, 20));

        verticalLayout->addWidget(descEdit);


        verticalLayout_2->addLayout(verticalLayout);

        typeGroup = new QFrame(frame);
        typeGroup->setObjectName(QStringLiteral("typeGroup"));
        typeGroup->setEnabled(false);
        typeGroup->setFrameShape(QFrame::StyledPanel);
        typeGroup->setFrameShadow(QFrame::Raised);
        horizontalLayout_6 = new QHBoxLayout(typeGroup);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        zadRb = new QRadioButton(typeGroup);
        zadRb->setObjectName(QStringLiteral("zadRb"));
        zadRb->setChecked(true);

        horizontalLayout_6->addWidget(zadRb);

        nodeRb = new QRadioButton(typeGroup);
        nodeRb->setObjectName(QStringLiteral("nodeRb"));

        horizontalLayout_6->addWidget(nodeRb);


        verticalLayout_2->addWidget(typeGroup);

        frame_7 = new QFrame(frame);
        frame_7->setObjectName(QStringLiteral("frame_7"));
        frame_7->setFrameShape(QFrame::NoFrame);
        frame_7->setFrameShadow(QFrame::Plain);
        frame_7->setLineWidth(0);
        horizontalLayout_5 = new QHBoxLayout(frame_7);
        horizontalLayout_5->setSpacing(-1);
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_2 = new QLabel(frame_7);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setFont(font);

        horizontalLayout_5->addWidget(label_2);

        prgEdit = new QLineEdit(frame_7);
        prgEdit->setObjectName(QStringLiteral("prgEdit"));
        prgEdit->setEnabled(false);

        horizontalLayout_5->addWidget(prgEdit);

        selFileButt = new QPushButton(frame_7);
        selFileButt->setObjectName(QStringLiteral("selFileButt"));
        selFileButt->setEnabled(false);

        horizontalLayout_5->addWidget(selFileButt);

        editFButt = new QPushButton(frame_7);
        editFButt->setObjectName(QStringLiteral("editFButt"));
        editFButt->setEnabled(false);

        horizontalLayout_5->addWidget(editFButt);


        verticalLayout_2->addWidget(frame_7);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setFont(font);

        verticalLayout_2->addWidget(label_3);

        frame_2 = new QFrame(frame);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::Box);
        frame_2->setFrameShadow(QFrame::Raised);
        frame_2->setLineWidth(0);
        horizontalLayout_4 = new QHBoxLayout(frame_2);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        frame_3 = new QFrame(frame_2);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setMinimumSize(QSize(0, 100));
        frame_3->setFrameShape(QFrame::NoFrame);
        frame_3->setFrameShadow(QFrame::Plain);
        frame_3->setLineWidth(0);
        verticalLayout_3 = new QVBoxLayout(frame_3);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        frame_5 = new QFrame(frame_3);
        frame_5->setObjectName(QStringLiteral("frame_5"));
        frame_5->setFrameShape(QFrame::Box);
        frame_5->setFrameShadow(QFrame::Plain);
        frame_5->setLineWidth(0);
        horizontalLayout_2 = new QHBoxLayout(frame_5);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        ispSel = new QComboBox(frame_5);
        ispSel->setObjectName(QStringLiteral("ispSel"));
        ispSel->setEnabled(false);

        horizontalLayout_2->addWidget(ispSel);

        addIspButt = new QPushButton(frame_5);
        addIspButt->setObjectName(QStringLiteral("addIspButt"));
        addIspButt->setEnabled(false);

        horizontalLayout_2->addWidget(addIspButt);


        verticalLayout_3->addWidget(frame_5);

        ispList = new QListWidget(frame_3);
        ispList->setObjectName(QStringLiteral("ispList"));
        ispList->setEnabled(false);

        verticalLayout_3->addWidget(ispList);

        remIspButt = new QPushButton(frame_3);
        remIspButt->setObjectName(QStringLiteral("remIspButt"));
        remIspButt->setEnabled(false);

        verticalLayout_3->addWidget(remIspButt);


        horizontalLayout_4->addWidget(frame_3);

        frame_4 = new QFrame(frame_2);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setFrameShape(QFrame::NoFrame);
        frame_4->setFrameShadow(QFrame::Raised);
        frame_4->setLineWidth(0);
        verticalLayout_4 = new QVBoxLayout(frame_4);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        fieldsList = new QListWidget(frame_4);
        fieldsList->setObjectName(QStringLiteral("fieldsList"));
        fieldsList->setEnabled(false);

        verticalLayout_4->addWidget(fieldsList);

        frame_6 = new QFrame(frame_4);
        frame_6->setObjectName(QStringLiteral("frame_6"));
        frame_6->setFrameShape(QFrame::NoFrame);
        frame_6->setFrameShadow(QFrame::Plain);
        frame_6->setLineWidth(0);
        horizontalLayout_3 = new QHBoxLayout(frame_6);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        addFieldButt = new QPushButton(frame_6);
        addFieldButt->setObjectName(QStringLiteral("addFieldButt"));
        addFieldButt->setEnabled(false);

        horizontalLayout_3->addWidget(addFieldButt);

        remFieldButt = new QPushButton(frame_6);
        remFieldButt->setObjectName(QStringLiteral("remFieldButt"));
        remFieldButt->setEnabled(false);

        horizontalLayout_3->addWidget(remFieldButt);


        verticalLayout_4->addWidget(frame_6);


        horizontalLayout_4->addWidget(frame_4);


        verticalLayout_2->addWidget(frame_2);


        horizontalLayout->addWidget(frame);

        splitter->addWidget(horizontalLayoutWidget);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        MainWindowTask->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindowTask);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowTask->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowTask);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowTask->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindowTask);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 734, 22));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        rescentMenu = new QMenu(menu);
        rescentMenu->setObjectName(QStringLiteral("rescentMenu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menuMove = new QMenu(menu_2);
        menuMove->setObjectName(QStringLiteral("menuMove"));
        menuMove->setEnabled(false);
        MainWindowTask->setMenuBar(menuBar);

        mainToolBar->addAction(loadCurs);
        mainToolBar->addAction(do_task);
        mainToolBar->addAction(actionS);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionup);
        mainToolBar->addAction(actionDown);
        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menu->addAction(actionNewK);
        menu->addAction(loadCurs);
        menu->addAction(rescentMenu->menuAction());
        menu->addAction(actionS);
        menu->addAction(actionSave);
        menu->addSeparator();
        menu->addAction(actionClose);
        menu->addSeparator();
        rescentMenu->addSeparator();
        menu_2->addAction(do_task);
        menu_2->addAction(actionAdd);
        menu_2->addAction(actionRemove);
        menu_2->addAction(addDeep);
        menu_2->addAction(actionup);
        menu_2->addAction(actionDown);
        menu_2->addAction(actionMakeSection);
        menu_2->addAction(menuMove->menuAction());
        menuMove->addSeparator();

        retranslateUi(MainWindowTask);

        QMetaObject::connectSlotsByName(MainWindowTask);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowTask)
    {
        MainWindowTask->setWindowTitle(QApplication::translate("MainWindowTask", "\320\240\320\265\320\264\320\260\320\272\321\202\320\276\321\200 \320\277\321\200\320\260\320\272\321\202\320\270\320\272\321\203\320\274\320\276\320\262", 0));
        loadCurs->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214", 0));
        checkTask->setText(QApplication::translate("MainWindowTask", "\320\237\321\200\320\276\320\262\320\265\321\200\320\270\321\202\321\214", 0));
        action_3->setText(QApplication::translate("MainWindowTask", "\320\241\320\261\321\200\320\276\321\201\320\270\321\202\321\214", 0));
        action_4->setText(QApplication::translate("MainWindowTask", "\320\237\320\276\320\264\321\201\320\272\320\260\320\267\320\272\320\260", 0));
        actionClose->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\272\321\200\321\213\321\202\321\214", 0));
        do_task->setText(QApplication::translate("MainWindowTask", "\320\222\321\213\320\277\320\276\320\273\320\275\321\217\321\202\321\214", 0));
        actionSave->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\272\320\260\320\272...", 0));
        actionReset->setText(QApplication::translate("MainWindowTask", "\320\241\320\261\321\200\320\276\321\201\320\270\321\202\321\214", 0));
        actionTested->setText(QApplication::translate("MainWindowTask", "\320\222\320\265\321\200\320\275\321\203\321\202\321\214\321\201\321\217 \320\272 \320\277\321\200\320\276\320\262\320\265\321\200\320\265\320\275\320\275\320\276\320\271", 0));
        actionAdd->setText(QApplication::translate("MainWindowTask", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0));
        actionRemove->setText(QApplication::translate("MainWindowTask", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 ", 0));
        actionEdit->setText(QApplication::translate("MainWindowTask", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0));
        addDeep->setText(QApplication::translate("MainWindowTask", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214 \320\262\320\263\320\273\321\203\320\261\321\214", 0));
        actionSaveK->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", 0));
        actionSaveKas->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\272\320\260\320\272...", 0));
        actionNewK->setText(QApplication::translate("MainWindowTask", "\320\235\320\276\320\262\321\213\320\271", 0));
        actionup->setText(QApplication::translate("MainWindowTask", "\320\241\320\264\320\262\320\270\320\275\321\203\321\202\321\214 \320\262\320\262\320\265\321\200\321\205", 0));
        actionDown->setText(QApplication::translate("MainWindowTask", "\320\241\320\264\320\262\320\270\320\275\321\203\321\202\321\214 \320\262\320\275\320\270\320\267", 0));
        actionS->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 ", 0));
        actionMakeSection->setText(QApplication::translate("MainWindowTask", "\320\236\320\261\321\212\320\265\320\264\320\265\320\275\320\270\321\202\321\214 \320\262 \321\200\320\260\320\267\320\264\320\265\320\273", 0));
        actionMove->setText(QApplication::translate("MainWindowTask", "\320\237\320\265\321\200\320\265\320\274\320\265\321\201\321\202\320\270\321\202\321\214", 0));
        label->setText(QApplication::translate("MainWindowTask", "\320\236\320\277\320\270\321\201\320\260\320\275\320\270\320\265 \320\267\320\260\320\264\320\260\320\275\320\270\321\217:", 0));
        zadRb->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\264\320\260\320\275\320\270\320\265", 0));
        nodeRb->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\263\320\276\320\273\320\276\320\262\320\276\320\272 \321\200\320\260\320\267\320\264\320\265\320\273\320\260", 0));
        label_2->setText(QApplication::translate("MainWindowTask", "\320\237\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\260:", 0));
        selFileButt->setText(QApplication::translate("MainWindowTask", "...", 0));
        editFButt->setText(QApplication::translate("MainWindowTask", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0));
        label_3->setText(QApplication::translate("MainWindowTask", "\320\230\321\201\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\320\270 \320\270 \320\276\320\261\321\201\321\202\320\260\320\275\320\276\320\262\320\272\320\270:", 0));
        addIspButt->setText(QApplication::translate("MainWindowTask", "+", 0));
        remIspButt->setText(QApplication::translate("MainWindowTask", "-", 0));
        addFieldButt->setText(QApplication::translate("MainWindowTask", "+", 0));
        remFieldButt->setText(QApplication::translate("MainWindowTask", "-", 0));
        menu->setTitle(QApplication::translate("MainWindowTask", "\320\237\321\200\320\260\320\272\321\202\320\270\320\272\321\203\320\274", 0));
        rescentMenu->setTitle(QApplication::translate("MainWindowTask", "\320\235\320\265\320\264\320\260\320\262\320\275\320\270\320\265", 0));
        menu_2->setTitle(QApplication::translate("MainWindowTask", "\320\227\320\260\320\264\320\260\320\275\320\270\320\265", 0));
        menuMove->setTitle(QApplication::translate("MainWindowTask", "\320\237\320\265\321\200\320\265\320\274\320\265\321\201\321\202\320\270\321\202\321\214", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowTask: public Ui_MainWindowTask {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
