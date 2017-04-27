/********************************************************************************
** Form generated from reading UI file 'quickreferencewidget.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUICKREFERENCEWIDGET_H
#define UI_QUICKREFERENCEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace KumirAnalizer {

class Ui_QuickReferenceWidget
{
public:
    QVBoxLayout *verticalLayout_6;
    QWidget *centralView;
    QHBoxLayout *horizontalLayout;
    QWidget *leftColumn;
    QVBoxLayout *verticalLayout;
    QWidget *statementsBlock;
    QVBoxLayout *verticalLayout_5;
    QLabel *statementsLabel;
    QWidget *statementsContents;
    QWidget *typesBlock;
    QVBoxLayout *verticalLayout_4;
    QLabel *typesLabel;
    QWidget *typesContents;
    QSpacerItem *verticalSpacer;
    QWidget *rightColumn;
    QVBoxLayout *verticalLayout_2;
    QWidget *functionsBlock;
    QVBoxLayout *verticalLayout_3;
    QLabel *functionsLabel;
    QWidget *functionsContents;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *KumirAnalizer__QuickReferenceWidget)
    {
        if (KumirAnalizer__QuickReferenceWidget->objectName().isEmpty())
            KumirAnalizer__QuickReferenceWidget->setObjectName(QStringLiteral("KumirAnalizer__QuickReferenceWidget"));
        KumirAnalizer__QuickReferenceWidget->resize(504, 187);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(KumirAnalizer__QuickReferenceWidget->sizePolicy().hasHeightForWidth());
        KumirAnalizer__QuickReferenceWidget->setSizePolicy(sizePolicy);
        KumirAnalizer__QuickReferenceWidget->setMinimumSize(QSize(420, 0));
        verticalLayout_6 = new QVBoxLayout(KumirAnalizer__QuickReferenceWidget);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        centralView = new QWidget(KumirAnalizer__QuickReferenceWidget);
        centralView->setObjectName(QStringLiteral("centralView"));
        horizontalLayout = new QHBoxLayout(centralView);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(10, 10, 10, 10);
        leftColumn = new QWidget(centralView);
        leftColumn->setObjectName(QStringLiteral("leftColumn"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(leftColumn->sizePolicy().hasHeightForWidth());
        leftColumn->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(leftColumn);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        statementsBlock = new QWidget(leftColumn);
        statementsBlock->setObjectName(QStringLiteral("statementsBlock"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(statementsBlock->sizePolicy().hasHeightForWidth());
        statementsBlock->setSizePolicy(sizePolicy2);
        statementsBlock->setMinimumSize(QSize(220, 0));
        verticalLayout_5 = new QVBoxLayout(statementsBlock);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        statementsLabel = new QLabel(statementsBlock);
        statementsLabel->setObjectName(QStringLiteral("statementsLabel"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(statementsLabel->sizePolicy().hasHeightForWidth());
        statementsLabel->setSizePolicy(sizePolicy3);

        verticalLayout_5->addWidget(statementsLabel);

        statementsContents = new QWidget(statementsBlock);
        statementsContents->setObjectName(QStringLiteral("statementsContents"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(statementsContents->sizePolicy().hasHeightForWidth());
        statementsContents->setSizePolicy(sizePolicy4);
        statementsContents->setMinimumSize(QSize(200, 40));

        verticalLayout_5->addWidget(statementsContents);


        verticalLayout->addWidget(statementsBlock);

        typesBlock = new QWidget(leftColumn);
        typesBlock->setObjectName(QStringLiteral("typesBlock"));
        sizePolicy2.setHeightForWidth(typesBlock->sizePolicy().hasHeightForWidth());
        typesBlock->setSizePolicy(sizePolicy2);
        typesBlock->setMinimumSize(QSize(220, 0));
        verticalLayout_4 = new QVBoxLayout(typesBlock);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        typesLabel = new QLabel(typesBlock);
        typesLabel->setObjectName(QStringLiteral("typesLabel"));
        sizePolicy3.setHeightForWidth(typesLabel->sizePolicy().hasHeightForWidth());
        typesLabel->setSizePolicy(sizePolicy3);

        verticalLayout_4->addWidget(typesLabel);

        typesContents = new QWidget(typesBlock);
        typesContents->setObjectName(QStringLiteral("typesContents"));
        sizePolicy4.setHeightForWidth(typesContents->sizePolicy().hasHeightForWidth());
        typesContents->setSizePolicy(sizePolicy4);
        typesContents->setMinimumSize(QSize(200, 40));

        verticalLayout_4->addWidget(typesContents);


        verticalLayout->addWidget(typesBlock);

        verticalSpacer = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addWidget(leftColumn);

        rightColumn = new QWidget(centralView);
        rightColumn->setObjectName(QStringLiteral("rightColumn"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(rightColumn->sizePolicy().hasHeightForWidth());
        rightColumn->setSizePolicy(sizePolicy5);
        verticalLayout_2 = new QVBoxLayout(rightColumn);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        functionsBlock = new QWidget(rightColumn);
        functionsBlock->setObjectName(QStringLiteral("functionsBlock"));
        QSizePolicy sizePolicy6(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(functionsBlock->sizePolicy().hasHeightForWidth());
        functionsBlock->setSizePolicy(sizePolicy6);
        functionsBlock->setMinimumSize(QSize(270, 0));
        verticalLayout_3 = new QVBoxLayout(functionsBlock);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        functionsLabel = new QLabel(functionsBlock);
        functionsLabel->setObjectName(QStringLiteral("functionsLabel"));
        sizePolicy3.setHeightForWidth(functionsLabel->sizePolicy().hasHeightForWidth());
        functionsLabel->setSizePolicy(sizePolicy3);

        verticalLayout_3->addWidget(functionsLabel);

        functionsContents = new QWidget(functionsBlock);
        functionsContents->setObjectName(QStringLiteral("functionsContents"));
        sizePolicy4.setHeightForWidth(functionsContents->sizePolicy().hasHeightForWidth());
        functionsContents->setSizePolicy(sizePolicy4);
        functionsContents->setMinimumSize(QSize(60, 40));

        verticalLayout_3->addWidget(functionsContents);


        verticalLayout_2->addWidget(functionsBlock);

        verticalSpacer_2 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        horizontalLayout->addWidget(rightColumn);


        verticalLayout_6->addWidget(centralView);


        retranslateUi(KumirAnalizer__QuickReferenceWidget);

        QMetaObject::connectSlotsByName(KumirAnalizer__QuickReferenceWidget);
    } // setupUi

    void retranslateUi(QWidget *KumirAnalizer__QuickReferenceWidget)
    {
        KumirAnalizer__QuickReferenceWidget->setWindowTitle(QApplication::translate("KumirAnalizer::QuickReferenceWidget", "Quick Reference", 0));
        statementsLabel->setText(QApplication::translate("KumirAnalizer::QuickReferenceWidget", "Language Statements", 0));
        typesLabel->setText(QApplication::translate("KumirAnalizer::QuickReferenceWidget", "Value Types", 0));
        functionsLabel->setText(QApplication::translate("KumirAnalizer::QuickReferenceWidget", "Built-in Functions", 0));
    } // retranslateUi

};

} // namespace KumirAnalizer

namespace KumirAnalizer {
namespace Ui {
    class QuickReferenceWidget: public Ui_QuickReferenceWidget {};
} // namespace Ui
} // namespace KumirAnalizer

#endif // UI_QUICKREFERENCEWIDGET_H
