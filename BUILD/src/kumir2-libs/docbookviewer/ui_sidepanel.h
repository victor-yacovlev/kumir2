/********************************************************************************
** Form generated from reading UI file 'sidepanel.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIDEPANEL_H
#define UI_SIDEPANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

namespace DocBookViewer {

class Ui_SidePanel
{
public:
    QVBoxLayout *verticalLayout_4;
    QFrame *frame;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QPushButton *examples;
    QPushButton *algorithms;
    QPushButton *contents;
    QStackedWidget *stackedWidget;
    QWidget *pageContents;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *searchContents;
    QTreeWidget *contentsNavigator;
    QWidget *pageAlgorithms;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *searchAlgorithms;
    QTreeWidget *algorithmsNavigator;
    QWidget *pageExamples;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *searchExamples;
    QTreeWidget *examplesNavigator;
    QWidget *pageTables;
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *searchTables;
    QTreeWidget *tablesNavigator;

    void setupUi(QWidget *DocBookViewer__SidePanel)
    {
        if (DocBookViewer__SidePanel->objectName().isEmpty())
            DocBookViewer__SidePanel->setObjectName(QStringLiteral("DocBookViewer__SidePanel"));
        DocBookViewer__SidePanel->resize(200, 259);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DocBookViewer__SidePanel->sizePolicy().hasHeightForWidth());
        DocBookViewer__SidePanel->setSizePolicy(sizePolicy);
        DocBookViewer__SidePanel->setMinimumSize(QSize(180, 0));
        verticalLayout_4 = new QVBoxLayout(DocBookViewer__SidePanel);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(DocBookViewer__SidePanel);
        frame->setObjectName(QStringLiteral("frame"));
        gridLayout = new QGridLayout(frame);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_5 = new QLabel(frame);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(0, 28));

        gridLayout->addWidget(label_5, 0, 0, 1, 1);

        examples = new QPushButton(frame);
        examples->setObjectName(QStringLiteral("examples"));
        examples->setCheckable(true);

        gridLayout->addWidget(examples, 2, 0, 1, 1);

        algorithms = new QPushButton(frame);
        algorithms->setObjectName(QStringLiteral("algorithms"));
        algorithms->setCheckable(true);

        gridLayout->addWidget(algorithms, 2, 1, 1, 1);

        contents = new QPushButton(frame);
        contents->setObjectName(QStringLiteral("contents"));
        contents->setCheckable(true);
        contents->setChecked(true);

        gridLayout->addWidget(contents, 1, 0, 1, 2);


        verticalLayout_4->addWidget(frame);

        stackedWidget = new QStackedWidget(DocBookViewer__SidePanel);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        QFont font;
        font.setPointSize(12);
        stackedWidget->setFont(font);
        pageContents = new QWidget();
        pageContents->setObjectName(QStringLiteral("pageContents"));
        verticalLayout_2 = new QVBoxLayout(pageContents);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(pageContents);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        searchContents = new QLineEdit(pageContents);
        searchContents->setObjectName(QStringLiteral("searchContents"));

        horizontalLayout_2->addWidget(searchContents);


        verticalLayout_2->addLayout(horizontalLayout_2);

        contentsNavigator = new QTreeWidget(pageContents);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        contentsNavigator->setHeaderItem(__qtreewidgetitem);
        contentsNavigator->setObjectName(QStringLiteral("contentsNavigator"));
        contentsNavigator->setFont(font);
        contentsNavigator->setIndentation(10);
        contentsNavigator->setHeaderHidden(true);

        verticalLayout_2->addWidget(contentsNavigator);

        stackedWidget->addWidget(pageContents);
        pageAlgorithms = new QWidget();
        pageAlgorithms->setObjectName(QStringLiteral("pageAlgorithms"));
        verticalLayout_3 = new QVBoxLayout(pageAlgorithms);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(pageAlgorithms);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        searchAlgorithms = new QLineEdit(pageAlgorithms);
        searchAlgorithms->setObjectName(QStringLiteral("searchAlgorithms"));

        horizontalLayout->addWidget(searchAlgorithms);


        verticalLayout_3->addLayout(horizontalLayout);

        algorithmsNavigator = new QTreeWidget(pageAlgorithms);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setText(0, QStringLiteral("1"));
        algorithmsNavigator->setHeaderItem(__qtreewidgetitem1);
        algorithmsNavigator->setObjectName(QStringLiteral("algorithmsNavigator"));
        algorithmsNavigator->setFont(font);
        algorithmsNavigator->setIndentation(10);
        algorithmsNavigator->header()->setVisible(false);

        verticalLayout_3->addWidget(algorithmsNavigator);

        stackedWidget->addWidget(pageAlgorithms);
        pageExamples = new QWidget();
        pageExamples->setObjectName(QStringLiteral("pageExamples"));
        verticalLayout = new QVBoxLayout(pageExamples);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(pageExamples);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        searchExamples = new QLineEdit(pageExamples);
        searchExamples->setObjectName(QStringLiteral("searchExamples"));

        horizontalLayout_3->addWidget(searchExamples);


        verticalLayout->addLayout(horizontalLayout_3);

        examplesNavigator = new QTreeWidget(pageExamples);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem();
        __qtreewidgetitem2->setText(0, QStringLiteral("1"));
        examplesNavigator->setHeaderItem(__qtreewidgetitem2);
        examplesNavigator->setObjectName(QStringLiteral("examplesNavigator"));
        examplesNavigator->setFont(font);
        examplesNavigator->setIndentation(10);
        examplesNavigator->setHeaderHidden(true);

        verticalLayout->addWidget(examplesNavigator);

        stackedWidget->addWidget(pageExamples);
        pageTables = new QWidget();
        pageTables->setObjectName(QStringLiteral("pageTables"));
        verticalLayout_5 = new QVBoxLayout(pageTables);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_4 = new QLabel(pageTables);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_4->addWidget(label_4);

        searchTables = new QLineEdit(pageTables);
        searchTables->setObjectName(QStringLiteral("searchTables"));

        horizontalLayout_4->addWidget(searchTables);


        verticalLayout_5->addLayout(horizontalLayout_4);

        tablesNavigator = new QTreeWidget(pageTables);
        QTreeWidgetItem *__qtreewidgetitem3 = new QTreeWidgetItem();
        __qtreewidgetitem3->setText(0, QStringLiteral("1"));
        tablesNavigator->setHeaderItem(__qtreewidgetitem3);
        tablesNavigator->setObjectName(QStringLiteral("tablesNavigator"));
        tablesNavigator->setFont(font);
        tablesNavigator->setIndentation(10);
        tablesNavigator->setHeaderHidden(true);

        verticalLayout_5->addWidget(tablesNavigator);

        stackedWidget->addWidget(pageTables);

        verticalLayout_4->addWidget(stackedWidget);

#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(searchContents);
        label->setBuddy(searchAlgorithms);
        label_3->setBuddy(searchExamples);
        label_4->setBuddy(searchTables);
#endif // QT_NO_SHORTCUT

        retranslateUi(DocBookViewer__SidePanel);

        QMetaObject::connectSlotsByName(DocBookViewer__SidePanel);
    } // setupUi

    void retranslateUi(QWidget *DocBookViewer__SidePanel)
    {
        DocBookViewer__SidePanel->setWindowTitle(QApplication::translate("DocBookViewer::SidePanel", "Form", 0));
        label_5->setText(QString());
        examples->setText(QApplication::translate("DocBookViewer::SidePanel", "Examples", 0));
        algorithms->setText(QApplication::translate("DocBookViewer::SidePanel", "Algorithms", 0));
        contents->setText(QApplication::translate("DocBookViewer::SidePanel", "Contents", 0));
        label_2->setText(QApplication::translate("DocBookViewer::SidePanel", "Fi&lter:", 0));
        label->setText(QApplication::translate("DocBookViewer::SidePanel", "Filter:", 0));
        label_3->setText(QApplication::translate("DocBookViewer::SidePanel", "Filter:", 0));
        label_4->setText(QApplication::translate("DocBookViewer::SidePanel", "Filter:", 0));
    } // retranslateUi

};

} // namespace DocBookViewer

namespace DocBookViewer {
namespace Ui {
    class SidePanel: public Ui_SidePanel {};
} // namespace Ui
} // namespace DocBookViewer

#endif // UI_SIDEPANEL_H
