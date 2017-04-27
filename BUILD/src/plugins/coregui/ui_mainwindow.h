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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QWidget>
#include "menubar.h"

namespace CoreGUI {

class Ui_MainWindow
{
public:
    QAction *actionNewProgram;
    QAction *actionNewText;
    QAction *actionOpen;
    QAction *actionRecent_files;
    QAction *actionSave;
    QAction *actionSave_as;
    QAction *actionSave_all;
    QAction *actionClose;
    QAction *actionSwitch_workspace;
    QAction *actionExit;
    QAction *actionUsage;
    QAction *actionAbout;
    QAction *actionPreferences;
    QAction *actionNew_pascal_program;
    QAction *actionRestore_previous_session;
    QAction *actionVariables;
    QAction *actionShow_Console_Pane;
    QAction *actionMake_native_executable;
    QAction *actionLanguage_Quick_Reference;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QSplitter *splitter;
    MenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QMenu *menuEdit;
    QMenu *menuInsert;
    QMenu *menuRun;
    QMenu *menuWindow;

    void setupUi(QMainWindow *CoreGUI__MainWindow)
    {
        if (CoreGUI__MainWindow->objectName().isEmpty())
            CoreGUI__MainWindow->setObjectName(QStringLiteral("CoreGUI__MainWindow"));
        CoreGUI__MainWindow->resize(742, 542);
        CoreGUI__MainWindow->setTabShape(QTabWidget::Triangular);
        CoreGUI__MainWindow->setDockNestingEnabled(false);
        CoreGUI__MainWindow->setUnifiedTitleAndToolBarOnMac(true);
        actionNewProgram = new QAction(CoreGUI__MainWindow);
        actionNewProgram->setObjectName(QStringLiteral("actionNewProgram"));
        actionNewText = new QAction(CoreGUI__MainWindow);
        actionNewText->setObjectName(QStringLiteral("actionNewText"));
        actionOpen = new QAction(CoreGUI__MainWindow);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionRecent_files = new QAction(CoreGUI__MainWindow);
        actionRecent_files->setObjectName(QStringLiteral("actionRecent_files"));
        actionSave = new QAction(CoreGUI__MainWindow);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        actionSave_as = new QAction(CoreGUI__MainWindow);
        actionSave_as->setObjectName(QStringLiteral("actionSave_as"));
        actionSave_all = new QAction(CoreGUI__MainWindow);
        actionSave_all->setObjectName(QStringLiteral("actionSave_all"));
        actionClose = new QAction(CoreGUI__MainWindow);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        actionSwitch_workspace = new QAction(CoreGUI__MainWindow);
        actionSwitch_workspace->setObjectName(QStringLiteral("actionSwitch_workspace"));
        actionExit = new QAction(CoreGUI__MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionExit->setMenuRole(QAction::QuitRole);
        actionUsage = new QAction(CoreGUI__MainWindow);
        actionUsage->setObjectName(QStringLiteral("actionUsage"));
        actionUsage->setCheckable(false);
        actionUsage->setShortcutContext(Qt::ApplicationShortcut);
        actionAbout = new QAction(CoreGUI__MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionPreferences = new QAction(CoreGUI__MainWindow);
        actionPreferences->setObjectName(QStringLiteral("actionPreferences"));
        actionPreferences->setMenuRole(QAction::PreferencesRole);
        actionNew_pascal_program = new QAction(CoreGUI__MainWindow);
        actionNew_pascal_program->setObjectName(QStringLiteral("actionNew_pascal_program"));
        actionRestore_previous_session = new QAction(CoreGUI__MainWindow);
        actionRestore_previous_session->setObjectName(QStringLiteral("actionRestore_previous_session"));
        actionVariables = new QAction(CoreGUI__MainWindow);
        actionVariables->setObjectName(QStringLiteral("actionVariables"));
        actionVariables->setCheckable(false);
        actionVariables->setShortcutContext(Qt::ApplicationShortcut);
        actionShow_Console_Pane = new QAction(CoreGUI__MainWindow);
        actionShow_Console_Pane->setObjectName(QStringLiteral("actionShow_Console_Pane"));
        actionShow_Console_Pane->setCheckable(true);
        actionMake_native_executable = new QAction(CoreGUI__MainWindow);
        actionMake_native_executable->setObjectName(QStringLiteral("actionMake_native_executable"));
        actionLanguage_Quick_Reference = new QAction(CoreGUI__MainWindow);
        actionLanguage_Quick_Reference->setObjectName(QStringLiteral("actionLanguage_Quick_Reference"));
        actionLanguage_Quick_Reference->setShortcutContext(Qt::ApplicationShortcut);
        actionLanguage_Quick_Reference->setVisible(false);
        centralwidget = new QWidget(CoreGUI__MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        splitter->setHandleWidth(10);

        gridLayout->addWidget(splitter, 0, 0, 1, 1);

        CoreGUI__MainWindow->setCentralWidget(centralwidget);
        menubar = new MenuBar(CoreGUI__MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 742, 28));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuFile->setTearOffEnabled(false);
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        menuHelp->setTearOffEnabled(false);
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuInsert = new QMenu(menubar);
        menuInsert->setObjectName(QStringLiteral("menuInsert"));
        menuRun = new QMenu(menubar);
        menuRun->setObjectName(QStringLiteral("menuRun"));
        menuWindow = new QMenu(menubar);
        menuWindow->setObjectName(QStringLiteral("menuWindow"));
        CoreGUI__MainWindow->setMenuBar(menubar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuInsert->menuAction());
        menubar->addAction(menuRun->menuAction());
        menubar->addAction(menuWindow->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNewProgram);
        menuFile->addAction(actionNewText);
        menuFile->addSeparator();
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionRecent_files);
        menuFile->addSeparator();
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSave_as);
        menuFile->addAction(actionSave_all);
        menuFile->addAction(actionMake_native_executable);
        menuFile->addSeparator();
        menuFile->addAction(actionClose);
        menuFile->addSeparator();
        menuFile->addAction(actionSwitch_workspace);
        menuFile->addAction(actionRestore_previous_session);
        menuFile->addSeparator();
        menuFile->addAction(actionPreferences);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionUsage);
        menuHelp->addAction(actionVariables);
        menuHelp->addAction(actionLanguage_Quick_Reference);
        menuHelp->addAction(actionAbout);
        menuWindow->addAction(actionShow_Console_Pane);

        retranslateUi(CoreGUI__MainWindow);
        QObject::connect(actionExit, SIGNAL(triggered()), CoreGUI__MainWindow, SLOT(close()));

        QMetaObject::connectSlotsByName(CoreGUI__MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *CoreGUI__MainWindow)
    {
        CoreGUI__MainWindow->setWindowTitle(QApplication::translate("CoreGUI::MainWindow", "Kumir", 0));
        actionNewProgram->setText(QApplication::translate("CoreGUI::MainWindow", "&New program", 0));
        actionNewProgram->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+N", 0));
        actionNewText->setText(QApplication::translate("CoreGUI::MainWindow", "New te&xt", 0));
        actionNewText->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+Shift+N", 0));
        actionOpen->setText(QApplication::translate("CoreGUI::MainWindow", "&Open...", 0));
        actionOpen->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+O", 0));
        actionRecent_files->setText(QApplication::translate("CoreGUI::MainWindow", "&Recent files", 0));
        actionSave->setText(QApplication::translate("CoreGUI::MainWindow", "&Save", 0));
        actionSave->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+S", 0));
        actionSave_as->setText(QApplication::translate("CoreGUI::MainWindow", "Sa&ve as...", 0));
        actionSave_all->setText(QApplication::translate("CoreGUI::MainWindow", "Save a&ll", 0));
        actionSave_all->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+Shift+S", 0));
        actionClose->setText(QApplication::translate("CoreGUI::MainWindow", "&Close", 0));
        actionClose->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+W", 0));
        actionSwitch_workspace->setText(QApplication::translate("CoreGUI::MainWindow", "Switch &workspace...", 0));
        actionExit->setText(QApplication::translate("CoreGUI::MainWindow", "&Exit", 0));
        actionExit->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+Q", 0));
        actionUsage->setText(QApplication::translate("CoreGUI::MainWindow", "&Manuals", 0));
        actionUsage->setShortcut(QApplication::translate("CoreGUI::MainWindow", "F1", 0));
        actionAbout->setText(QApplication::translate("CoreGUI::MainWindow", "&About...", 0));
        actionPreferences->setText(QApplication::translate("CoreGUI::MainWindow", "&Preferences...", 0));
        actionNew_pascal_program->setText(QApplication::translate("CoreGUI::MainWindow", "New Pascal program", 0));
        actionNew_pascal_program->setShortcut(QApplication::translate("CoreGUI::MainWindow", "Ctrl+P", 0));
        actionRestore_previous_session->setText(QApplication::translate("CoreGUI::MainWindow", "Restore prev&ious session", 0));
        actionVariables->setText(QApplication::translate("CoreGUI::MainWindow", "&Variable Current Values", 0));
        actionVariables->setShortcut(QApplication::translate("CoreGUI::MainWindow", "F2", 0));
        actionShow_Console_Pane->setText(QApplication::translate("CoreGUI::MainWindow", "&Show Console Pane", 0));
        actionShow_Console_Pane->setShortcut(QApplication::translate("CoreGUI::MainWindow", "F12", 0));
        actionMake_native_executable->setText(QApplication::translate("CoreGUI::MainWindow", "&Make native executable...", 0));
        actionLanguage_Quick_Reference->setText(QApplication::translate("CoreGUI::MainWindow", "Language Quick Reference", 0));
        actionLanguage_Quick_Reference->setShortcut(QApplication::translate("CoreGUI::MainWindow", "F3", 0));
        menuFile->setTitle(QApplication::translate("CoreGUI::MainWindow", "Fi&le", 0));
        menuHelp->setTitle(QApplication::translate("CoreGUI::MainWindow", "Help", 0));
        menuEdit->setTitle(QApplication::translate("CoreGUI::MainWindow", "Edit", 0));
        menuInsert->setTitle(QApplication::translate("CoreGUI::MainWindow", "&Insert", 0));
        menuRun->setTitle(QApplication::translate("CoreGUI::MainWindow", "&Run", 0));
        menuWindow->setTitle(QApplication::translate("CoreGUI::MainWindow", "Wi&ndow", 0));
    } // retranslateUi

};

} // namespace CoreGUI

namespace CoreGUI {
namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui
} // namespace CoreGUI

#endif // UI_MAINWINDOW_H
