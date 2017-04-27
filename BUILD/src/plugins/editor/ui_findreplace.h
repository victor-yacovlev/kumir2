/********************************************************************************
** Form generated from reading UI file 'findreplace.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDREPLACE_H
#define UI_FINDREPLACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace Editor {

class Ui_FindReplace
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *findContainer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *find;
    QToolButton *btnPrev;
    QToolButton *btnNext;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *btnMore;
    QSpacerItem *horizontalSpacer;
    QToolButton *btnClose;
    QWidget *replaceContainer;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *replace;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btnReplace;
    QPushButton *btnReplaceAll;
    QWidget *optionsContainer;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QComboBox *searchMode;
    QSpacerItem *horizontalSpacer_4;
    QCheckBox *matchCase;

    void setupUi(QWidget *Editor__FindReplace)
    {
        if (Editor__FindReplace->objectName().isEmpty())
            Editor__FindReplace->setObjectName(QStringLiteral("Editor__FindReplace"));
        Editor__FindReplace->resize(492, 120);
        verticalLayout = new QVBoxLayout(Editor__FindReplace);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(4, 4, 4, 4);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        findContainer = new QWidget(Editor__FindReplace);
        findContainer->setObjectName(QStringLiteral("findContainer"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(findContainer->sizePolicy().hasHeightForWidth());
        findContainer->setSizePolicy(sizePolicy);
        findContainer->setMinimumSize(QSize(0, 32));
        horizontalLayout = new QHBoxLayout(findContainer);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(findContainer);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(80, 0));
        label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label);

        find = new QLineEdit(findContainer);
        find->setObjectName(QStringLiteral("find"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(find->sizePolicy().hasHeightForWidth());
        find->setSizePolicy(sizePolicy1);
        find->setMinimumSize(QSize(200, 0));
        find->setBaseSize(QSize(220, 0));

        horizontalLayout->addWidget(find);

        btnPrev = new QToolButton(findContainer);
        btnPrev->setObjectName(QStringLiteral("btnPrev"));
        btnPrev->setText(QStringLiteral("<"));
        btnPrev->setAutoRaise(false);

        horizontalLayout->addWidget(btnPrev);

        btnNext = new QToolButton(findContainer);
        btnNext->setObjectName(QStringLiteral("btnNext"));
        btnNext->setText(QStringLiteral(">"));
        btnNext->setAutoRaise(false);

        horizontalLayout->addWidget(btnNext);

        horizontalSpacer_2 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        btnMore = new QToolButton(findContainer);
        btnMore->setObjectName(QStringLiteral("btnMore"));
        btnMore->setText(QStringLiteral("O"));
        btnMore->setCheckable(true);
        btnMore->setAutoRaise(false);

        horizontalLayout->addWidget(btnMore);

        horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnClose = new QToolButton(findContainer);
        btnClose->setObjectName(QStringLiteral("btnClose"));
        btnClose->setText(QStringLiteral("X"));
        btnClose->setAutoRaise(false);

        horizontalLayout->addWidget(btnClose);


        verticalLayout->addWidget(findContainer);

        replaceContainer = new QWidget(Editor__FindReplace);
        replaceContainer->setObjectName(QStringLiteral("replaceContainer"));
        sizePolicy.setHeightForWidth(replaceContainer->sizePolicy().hasHeightForWidth());
        replaceContainer->setSizePolicy(sizePolicy);
        replaceContainer->setMinimumSize(QSize(0, 32));
        horizontalLayout_2 = new QHBoxLayout(replaceContainer);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(replaceContainer);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(80, 0));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_2);

        replace = new QLineEdit(replaceContainer);
        replace->setObjectName(QStringLiteral("replace"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(replace->sizePolicy().hasHeightForWidth());
        replace->setSizePolicy(sizePolicy2);
        replace->setMinimumSize(QSize(150, 0));
        replace->setBaseSize(QSize(200, 0));

        horizontalLayout_2->addWidget(replace);

        horizontalSpacer_3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        btnReplace = new QPushButton(replaceContainer);
        btnReplace->setObjectName(QStringLiteral("btnReplace"));

        horizontalLayout_2->addWidget(btnReplace);

        btnReplaceAll = new QPushButton(replaceContainer);
        btnReplaceAll->setObjectName(QStringLiteral("btnReplaceAll"));

        horizontalLayout_2->addWidget(btnReplaceAll);


        verticalLayout->addWidget(replaceContainer);

        optionsContainer = new QWidget(Editor__FindReplace);
        optionsContainer->setObjectName(QStringLiteral("optionsContainer"));
        sizePolicy.setHeightForWidth(optionsContainer->sizePolicy().hasHeightForWidth());
        optionsContainer->setSizePolicy(sizePolicy);
        optionsContainer->setMinimumSize(QSize(0, 32));
        horizontalLayout_3 = new QHBoxLayout(optionsContainer);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(optionsContainer);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        searchMode = new QComboBox(optionsContainer);
        searchMode->setObjectName(QStringLiteral("searchMode"));

        horizontalLayout_3->addWidget(searchMode);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_4);

        matchCase = new QCheckBox(optionsContainer);
        matchCase->setObjectName(QStringLiteral("matchCase"));

        horizontalLayout_3->addWidget(matchCase);


        verticalLayout->addWidget(optionsContainer);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(find);
        label_2->setBuddy(replace);
        label_3->setBuddy(searchMode);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(find, replace);
        QWidget::setTabOrder(replace, btnReplace);
        QWidget::setTabOrder(btnReplace, btnReplaceAll);
        QWidget::setTabOrder(btnReplaceAll, searchMode);
        QWidget::setTabOrder(searchMode, matchCase);
        QWidget::setTabOrder(matchCase, btnPrev);
        QWidget::setTabOrder(btnPrev, btnNext);
        QWidget::setTabOrder(btnNext, btnMore);
        QWidget::setTabOrder(btnMore, btnClose);

        retranslateUi(Editor__FindReplace);

        QMetaObject::connectSlotsByName(Editor__FindReplace);
    } // setupUi

    void retranslateUi(QWidget *Editor__FindReplace)
    {
        Editor__FindReplace->setWindowTitle(QApplication::translate("Editor::FindReplace", "Form", 0));
        label->setText(QApplication::translate("Editor::FindReplace", "Find:", 0));
#ifndef QT_NO_TOOLTIP
        btnPrev->setToolTip(QApplication::translate("Editor::FindReplace", "Find previous", 0));
#endif // QT_NO_TOOLTIP
        btnPrev->setShortcut(QApplication::translate("Editor::FindReplace", "Shift+F3", 0));
#ifndef QT_NO_TOOLTIP
        btnNext->setToolTip(QApplication::translate("Editor::FindReplace", "Find next", 0));
#endif // QT_NO_TOOLTIP
        btnNext->setShortcut(QApplication::translate("Editor::FindReplace", "F3", 0));
#ifndef QT_NO_TOOLTIP
        btnMore->setToolTip(QApplication::translate("Editor::FindReplace", "Show options", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btnClose->setToolTip(QApplication::translate("Editor::FindReplace", "Close search", 0));
#endif // QT_NO_TOOLTIP
        btnClose->setShortcut(QApplication::translate("Editor::FindReplace", "Esc", 0));
        label_2->setText(QApplication::translate("Editor::FindReplace", "Replace:", 0));
        btnReplace->setText(QApplication::translate("Editor::FindReplace", "Replace", 0));
        btnReplaceAll->setText(QApplication::translate("Editor::FindReplace", "Replace All", 0));
        label_3->setText(QApplication::translate("Editor::FindReplace", "Search mode:", 0));
        searchMode->clear();
        searchMode->insertItems(0, QStringList()
         << QApplication::translate("Editor::FindReplace", "Plain text", 0)
         << QApplication::translate("Editor::FindReplace", "Pattern", 0)
        );
        matchCase->setText(QApplication::translate("Editor::FindReplace", "Match case", 0));
    } // retranslateUi

};

} // namespace Editor

namespace Editor {
namespace Ui {
    class FindReplace: public Ui_FindReplace {};
} // namespace Ui
} // namespace Editor

#endif // UI_FINDREPLACE_H
