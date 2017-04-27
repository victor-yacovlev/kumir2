/********************************************************************************
** Form generated from reading UI file 'guisettingspage.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUISETTINGSPAGE_H
#define UI_GUISETTINGSPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

namespace CoreGUI {

class Ui_GUISettingsPage
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QLabel *lblRowsFirst;
    QLabel *lblColumnsFirst;
    QRadioButton *btnRowsFirst;
    QRadioButton *btnColumnsFirst;
    QSpacerItem *verticalSpacer;
    QGroupBox *visibleIconsGrid;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout;
    QCheckBox *userSystemFontSize;
    QLabel *label;
    QSpinBox *overrideFontSize;
    QGroupBox *groupBox_3;
    QFormLayout *formLayout_2;
    QLabel *label_2;
    QSpinBox *presentationModeMainFontSize;
    QLabel *label_3;
    QSpinBox *presentationModeEditorFontSize;

    void setupUi(QWidget *CoreGUI__GUISettingsPage)
    {
        if (CoreGUI__GUISettingsPage->objectName().isEmpty())
            CoreGUI__GUISettingsPage->setObjectName(QStringLiteral("CoreGUI__GUISettingsPage"));
        CoreGUI__GUISettingsPage->resize(392, 601);
        gridLayout = new QGridLayout(CoreGUI__GUISettingsPage);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox = new QGroupBox(CoreGUI__GUISettingsPage);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(10);
        lblRowsFirst = new QLabel(groupBox);
        lblRowsFirst->setObjectName(QStringLiteral("lblRowsFirst"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblRowsFirst->sizePolicy().hasHeightForWidth());
        lblRowsFirst->setSizePolicy(sizePolicy);
        lblRowsFirst->setMinimumSize(QSize(200, 155));

        gridLayout_2->addWidget(lblRowsFirst, 0, 0, 1, 1);

        lblColumnsFirst = new QLabel(groupBox);
        lblColumnsFirst->setObjectName(QStringLiteral("lblColumnsFirst"));

        gridLayout_2->addWidget(lblColumnsFirst, 0, 1, 1, 1);

        btnRowsFirst = new QRadioButton(groupBox);
        btnRowsFirst->setObjectName(QStringLiteral("btnRowsFirst"));

        gridLayout_2->addWidget(btnRowsFirst, 1, 0, 1, 1);

        btnColumnsFirst = new QRadioButton(groupBox);
        btnColumnsFirst->setObjectName(QStringLiteral("btnColumnsFirst"));

        gridLayout_2->addWidget(btnColumnsFirst, 1, 1, 1, 1);


        gridLayout->addWidget(groupBox, 2, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 5, 0, 1, 1);

        visibleIconsGrid = new QGroupBox(CoreGUI__GUISettingsPage);
        visibleIconsGrid->setObjectName(QStringLiteral("visibleIconsGrid"));

        gridLayout->addWidget(visibleIconsGrid, 3, 0, 1, 1);

        groupBox_2 = new QGroupBox(CoreGUI__GUISettingsPage);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        formLayout = new QFormLayout(groupBox_2);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        userSystemFontSize = new QCheckBox(groupBox_2);
        userSystemFontSize->setObjectName(QStringLiteral("userSystemFontSize"));

        formLayout->setWidget(0, QFormLayout::LabelRole, userSystemFontSize);

        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);

        overrideFontSize = new QSpinBox(groupBox_2);
        overrideFontSize->setObjectName(QStringLiteral("overrideFontSize"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(overrideFontSize->sizePolicy().hasHeightForWidth());
        overrideFontSize->setSizePolicy(sizePolicy2);
        overrideFontSize->setMinimumSize(QSize(25, 0));
        overrideFontSize->setMinimum(8);
        overrideFontSize->setMaximum(24);

        formLayout->setWidget(1, QFormLayout::FieldRole, overrideFontSize);


        gridLayout->addWidget(groupBox_2, 0, 0, 1, 1);

        groupBox_3 = new QGroupBox(CoreGUI__GUISettingsPage);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        formLayout_2 = new QFormLayout(groupBox_3);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy3);

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_2);

        presentationModeMainFontSize = new QSpinBox(groupBox_3);
        presentationModeMainFontSize->setObjectName(QStringLiteral("presentationModeMainFontSize"));
        sizePolicy2.setHeightForWidth(presentationModeMainFontSize->sizePolicy().hasHeightForWidth());
        presentationModeMainFontSize->setSizePolicy(sizePolicy2);
        presentationModeMainFontSize->setMinimum(8);
        presentationModeMainFontSize->setMaximum(24);
        presentationModeMainFontSize->setValue(14);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, presentationModeMainFontSize);

        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_3);

        presentationModeEditorFontSize = new QSpinBox(groupBox_3);
        presentationModeEditorFontSize->setObjectName(QStringLiteral("presentationModeEditorFontSize"));
        sizePolicy2.setHeightForWidth(presentationModeEditorFontSize->sizePolicy().hasHeightForWidth());
        presentationModeEditorFontSize->setSizePolicy(sizePolicy2);
        presentationModeEditorFontSize->setMinimum(8);
        presentationModeEditorFontSize->setMaximum(24);
        presentationModeEditorFontSize->setValue(16);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, presentationModeEditorFontSize);


        gridLayout->addWidget(groupBox_3, 1, 0, 1, 1);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(overrideFontSize);
#endif // QT_NO_SHORTCUT

        retranslateUi(CoreGUI__GUISettingsPage);
        QObject::connect(userSystemFontSize, SIGNAL(toggled(bool)), label, SLOT(setDisabled(bool)));
        QObject::connect(userSystemFontSize, SIGNAL(toggled(bool)), overrideFontSize, SLOT(setDisabled(bool)));

        QMetaObject::connectSlotsByName(CoreGUI__GUISettingsPage);
    } // setupUi

    void retranslateUi(QWidget *CoreGUI__GUISettingsPage)
    {
        CoreGUI__GUISettingsPage->setWindowTitle(QApplication::translate("CoreGUI::GUISettingsPage", "User Interface", 0));
        groupBox->setTitle(QApplication::translate("CoreGUI::GUISettingsPage", "Docking layout", 0));
        lblRowsFirst->setText(QApplication::translate("CoreGUI::GUISettingsPage", "TextLabel", 0));
        lblColumnsFirst->setText(QApplication::translate("CoreGUI::GUISettingsPage", "TextLabel", 0));
        btnRowsFirst->setText(QApplication::translate("CoreGUI::GUISettingsPage", "Rows first", 0));
        btnColumnsFirst->setText(QApplication::translate("CoreGUI::GUISettingsPage", "Co&lumns first", 0));
        visibleIconsGrid->setTitle(QApplication::translate("CoreGUI::GUISettingsPage", "Visible icons in toolbar", 0));
        groupBox_2->setTitle(QApplication::translate("CoreGUI::GUISettingsPage", "Interface font size", 0));
        userSystemFontSize->setText(QApplication::translate("CoreGUI::GUISettingsPage", "Use system settings", 0));
        label->setText(QApplication::translate("CoreGUI::GUISettingsPage", "Override font si&ze:", 0));
        groupBox_3->setTitle(QApplication::translate("CoreGUI::GUISettingsPage", "Presentation mode", 0));
        label_2->setText(QApplication::translate("CoreGUI::GUISettingsPage", "Main font size:", 0));
        label_3->setText(QApplication::translate("CoreGUI::GUISettingsPage", "Editor font size:", 0));
    } // retranslateUi

};

} // namespace CoreGUI

namespace CoreGUI {
namespace Ui {
    class GUISettingsPage: public Ui_GUISettingsPage {};
} // namespace Ui
} // namespace CoreGUI

#endif // UI_GUISETTINGSPAGE_H
