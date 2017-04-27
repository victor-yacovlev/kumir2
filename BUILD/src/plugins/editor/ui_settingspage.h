/********************************************************************************
** Form generated from reading UI file 'settingspage.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSPAGE_H
#define UI_SETTINGSPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace Editor {

class Ui_SettingsPage
{
public:
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupKeyboard;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_12;
    QSpacerItem *horizontalSpacer_9;
    QComboBox *layoutSwitchKey;
    QGroupBox *groupFont;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QFontComboBox *fontFamily;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QSpinBox *fontSize;
    QLabel *label_3;
    QPlainTextEdit *fontPreview;
    QGroupBox *groupSyntax;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *invertColors;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QSpacerItem *horizontalSpacer;
    QToolButton *kwColor;
    QCheckBox *kwBold;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *typeColor;
    QCheckBox *typeBold;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_3;
    QToolButton *numericColor;
    QCheckBox *numericBold;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QSpacerItem *horizontalSpacer_4;
    QToolButton *literalColor;
    QCheckBox *literalBold;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer_5;
    QToolButton *moduleColor;
    QCheckBox *moduleBold;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_9;
    QSpacerItem *horizontalSpacer_6;
    QToolButton *algorhitmColor;
    QCheckBox *algorhitmBold;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_10;
    QSpacerItem *horizontalSpacer_7;
    QToolButton *docColor;
    QCheckBox *docBold;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_11;
    QSpacerItem *horizontalSpacer_8;
    QToolButton *commentColor;
    QCheckBox *commentBold;
    QGroupBox *groupInitial;
    QGridLayout *gridLayout;
    QToolButton *btnBrowseTemplateFile;
    QLineEdit *templateFileName;
    QLabel *fileNotExistsLabel;
    QLabel *label_14;
    QGroupBox *groupOther;
    QVBoxLayout *_2;
    QCheckBox *pressTextLeft;
    QCheckBox *showTrailingSpaces;
    QCheckBox *autoInsertClosingOperationalBrackets;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_15;
    QComboBox *freeCursorPositioning;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *Editor__SettingsPage)
    {
        if (Editor__SettingsPage->objectName().isEmpty())
            Editor__SettingsPage->setObjectName(QStringLiteral("Editor__SettingsPage"));
        Editor__SettingsPage->resize(406, 914);
        verticalLayout_3 = new QVBoxLayout(Editor__SettingsPage);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        groupKeyboard = new QGroupBox(Editor__SettingsPage);
        groupKeyboard->setObjectName(QStringLiteral("groupKeyboard"));
        verticalLayout_4 = new QVBoxLayout(groupKeyboard);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_12 = new QLabel(groupKeyboard);
        label_12->setObjectName(QStringLiteral("label_12"));

        horizontalLayout_11->addWidget(label_12);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_9);

        layoutSwitchKey = new QComboBox(groupKeyboard);
        layoutSwitchKey->insertItems(0, QStringList()
         << QStringLiteral("Alt")
         << QStringLiteral("AltGr")
         << QStringLiteral("Meta")
         << QStringLiteral("Menu")
         << QStringLiteral("ScrollLock")
         << QStringLiteral("Pause")
        );
        layoutSwitchKey->setObjectName(QStringLiteral("layoutSwitchKey"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(layoutSwitchKey->sizePolicy().hasHeightForWidth());
        layoutSwitchKey->setSizePolicy(sizePolicy);
        layoutSwitchKey->setMinimumSize(QSize(80, 0));
        layoutSwitchKey->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_11->addWidget(layoutSwitchKey);


        verticalLayout_4->addLayout(horizontalLayout_11);


        verticalLayout_3->addWidget(groupKeyboard);

        groupFont = new QGroupBox(Editor__SettingsPage);
        groupFont->setObjectName(QStringLiteral("groupFont"));
        verticalLayout = new QVBoxLayout(groupFont);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(groupFont);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        fontFamily = new QFontComboBox(groupFont);
        fontFamily->setObjectName(QStringLiteral("fontFamily"));
        fontFamily->setWritingSystem(QFontDatabase::Cyrillic);
        fontFamily->setFontFilters(QFontComboBox::MonospacedFonts|QFontComboBox::ScalableFonts);

        horizontalLayout_2->addWidget(fontFamily);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(groupFont);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        fontSize = new QSpinBox(groupFont);
        fontSize->setObjectName(QStringLiteral("fontSize"));
        fontSize->setMinimum(8);
        fontSize->setMaximum(48);
        fontSize->setValue(12);

        horizontalLayout->addWidget(fontSize);


        verticalLayout->addLayout(horizontalLayout);

        label_3 = new QLabel(groupFont);
        label_3->setObjectName(QStringLiteral("label_3"));

        verticalLayout->addWidget(label_3);

        fontPreview = new QPlainTextEdit(groupFont);
        fontPreview->setObjectName(QStringLiteral("fontPreview"));

        verticalLayout->addWidget(fontPreview);


        verticalLayout_3->addWidget(groupFont);

        groupSyntax = new QGroupBox(Editor__SettingsPage);
        groupSyntax->setObjectName(QStringLiteral("groupSyntax"));
        verticalLayout_2 = new QVBoxLayout(groupSyntax);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        invertColors = new QCheckBox(groupSyntax);
        invertColors->setObjectName(QStringLiteral("invertColors"));

        verticalLayout_2->addWidget(invertColors);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_4 = new QLabel(groupSyntax);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_3->addWidget(label_4);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        kwColor = new QToolButton(groupSyntax);
        kwColor->setObjectName(QStringLiteral("kwColor"));

        horizontalLayout_3->addWidget(kwColor);

        kwBold = new QCheckBox(groupSyntax);
        kwBold->setObjectName(QStringLiteral("kwBold"));

        horizontalLayout_3->addWidget(kwBold);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_5 = new QLabel(groupSyntax);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_4->addWidget(label_5);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        typeColor = new QToolButton(groupSyntax);
        typeColor->setObjectName(QStringLiteral("typeColor"));

        horizontalLayout_4->addWidget(typeColor);

        typeBold = new QCheckBox(groupSyntax);
        typeBold->setObjectName(QStringLiteral("typeBold"));

        horizontalLayout_4->addWidget(typeBold);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_6 = new QLabel(groupSyntax);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_5->addWidget(label_6);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        numericColor = new QToolButton(groupSyntax);
        numericColor->setObjectName(QStringLiteral("numericColor"));

        horizontalLayout_5->addWidget(numericColor);

        numericBold = new QCheckBox(groupSyntax);
        numericBold->setObjectName(QStringLiteral("numericBold"));

        horizontalLayout_5->addWidget(numericBold);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_7 = new QLabel(groupSyntax);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_6->addWidget(label_7);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        literalColor = new QToolButton(groupSyntax);
        literalColor->setObjectName(QStringLiteral("literalColor"));

        horizontalLayout_6->addWidget(literalColor);

        literalBold = new QCheckBox(groupSyntax);
        literalBold->setObjectName(QStringLiteral("literalBold"));

        horizontalLayout_6->addWidget(literalBold);


        verticalLayout_2->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_8 = new QLabel(groupSyntax);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_7->addWidget(label_8);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_5);

        moduleColor = new QToolButton(groupSyntax);
        moduleColor->setObjectName(QStringLiteral("moduleColor"));

        horizontalLayout_7->addWidget(moduleColor);

        moduleBold = new QCheckBox(groupSyntax);
        moduleBold->setObjectName(QStringLiteral("moduleBold"));

        horizontalLayout_7->addWidget(moduleBold);


        verticalLayout_2->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_9 = new QLabel(groupSyntax);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_8->addWidget(label_9);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_6);

        algorhitmColor = new QToolButton(groupSyntax);
        algorhitmColor->setObjectName(QStringLiteral("algorhitmColor"));

        horizontalLayout_8->addWidget(algorhitmColor);

        algorhitmBold = new QCheckBox(groupSyntax);
        algorhitmBold->setObjectName(QStringLiteral("algorhitmBold"));

        horizontalLayout_8->addWidget(algorhitmBold);


        verticalLayout_2->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_10 = new QLabel(groupSyntax);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_9->addWidget(label_10);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_7);

        docColor = new QToolButton(groupSyntax);
        docColor->setObjectName(QStringLiteral("docColor"));

        horizontalLayout_9->addWidget(docColor);

        docBold = new QCheckBox(groupSyntax);
        docBold->setObjectName(QStringLiteral("docBold"));

        horizontalLayout_9->addWidget(docBold);


        verticalLayout_2->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        label_11 = new QLabel(groupSyntax);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_10->addWidget(label_11);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_8);

        commentColor = new QToolButton(groupSyntax);
        commentColor->setObjectName(QStringLiteral("commentColor"));

        horizontalLayout_10->addWidget(commentColor);

        commentBold = new QCheckBox(groupSyntax);
        commentBold->setObjectName(QStringLiteral("commentBold"));

        horizontalLayout_10->addWidget(commentBold);


        verticalLayout_2->addLayout(horizontalLayout_10);


        verticalLayout_3->addWidget(groupSyntax);

        groupInitial = new QGroupBox(Editor__SettingsPage);
        groupInitial->setObjectName(QStringLiteral("groupInitial"));
        gridLayout = new QGridLayout(groupInitial);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        btnBrowseTemplateFile = new QToolButton(groupInitial);
        btnBrowseTemplateFile->setObjectName(QStringLiteral("btnBrowseTemplateFile"));

        gridLayout->addWidget(btnBrowseTemplateFile, 1, 1, 1, 1);

        templateFileName = new QLineEdit(groupInitial);
        templateFileName->setObjectName(QStringLiteral("templateFileName"));

        gridLayout->addWidget(templateFileName, 1, 0, 1, 1);

        fileNotExistsLabel = new QLabel(groupInitial);
        fileNotExistsLabel->setObjectName(QStringLiteral("fileNotExistsLabel"));
        QPalette palette;
        QBrush brush(QColor(192, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(87, 89, 97, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        fileNotExistsLabel->setPalette(palette);
        QFont font;
        font.setItalic(true);
        fileNotExistsLabel->setFont(font);
        fileNotExistsLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(fileNotExistsLabel, 2, 0, 1, 2);

        label_14 = new QLabel(groupInitial);
        label_14->setObjectName(QStringLiteral("label_14"));

        gridLayout->addWidget(label_14, 0, 0, 1, 2);


        verticalLayout_3->addWidget(groupInitial);

        groupOther = new QGroupBox(Editor__SettingsPage);
        groupOther->setObjectName(QStringLiteral("groupOther"));
        _2 = new QVBoxLayout(groupOther);
        _2->setObjectName(QStringLiteral("_2"));
        pressTextLeft = new QCheckBox(groupOther);
        pressTextLeft->setObjectName(QStringLiteral("pressTextLeft"));

        _2->addWidget(pressTextLeft);

        showTrailingSpaces = new QCheckBox(groupOther);
        showTrailingSpaces->setObjectName(QStringLiteral("showTrailingSpaces"));

        _2->addWidget(showTrailingSpaces);

        autoInsertClosingOperationalBrackets = new QCheckBox(groupOther);
        autoInsertClosingOperationalBrackets->setObjectName(QStringLiteral("autoInsertClosingOperationalBrackets"));

        _2->addWidget(autoInsertClosingOperationalBrackets);

        widget = new QWidget(groupOther);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout_13 = new QHBoxLayout(widget);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        label_15 = new QLabel(widget);
        label_15->setObjectName(QStringLiteral("label_15"));

        horizontalLayout_13->addWidget(label_15);

        freeCursorPositioning = new QComboBox(widget);
        freeCursorPositioning->setObjectName(QStringLiteral("freeCursorPositioning"));

        horizontalLayout_13->addWidget(freeCursorPositioning);


        _2->addWidget(widget);


        verticalLayout_3->addWidget(groupOther);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

#ifndef QT_NO_SHORTCUT
        label_14->setBuddy(templateFileName);
#endif // QT_NO_SHORTCUT

        retranslateUi(Editor__SettingsPage);

        QMetaObject::connectSlotsByName(Editor__SettingsPage);
    } // setupUi

    void retranslateUi(QWidget *Editor__SettingsPage)
    {
        Editor__SettingsPage->setWindowTitle(QApplication::translate("Editor::SettingsPage", "Form", 0));
        groupKeyboard->setTitle(QApplication::translate("Editor::SettingsPage", "Keyboard", 0));
        label_12->setText(QApplication::translate("Editor::SettingsPage", "Key to temporary switch layout:", 0));
        groupFont->setTitle(QApplication::translate("Editor::SettingsPage", "Font", 0));
        label->setText(QApplication::translate("Editor::SettingsPage", "Family:", 0));
        label_2->setText(QApplication::translate("Editor::SettingsPage", "Size:", 0));
        label_3->setText(QApplication::translate("Editor::SettingsPage", "Preview:", 0));
        fontPreview->setPlainText(QApplication::translate("Editor::SettingsPage", "The keywords of Kumir are: alg, begin, end and others", 0));
        groupSyntax->setTitle(QApplication::translate("Editor::SettingsPage", "Syntax highlighting", 0));
        invertColors->setText(QApplication::translate("Editor::SettingsPage", "Invert colors if system color scheme has dark background", 0));
        label_4->setText(QApplication::translate("Editor::SettingsPage", "Keywords:", 0));
        kwColor->setText(QString());
        kwBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        label_5->setText(QApplication::translate("Editor::SettingsPage", "Type names:", 0));
        typeColor->setText(QString());
        typeBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        label_6->setText(QApplication::translate("Editor::SettingsPage", "Numeric constants:", 0));
        numericColor->setText(QString());
        numericBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        label_7->setText(QApplication::translate("Editor::SettingsPage", "Literal constants::", 0));
        literalColor->setText(QString());
        literalBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        label_8->setText(QApplication::translate("Editor::SettingsPage", "Module names:", 0));
        moduleColor->setText(QString());
        moduleBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        label_9->setText(QApplication::translate("Editor::SettingsPage", "Algorhitm names:", 0));
        algorhitmColor->setText(QString());
        algorhitmBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        label_10->setText(QApplication::translate("Editor::SettingsPage", "Algorhitm descriptions after #:", 0));
        docColor->setText(QString());
        docBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        label_11->setText(QApplication::translate("Editor::SettingsPage", "Comments:", 0));
        commentColor->setText(QString());
        commentBold->setText(QApplication::translate("Editor::SettingsPage", "Bold", 0));
        groupInitial->setTitle(QApplication::translate("Editor::SettingsPage", "Initial Text", 0));
        btnBrowseTemplateFile->setText(QApplication::translate("Editor::SettingsPage", "Browse...", 0));
        fileNotExistsLabel->setText(QString());
        label_14->setText(QApplication::translate("Editor::SettingsPage", "Initial program template file:", 0));
        groupOther->setTitle(QApplication::translate("Editor::SettingsPage", "Other", 0));
        pressTextLeft->setText(QApplication::translate("Editor::SettingsPage", "Force press program text to the left", 0));
        showTrailingSpaces->setText(QApplication::translate("Editor::SettingsPage", "Show trailing spaces", 0));
        autoInsertClosingOperationalBrackets->setText(QApplication::translate("Editor::SettingsPage", "Auto insert pairing operational brackets", 0));
        label_15->setText(QApplication::translate("Editor::SettingsPage", "Free cursor positioning:", 0));
        freeCursorPositioning->clear();
        freeCursorPositioning->insertItems(0, QStringList()
         << QApplication::translate("Editor::SettingsPage", "Texts and comments", 0)
         << QApplication::translate("Editor::SettingsPage", "Always", 0)
         << QApplication::translate("Editor::SettingsPage", "Texts only", 0)
        );
    } // retranslateUi

};

} // namespace Editor

namespace Editor {
namespace Ui {
    class SettingsPage: public Ui_SettingsPage {};
} // namespace Ui
} // namespace Editor

#endif // UI_SETTINGSPAGE_H
