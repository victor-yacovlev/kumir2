/********************************************************************************
** Form generated from reading UI file 'aboutdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTDIALOG_H
#define UI_ABOUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <kumir2-libs/widgets/utf8textbrowser.h>

namespace CoreGUI {

class Ui_AboutDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QGridLayout *gridLayout;
    Widgets::Utf8TextBrowser *aboutTextBrowser;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QTextBrowser *textBrowser;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label;
    QLineEdit *version;
    QLabel *label_3;
    QLineEdit *lastModified;
    QTableWidget *tableWidget;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnCopyEnvironmentAndVersion;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnClose;

    void setupUi(QDialog *CoreGUI__AboutDialog)
    {
        if (CoreGUI__AboutDialog->objectName().isEmpty())
            CoreGUI__AboutDialog->setObjectName(QStringLiteral("CoreGUI__AboutDialog"));
        CoreGUI__AboutDialog->resize(647, 494);
        verticalLayout = new QVBoxLayout(CoreGUI__AboutDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(CoreGUI__AboutDialog);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout = new QGridLayout(tab_3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        aboutTextBrowser = new Widgets::Utf8TextBrowser(tab_3);
        aboutTextBrowser->setObjectName(QStringLiteral("aboutTextBrowser"));

        gridLayout->addWidget(aboutTextBrowser, 0, 0, 1, 1);

        tabWidget->addTab(tab_3, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        textBrowser = new QTextBrowser(tab);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));

        verticalLayout_2->addWidget(textBrowser);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        widget_2 = new QWidget(tab_2);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        verticalLayout_3 = new QVBoxLayout(widget_2);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label = new QLabel(widget_2);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout_3->addWidget(label);

        version = new QLineEdit(widget_2);
        version->setObjectName(QStringLiteral("version"));
        version->setReadOnly(true);

        verticalLayout_3->addWidget(version);

        label_3 = new QLabel(widget_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout_3->addWidget(label_3);

        lastModified = new QLineEdit(widget_2);
        lastModified->setObjectName(QStringLiteral("lastModified"));
        lastModified->setReadOnly(true);

        verticalLayout_3->addWidget(lastModified);


        verticalLayout_4->addWidget(widget_2);

        tableWidget = new QTableWidget(tab_2);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout_4->addWidget(tableWidget);

        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        widget = new QWidget(CoreGUI__AboutDialog);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btnCopyEnvironmentAndVersion = new QPushButton(widget);
        btnCopyEnvironmentAndVersion->setObjectName(QStringLiteral("btnCopyEnvironmentAndVersion"));

        horizontalLayout->addWidget(btnCopyEnvironmentAndVersion);

        horizontalSpacer = new QSpacerItem(283, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnClose = new QPushButton(widget);
        btnClose->setObjectName(QStringLiteral("btnClose"));

        horizontalLayout->addWidget(btnClose);


        verticalLayout->addWidget(widget);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(version);
        label_3->setBuddy(lastModified);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(btnClose, btnCopyEnvironmentAndVersion);
        QWidget::setTabOrder(btnCopyEnvironmentAndVersion, tabWidget);
        QWidget::setTabOrder(tabWidget, textBrowser);
        QWidget::setTabOrder(textBrowser, tableWidget);

        retranslateUi(CoreGUI__AboutDialog);
        QObject::connect(btnClose, SIGNAL(clicked()), CoreGUI__AboutDialog, SLOT(accept()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(CoreGUI__AboutDialog);
    } // setupUi

    void retranslateUi(QDialog *CoreGUI__AboutDialog)
    {
        CoreGUI__AboutDialog->setWindowTitle(QApplication::translate("CoreGUI::AboutDialog", "About Kumir", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("CoreGUI::AboutDialog", "About", 0));
        textBrowser->setHtml(QApplication::translate("CoreGUI::AboutDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Droid Sans'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic;\">\320\227\320\260 \320\277\321\200\320\265\320\264\320\265\320\273\320\260\320\274\320\270 \320\240\320\276\321\201\321\201\320\270\320\271\321\201\320\272\320\276\320\271 \320\244\320\265\320\264\320\265\321\200\320\260\321\206\320\270\320\270 \320\264\320\260\320\275\320\275\320\260\321\217 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\260 \320\274\320\276\320\266\320\265\321\202 \321\201\320\262\320\276\320\261\320\276\320\264\320\275\320\276 \321\200\320\260\321\201\320\277\321\200\320"
                        "\276\321\201\321\202\321\200\320\260\320\275\321\217\321\202\321\214\321\201\321\217 \320\277\320\276 \320\273\320\270\321\206\320\265\320\275\321\206\320\270\320\270 GNU GPL v2.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt; font-style:italic;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-style:italic;\">\320\235\320\260 \321\202\320\265\321\200\321\200\320\270\321\202\320\276\321\200\320\270\320\270 \320\240\320\276\321\201\321\201\320\270\320\271\321\201\320\272\320\276\320\271 \320\244\320\265\320\264\320\265\321\200\320\260\321\206\320\270\320\270 \320\264\320\260\320\275\320\275\320\260\321\217 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\260 \321\200\320\260\321\201\320\277\321\200\320\276\321\201\321\202\321\200\320\260\320\275"
                        "\321\217\320\265\321\202\321\201\321\217 \320\262 \321\201\320\276\320\276\321\202\320\262\320\265\321\202\321\201\321\202\320\262\320\270\320\270 \321\201 \320\264\320\260\320\275\320\275\321\213\320\274 \320\233\320\270\321\206\320\265\320\275\320\267\320\270\320\276\320\275\320\275\321\213\320\274 \321\201\320\276\320\263\320\273\320\260\321\210\320\265\320\275\320\270\320\265\320\274.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">\320\233\320\230\320\246\320\225\320\235\320\227\320\230\320\236\320\235\320\235\320\236\320\225"
                        " \320\241\320\236\320\223\320\233\320\220\320\250\320\225\320\235\320\230\320\225</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">1. \320\243\321\201\321\202\320\260\320\275\320\260\320\262\320\273\320\270\320\262\320\260\321\217 \320\264\320\260\320\275\320\275\321\213\320\271 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\275\321\213\320\271 \320\277\321\200\320\276\320\264\321\203\320\272\321\202, \320\222\321\213 \320\260\320\262\321\202\320\276\320\274\320\260\321\202\320\270\321\207\320\265\321\201\320\272\320\270 \320\277\321\200\320\270\320\275\320\270\320\274\320\260\320\265\321\202\320\265 \321\203\321\201\320\273\320\276\320\262\320\270\321\217 \320\264\320\260\320\275\320\275\320\276\320"
                        "\263\320\276 \320\273\320\270\321\206\320\265\320\275\320\267\320\270\320\276\320\275\320\275\320\276\320\263\320\276 \321\201\320\276\320\263\320\273\320\260\321\210\320\265\320\275\320\270\321\217.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">2. \320\222\321\213 \320\274\320\276\320\266\320\265\321\202\320\265 \321\203\321\201\321\202\320\260\320\275\320\260\320\262\320\273\320\270\320\262\320\260\321\202\321\214 \320\264\320\260\320\275\320\275\321\203\321\216 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\321\203 \320\275\320\260 \320\273\321\216\320\261\320\276\320\265 \321\207\320\270\321\201\320\273\320\276 \320\272\320\276\320\274\320\277\321\214\321\216\321\202\320\265\321\200\320\276\320"
                        "\262 \320\275\320\265\320\276\320\263\321\200\320\260\320\275\320\270\321\207\320\265\320\275\320\275\320\276\320\265 \321\207\320\270\321\201\320\273\320\276 \321\200\320\260\320\267.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">3. \320\222\321\213 \320\274\320\276\320\266\320\265\321\202\320\265 \320\264\320\265\320\273\320\260\321\202\321\214  \320\275\320\265\320\276\320\263\321\200\320\260\320\275\320\270\321\207\320\265\320\275\320\275\320\276\320\265 \321\207\320\270\321\201\320\273\320\276 \320\272\320\276\320\277\320\270\320\271 \320\264\320\260\320\275\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320"
                        "\264\321\203\320\272\321\202\320\260.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">4. \320\222\321\213 \320\274\320\276\320\266\320\265\321\202\320\265 \320\277\320\265\321\200\320\265\320\264\320\260\320\262\320\260\321\202\321\214 \320\272\320\276\320\277\320\270\320\270 \320\264\320\260\320\275\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\264\321\203\320\272\321\202\320\260 \320\262\320\276\320\267\320\274\320\265\320\267\320\264\320\275\320\276 \320\270\320\273\320\270 \320\261\320\265\320\267\320\262\320\276\320\267\320\274\320\265\320\267\320\264\320\275\320\276 \320\275\320\265\320\276\320\263\321"
                        "\200\320\260\320\275\320\270\321\207\320\265\320\275\320\275\320\276\320\274\321\203 \321\207\320\270\321\201\320\273\321\203 \321\202\321\200\320\265\321\202\321\214\320\270\321\205 \320\273\320\270\321\206.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">5. \320\222\321\213 \320\270\320\274\320\265\320\265\321\202\320\265 \320\277\321\200\320\260\320\262\320\276 \320\267\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214 \321\201 \321\201\320\260\320\271\321\202\320\260 \321\200\320\260\320\267\321\200\320\260\320\261\320\276\321\202\321\207\320\270\320\272\320\260 \320\270\321\201\321\205\320\276\320\264\320\275\321\213\320\265 \321\202\320\265\320\272\321\201\321\202\321\213 \320\264\320\260\320\275\320"
                        "\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\264\321\203\320\272\321\202\320\260 \320\270 \320\270\321\201\320\277\320\276\320\273\321\214\320\267\320\276\320\262\320\260\321\202\321\214 \320\270\321\205 \320\262 \321\201\320\276\320\276\321\202\320\262\320\265\321\202\321\201\321\202\320\262\320\270\320\270 \321\201 \321\202\321\200\320\265\320\261\320\276\320\262\320\260\320\275\320\270\321\217\320\274\320\270 \320\273\320\270\321\206\320\265\320\275\320\267\320\270\320\270 GNU GPL v2, \321\202\320\265\320\272\321\201\321\202 \320\272\320\276\321\202\320\276\321\200\320\276\320\271 \320\275\320\260 \320\260\320\275\320\263\320\273\320\270\320\271\321\201\320\272\320\276\320\274 \321\217\320\267\321\213\320\272\320\265 \320\277\321\200\320\270\320\273\320\260\320\263\320\260\320\265\321\202\321\201\321\217 \320\262 \321\204\320\260\320\271\320\273\320\265 license.gpl.</span></p>\n"
"<p style=\"-qt-par"
                        "agraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt;\">6. \320\240\320\260\320\267\321\200\320\260\320\261\320\276\321\202\321\207\320\270\320\272 \320\275\320\265 \320\264\320\260\320\265\321\202 \320\275\320\270\320\272\320\260\320\272\320\270\321\205 \320\263\320\260\321\200\320\260\320\275\321\202\320\270\320\271 \321\200\320\260\320\261\320\276\321\202\320\276\321\201\320\277\320\276\320\261\320\276\321\201\320\275\320\276\321\201\321\202\320\270 \320\264\320\260\320\275\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\264\321\203\320\272\321\202\320\260 \320\270 \320\275\320\265 \320\275\320\265\321\201\320\265\321\202 \320\276\321\202\320\262\320\265\321\202\321\201\321\202\320\262\320\265\320\275\320\275\320\276\321\201\321\202\320\270 \320"
                        "\267\320\260 \320\273\321\216\320\261\320\276\320\271 \321\203\321\211\320\265\321\200\320\261, \320\277\321\200\320\270\321\207\320\270\320\275\320\265\320\275\320\275\321\213\320\271 \320\262\321\201\320\273\320\265\320\264\321\201\321\202\320\262\320\270\320\270 \321\203\321\201\321\202\320\260\320\275\320\276\320\262\320\272\320\270 \320\270\320\273\320\270 \320\267\320\260\320\277\321\203\321\201\320\272\320\260 \320\264\320\260\320\275\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\263\321\200\320\260\320\274\320\274\320\275\320\276\320\263\320\276 \320\277\321\200\320\276\320\264\321\203\320\272\321\202\320\260.</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p></body></html>", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("CoreGUI::AboutDialog", "Licensee", 0));
        label->setText(QApplication::translate("CoreGUI::AboutDialog", "Version:", 0));
        version->setText(QApplication::translate("CoreGUI::AboutDialog", "unknown", 0));
        label_3->setText(QApplication::translate("CoreGUI::AboutDialog", "Last modified:", 0));
        lastModified->setText(QApplication::translate("CoreGUI::AboutDialog", "unknown", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("CoreGUI::AboutDialog", "Parameter", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("CoreGUI::AboutDialog", "Value", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("CoreGUI::AboutDialog", "System Information", 0));
        btnCopyEnvironmentAndVersion->setText(QApplication::translate("CoreGUI::AboutDialog", "Copy system information to clipboard", 0));
        btnClose->setText(QApplication::translate("CoreGUI::AboutDialog", "Close", 0));
        btnClose->setShortcut(QApplication::translate("CoreGUI::AboutDialog", "Esc", 0));
    } // retranslateUi

};

} // namespace CoreGUI

namespace CoreGUI {
namespace Ui {
    class AboutDialog: public Ui_AboutDialog {};
} // namespace Ui
} // namespace CoreGUI

#endif // UI_ABOUTDIALOG_H
