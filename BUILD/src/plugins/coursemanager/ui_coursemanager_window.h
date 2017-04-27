/********************************************************************************
** Form generated from reading UI file 'coursemanager_window.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COURSEMANAGER_WINDOW_H
#define UI_COURSEMANAGER_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
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
    QAction *actionNext;
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QSplitter *splitter;
    QTreeView *treeView;
    QWidget *webView;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *MainWindowTask)
    {
        if (MainWindowTask->objectName().isEmpty())
            MainWindowTask->setObjectName(QStringLiteral("MainWindowTask"));
        MainWindowTask->resize(545, 405);
        MainWindowTask->setMinimumSize(QSize(100, 100));
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
        QIcon icon;
        icon.addFile(QStringLiteral(":/arrow_up.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QStringLiteral(":/arrow_upDIS.svg"), QSize(), QIcon::Disabled, QIcon::Off);
        actionup->setIcon(icon);
        actionDown = new QAction(MainWindowTask);
        actionDown->setObjectName(QStringLiteral("actionDown"));
        actionDown->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/arrow_down.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QStringLiteral(":/arrow_downDIS.svg"), QSize(), QIcon::Disabled, QIcon::Off);
        actionDown->setIcon(icon1);
        actionNext = new QAction(MainWindowTask);
        actionNext->setObjectName(QStringLiteral("actionNext"));
        actionNext->setEnabled(false);
        centralWidget = new QWidget(MainWindowTask);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(0);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        treeView = new QTreeView(splitter);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setMinimumSize(QSize(50, 100));
        QFont font;
        font.setPointSize(10);
        font.setKerning(false);
        treeView->setFont(font);
        treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeView->setProperty("showDropIndicator", QVariant(false));
        treeView->setIconSize(QSize(20, 20));
        treeView->setIndentation(15);
        treeView->setUniformRowHeights(true);
        treeView->setAnimated(true);
        splitter->addWidget(treeView);
        webView = new QWidget(splitter);
        webView->setObjectName(QStringLiteral("webView"));
        splitter->addWidget(webView);

        gridLayout_2->addWidget(splitter, 0, 0, 1, 1);

        MainWindowTask->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindowTask);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowTask->addToolBar(Qt::TopToolBarArea, mainToolBar);

        mainToolBar->addAction(loadCurs);
        mainToolBar->addAction(do_task);
        mainToolBar->addAction(checkTask);
        mainToolBar->addAction(actionNext);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionup);
        mainToolBar->addAction(actionDown);

        retranslateUi(MainWindowTask);

        QMetaObject::connectSlotsByName(MainWindowTask);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowTask)
    {
        MainWindowTask->setWindowTitle(QApplication::translate("MainWindowTask", "\320\237\321\200\320\260\320\272\321\202\320\270\320\272\321\203\320\274", 0));
        loadCurs->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \320\272\321\203\321\200\321\201", 0));
        checkTask->setText(QApplication::translate("MainWindowTask", "\320\237\321\200\320\276\320\262\320\265\321\200\320\270\321\202\321\214", 0));
        action_3->setText(QApplication::translate("MainWindowTask", "\320\241\320\261\321\200\320\276\321\201\320\270\321\202\321\214", 0));
        action_4->setText(QApplication::translate("MainWindowTask", "\320\237\320\276\320\264\321\201\320\272\320\260\320\267\320\272\320\260", 0));
        actionClose->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\272\321\200\321\213\321\202\321\214", 0));
        do_task->setText(QApplication::translate("MainWindowTask", "\320\222\321\213\320\277\320\276\320\273\320\275\321\217\321\202\321\214", 0));
        actionSave->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\272\320\260\320\272...", 0));
        actionReset->setText(QApplication::translate("MainWindowTask", "\320\222\320\265\321\200\320\275\321\203\321\202\321\214\321\201\321\217 \320\272 \320\270\321\201\321\205\320\276\320\264\320\275\320\276\320\271", 0));
        actionTested->setText(QApplication::translate("MainWindowTask", "\320\222\320\265\321\200\320\275\321\203\321\202\321\214\321\201\321\217 \320\272 \320\277\321\200\320\276\320\262\320\265\321\200\320\265\320\275\320\275\320\276\320\271", 0));
        actionAdd->setText(QApplication::translate("MainWindowTask", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0));
        actionRemove->setText(QApplication::translate("MainWindowTask", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 ", 0));
        actionEdit->setText(QApplication::translate("MainWindowTask", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0));
        addDeep->setText(QApplication::translate("MainWindowTask", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\262\320\263\320\273\321\203\320\261\321\214", 0));
        actionSaveK->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", 0));
        actionSaveKas->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\272\320\260\320\272...", 0));
        actionNewK->setText(QApplication::translate("MainWindowTask", "\320\235\320\276\320\262\321\213\320\271 \320\272\321\203\321\200\321\201", 0));
        actionup->setText(QApplication::translate("MainWindowTask", "\320\237\320\265\321\200\320\265\320\274\320\265\321\201\321\202\320\270\321\202\321\214 \320\262\320\262\320\265\321\200\321\205", 0));
        actionDown->setText(QApplication::translate("MainWindowTask", "\320\237\320\265\321\200\320\265\320\274\320\265\321\201\321\202\320\270\321\202\321\214 \320\262\320\275\320\270\320\267", 0));
        actionNext->setText(QApplication::translate("MainWindowTask", "\320\241\320\273\320\265\320\264\321\203\321\216\321\211\320\265\320\265 \320\267\320\260\320\264\320\260\320\275\320\270\320\265", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindowTask: public Ui_MainWindowTask {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COURSEMANAGER_WINDOW_H
