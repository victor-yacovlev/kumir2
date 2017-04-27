/********************************************************************************
** Form generated from reading UI file 'iosettingseditorpage.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IOSETTINGSEDITORPAGE_H
#define UI_IOSETTINGSEDITORPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace CoreGUI {

class Ui_IOSettingsEditorPage
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *useFixedWidth;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QSpinBox *widthInCharacters;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *CoreGUI__IOSettingsEditorPage)
    {
        if (CoreGUI__IOSettingsEditorPage->objectName().isEmpty())
            CoreGUI__IOSettingsEditorPage->setObjectName(QStringLiteral("CoreGUI__IOSettingsEditorPage"));
        CoreGUI__IOSettingsEditorPage->resize(400, 300);
        verticalLayout = new QVBoxLayout(CoreGUI__IOSettingsEditorPage);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(CoreGUI__IOSettingsEditorPage);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        useFixedWidth = new QCheckBox(groupBox);
        useFixedWidth->setObjectName(QStringLiteral("useFixedWidth"));

        verticalLayout_2->addWidget(useFixedWidth);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setEnabled(false);

        horizontalLayout->addWidget(label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        widthInCharacters = new QSpinBox(groupBox);
        widthInCharacters->setObjectName(QStringLiteral("widthInCharacters"));
        widthInCharacters->setEnabled(false);
        widthInCharacters->setMinimum(1);
        widthInCharacters->setMaximum(1000);
        widthInCharacters->setValue(80);

        horizontalLayout->addWidget(widthInCharacters);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 184, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(widthInCharacters);
#endif // QT_NO_SHORTCUT

        retranslateUi(CoreGUI__IOSettingsEditorPage);
        QObject::connect(useFixedWidth, SIGNAL(toggled(bool)), label, SLOT(setEnabled(bool)));
        QObject::connect(useFixedWidth, SIGNAL(toggled(bool)), widthInCharacters, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(CoreGUI__IOSettingsEditorPage);
    } // setupUi

    void retranslateUi(QWidget *CoreGUI__IOSettingsEditorPage)
    {
        CoreGUI__IOSettingsEditorPage->setWindowTitle(QApplication::translate("CoreGUI::IOSettingsEditorPage", "Input/Output", 0));
        groupBox->setTitle(QApplication::translate("CoreGUI::IOSettingsEditorPage", "Terminal width", 0));
        useFixedWidth->setText(QApplication::translate("CoreGUI::IOSettingsEditorPage", "Use fixed width for new launch sessions", 0));
        label->setText(QApplication::translate("CoreGUI::IOSettingsEditorPage", "Width in characters:", 0));
    } // retranslateUi

};

} // namespace CoreGUI

namespace CoreGUI {
namespace Ui {
    class IOSettingsEditorPage: public Ui_IOSettingsEditorPage {};
} // namespace Ui
} // namespace CoreGUI

#endif // UI_IOSETTINGSEDITORPAGE_H
