/********************************************************************************
** Form generated from reading UI file 'switchworkspacedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SWITCHWORKSPACEDIALOG_H
#define UI_SWITCHWORKSPACEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>

namespace CoreGUI {

class Ui_SwitchWorkspaceDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QCheckBox *checkBox;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *CoreGUI__SwitchWorkspaceDialog)
    {
        if (CoreGUI__SwitchWorkspaceDialog->objectName().isEmpty())
            CoreGUI__SwitchWorkspaceDialog->setObjectName(QStringLiteral("CoreGUI__SwitchWorkspaceDialog"));
        CoreGUI__SwitchWorkspaceDialog->resize(699, 285);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CoreGUI__SwitchWorkspaceDialog->sizePolicy().hasHeightForWidth());
        CoreGUI__SwitchWorkspaceDialog->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(CoreGUI__SwitchWorkspaceDialog);
        verticalLayout->setSpacing(3);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        textBrowser = new QTextBrowser(CoreGUI__SwitchWorkspaceDialog);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textBrowser->sizePolicy().hasHeightForWidth());
        textBrowser->setSizePolicy(sizePolicy1);
        textBrowser->setMaximumSize(QSize(16777215, 120));
        textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        verticalLayout->addWidget(textBrowser);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(CoreGUI__SwitchWorkspaceDialog);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        comboBox = new QComboBox(CoreGUI__SwitchWorkspaceDialog);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(comboBox->sizePolicy().hasHeightForWidth());
        comboBox->setSizePolicy(sizePolicy2);
        comboBox->setEditable(true);

        horizontalLayout->addWidget(comboBox);

        pushButton = new QPushButton(CoreGUI__SwitchWorkspaceDialog);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);


        verticalLayout->addLayout(horizontalLayout);

        checkBox = new QCheckBox(CoreGUI__SwitchWorkspaceDialog);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout->addWidget(checkBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        buttonBox = new QDialogButtonBox(CoreGUI__SwitchWorkspaceDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Ok);

        horizontalLayout_2->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout_2);

        QWidget::setTabOrder(comboBox, pushButton);
        QWidget::setTabOrder(pushButton, checkBox);
        QWidget::setTabOrder(checkBox, buttonBox);
        QWidget::setTabOrder(buttonBox, textBrowser);

        retranslateUi(CoreGUI__SwitchWorkspaceDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), CoreGUI__SwitchWorkspaceDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CoreGUI__SwitchWorkspaceDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(CoreGUI__SwitchWorkspaceDialog);
    } // setupUi

    void retranslateUi(QDialog *CoreGUI__SwitchWorkspaceDialog)
    {
        CoreGUI__SwitchWorkspaceDialog->setWindowTitle(QApplication::translate("CoreGUI::SwitchWorkspaceDialog", "Workspace Launcher", 0));
        textBrowser->setHtml(QApplication::translate("CoreGUI::SwitchWorkspaceDialog", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Droid Sans'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0));
        label->setText(QApplication::translate("CoreGUI::SwitchWorkspaceDialog", "Workspace:", 0));
        pushButton->setText(QApplication::translate("CoreGUI::SwitchWorkspaceDialog", "Browse...", 0));
        checkBox->setText(QApplication::translate("CoreGUI::SwitchWorkspaceDialog", "Use this as the default and do not ask again", 0));
    } // retranslateUi

};

} // namespace CoreGUI

namespace CoreGUI {
namespace Ui {
    class SwitchWorkspaceDialog: public Ui_SwitchWorkspaceDialog {};
} // namespace Ui
} // namespace CoreGUI

#endif // UI_SWITCHWORKSPACEDIALOG_H
