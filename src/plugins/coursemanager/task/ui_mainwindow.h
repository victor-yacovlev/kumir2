/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Thu Apr 26 15:53:48 2012
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QWidget>
#include <QtWebKit/QWebView>

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
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QSplitter *splitter;
    QTreeView *treeView;
    QWebView *webView;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menuKurs;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowTask)
    {
        if (MainWindowTask->objectName().isEmpty())
            MainWindowTask->setObjectName(QString::fromUtf8("MainWindowTask"));
        MainWindowTask->resize(545, 405);
        loadCurs = new QAction(MainWindowTask);
        loadCurs->setObjectName(QString::fromUtf8("loadCurs"));
        checkTask = new QAction(MainWindowTask);
        checkTask->setObjectName(QString::fromUtf8("checkTask"));
        checkTask->setEnabled(false);
        action_3 = new QAction(MainWindowTask);
        action_3->setObjectName(QString::fromUtf8("action_3"));
        action_3->setEnabled(false);
        action_4 = new QAction(MainWindowTask);
        action_4->setObjectName(QString::fromUtf8("action_4"));
        action_4->setEnabled(false);
        actionClose = new QAction(MainWindowTask);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        do_task = new QAction(MainWindowTask);
        do_task->setObjectName(QString::fromUtf8("do_task"));
        do_task->setEnabled(false);
        do_task->setVisible(false);
        actionSave = new QAction(MainWindowTask);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSave->setEnabled(false);
        actionReset = new QAction(MainWindowTask);
        actionReset->setObjectName(QString::fromUtf8("actionReset"));
        actionReset->setEnabled(false);
        actionTested = new QAction(MainWindowTask);
        actionTested->setObjectName(QString::fromUtf8("actionTested"));
        actionTested->setEnabled(false);
        actionAdd = new QAction(MainWindowTask);
        actionAdd->setObjectName(QString::fromUtf8("actionAdd"));
        actionAdd->setCheckable(false);
        actionRemove = new QAction(MainWindowTask);
        actionRemove->setObjectName(QString::fromUtf8("actionRemove"));
        actionEdit = new QAction(MainWindowTask);
        actionEdit->setObjectName(QString::fromUtf8("actionEdit"));
        actionEdit->setEnabled(false);
        addDeep = new QAction(MainWindowTask);
        addDeep->setObjectName(QString::fromUtf8("addDeep"));
        actionSaveK = new QAction(MainWindowTask);
        actionSaveK->setObjectName(QString::fromUtf8("actionSaveK"));
        actionSaveK->setVisible(false);
        actionSaveKas = new QAction(MainWindowTask);
        actionSaveKas->setObjectName(QString::fromUtf8("actionSaveKas"));
        actionNewK = new QAction(MainWindowTask);
        actionNewK->setObjectName(QString::fromUtf8("actionNewK"));
        actionup = new QAction(MainWindowTask);
        actionup->setObjectName(QString::fromUtf8("actionup"));
        actionup->setEnabled(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/arrow_up.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/arrow_upDIS.svg"), QSize(), QIcon::Disabled, QIcon::Off);
        actionup->setIcon(icon);
        actionDown = new QAction(MainWindowTask);
        actionDown->setObjectName(QString::fromUtf8("actionDown"));
        actionDown->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/arrow_down.svg"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/arrow_downDIS.svg"), QSize(), QIcon::Disabled, QIcon::Off);
        actionDown->setIcon(icon1);
        centralWidget = new QWidget(MainWindowTask);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);

        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        treeView = new QTreeView(splitter);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setMinimumSize(QSize(200, 100));
        treeView->setProperty("showDropIndicator", QVariant(false));
        treeView->setIconSize(QSize(32, 32));
        treeView->setIndentation(20);
        treeView->setUniformRowHeights(true);
        treeView->setAnimated(true);
        splitter->addWidget(treeView);
        webView = new QWebView(splitter);
        webView->setObjectName(QString::fromUtf8("webView"));
        webView->setUrl(QUrl("about:blank"));
        webView->setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing|QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);
        splitter->addWidget(webView);

        gridLayout_2->addWidget(splitter, 0, 1, 1, 1);

        MainWindowTask->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowTask);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 545, 22));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menuKurs = new QMenu(menuBar);
        menuKurs->setObjectName(QString::fromUtf8("menuKurs"));
        menuKurs->setEnabled(true);
        MainWindowTask->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowTask);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindowTask->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowTask);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindowTask->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menuKurs->menuAction());
        menu->addAction(loadCurs);
        menu->addAction(actionSave);
        menu->addSeparator();
        menu->addAction(actionClose);
        menu->addSeparator();
        menu_2->addAction(do_task);
        menu_2->addAction(checkTask);
        menu_2->addAction(action_4);
        menu_2->addAction(actionReset);
        menu_2->addAction(actionTested);
        menuKurs->addAction(actionAdd);
        menuKurs->addAction(actionRemove);
        menuKurs->addAction(actionEdit);
        menuKurs->addAction(addDeep);
        menuKurs->addSeparator();
        menuKurs->addAction(actionSaveK);
        menuKurs->addAction(actionSaveKas);
        menuKurs->addAction(actionNewK);
        menuKurs->addSeparator();
        menuKurs->addAction(actionup);
        menuKurs->addAction(actionDown);
        mainToolBar->addAction(loadCurs);
        mainToolBar->addAction(do_task);
        mainToolBar->addAction(checkTask);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionup);
        mainToolBar->addAction(actionDown);

        retranslateUi(MainWindowTask);

        QMetaObject::connectSlotsByName(MainWindowTask);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowTask)
    {
        MainWindowTask->setWindowTitle(QApplication::translate("MainWindowTask", "\320\237\321\200\320\260\320\272\321\202\320\270\320\272\321\203\320\274", 0, QApplication::UnicodeUTF8));
        loadCurs->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \320\272\321\203\321\200\321\201", 0, QApplication::UnicodeUTF8));
        checkTask->setText(QApplication::translate("MainWindowTask", "\320\237\321\200\320\276\320\262\320\265\321\200\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        action_3->setText(QApplication::translate("MainWindowTask", "\320\241\320\261\321\200\320\276\321\201\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        action_4->setText(QApplication::translate("MainWindowTask", "\320\237\320\276\320\264\321\201\320\272\320\260\320\267\320\272\320\260", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("MainWindowTask", "\320\227\320\260\320\272\321\200\321\213\321\202\321\214", 0, QApplication::UnicodeUTF8));
        do_task->setText(QApplication::translate("MainWindowTask", "\320\222\321\213\320\277\320\276\320\273\320\275\321\217\321\202\321\214", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\272\320\260\320\272...", 0, QApplication::UnicodeUTF8));
        actionReset->setText(QApplication::translate("MainWindowTask", "\320\222\320\265\321\200\320\275\321\203\321\202\321\214\321\201\321\217 \320\272 \320\270\321\201\321\205\320\276\320\264\320\275\320\276\320\271", 0, QApplication::UnicodeUTF8));
        actionTested->setText(QApplication::translate("MainWindowTask", "\320\222\320\265\321\200\320\275\321\203\321\202\321\214\321\201\321\217 \320\272 \320\277\321\200\320\276\320\262\320\265\321\200\320\265\320\275\320\275\320\276\320\271", 0, QApplication::UnicodeUTF8));
        actionAdd->setText(QApplication::translate("MainWindowTask", "\320\232\320\276\320\277\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        actionRemove->setText(QApplication::translate("MainWindowTask", "\320\243\320\264\320\260\320\273\320\270\321\202\321\214 ", 0, QApplication::UnicodeUTF8));
        actionEdit->setText(QApplication::translate("MainWindowTask", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
        addDeep->setText(QApplication::translate("MainWindowTask", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\262\320\263\320\273\321\203\320\261\321\214", 0, QApplication::UnicodeUTF8));
        actionSaveK->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
        actionSaveKas->setText(QApplication::translate("MainWindowTask", "\320\241\320\276\321\205\321\200\320\260\320\275\320\270\321\202\321\214 \320\272\320\260\320\272...", 0, QApplication::UnicodeUTF8));
        actionNewK->setText(QApplication::translate("MainWindowTask", "\320\235\320\276\320\262\321\213\320\271 \320\272\321\203\321\200\321\201", 0, QApplication::UnicodeUTF8));
        actionup->setText(QApplication::translate("MainWindowTask", "\320\237\320\265\321\200\320\265\320\274\320\265\321\201\321\202\320\270\321\202\321\214 \320\262\320\262\320\265\321\200\321\205", 0, QApplication::UnicodeUTF8));
        actionDown->setText(QApplication::translate("MainWindowTask", "\320\237\320\265\321\200\320\265\320\274\320\265\321\201\321\202\320\270\321\202\321\214 \320\262\320\275\320\270\320\267", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MainWindowTask", "\320\237\321\200\320\260\320\272\321\202\320\270\320\272\321\203\320\274", 0, QApplication::UnicodeUTF8));
        menu_2->setTitle(QApplication::translate("MainWindowTask", "\320\227\320\260\320\264\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
        menuKurs->setTitle(QApplication::translate("MainWindowTask", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowTask: public Ui_MainWindowTask {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
