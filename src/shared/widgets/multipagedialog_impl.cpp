#include "multipagedialog.h"
#include "multipagedialog_impl.h"

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

namespace Widgets {

MultiPageDialogImpl::MultiPageDialogImpl(class MultiPageDialog * parent)
    : QObject(parent)
    , pClass_(parent)
{
}

void MultiPageDialogImpl::setupUi()
{
    QGridLayout * grid = new QGridLayout;
    pClass_->setMinimumSize(400, 300);
    pClass_->setLayout(grid);
    list_ = new QTreeWidget(pClass_);
    list_->setHeaderHidden(true);
    grid->addWidget(list_, 0, 0);
    stack_ = new QStackedWidget(pClass_);
    grid->addWidget(stack_, 0, 1);
    connect(list_, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(handleGroupSelected(QTreeWidgetItem*,QTreeWidgetItem*)));
    buttonBox_ = new QDialogButtonBox(pClass_);
    grid->addWidget(buttonBox_, 1, 0, 1, 2);

    QPushButton * btnAccept =
            buttonBox_->addButton(tr("OK"), QDialogButtonBox::AcceptRole);
    connect(btnAccept, SIGNAL(clicked()), pClass_, SLOT(accept()));

    QPushButton * btnCancel =
            buttonBox_->addButton(tr("Cancel"), QDialogButtonBox::RejectRole);
    connect(btnCancel, SIGNAL(clicked()), pClass_, SLOT(reject()));

    QPushButton * btnReset =
            buttonBox_->addButton(tr("Reset to Defaults"), QDialogButtonBox::ResetRole);
    connect(btnReset, SIGNAL(clicked()), this, SLOT(resetAllPages()));

}

void MultiPageDialogImpl::acceptAllPages()
{
    for (int i=0; i<pages_.size(); i++) {
        const QMetaObject * mo = pages_[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
#if QT_VERSION >= 0x050000
            const QString signature = mo->method(j).methodSignature();
#else
            const QString signature = mo->method(j).signature();
#endif
            if (signature=="accept()") {
                m = mo->method(j);
                found = true;
                break;
            }
        }
        if (found) {
            m.invoke(pages_[i]);
        }
    }
}

void MultiPageDialogImpl::resetAllPages()
{
    for (int i=0; i<pages_.size(); i++) {
        const QMetaObject * mo = pages_[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
#if QT_VERSION >= 0x050000
            const QString signature = mo->method(j).methodSignature();
#else
            const QString signature = mo->method(j).signature();
#endif
            if (signature=="resetToDefaults()") {
                m = mo->method(j);
                found = true;
                break;
            }
        }
        if (found) {
            m.invoke(pages_[i]);
        }
    }
}

void MultiPageDialogImpl::init()
{
    for (int i=0; i<pages_.size(); i++) {
        const QMetaObject * mo = pages_[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
#if QT_VERSION >= 0x050000
            const QString signature = mo->method(j).methodSignature();
#else
            const QString signature = mo->method(j).signature();
#endif
            if (signature=="init()") {
                m = mo->method(j);
                found = true;
                break;
            }
        }
        if (found) {
            m.invoke(pages_[i]);
        }
    }
}

void MultiPageDialogImpl::handleGroupSelected(QTreeWidgetItem * current, QTreeWidgetItem * )
{
    const int index = current->data(0, Qt::UserRole).toInt();
    stack_->setCurrentIndex(index);
}

void MultiPageDialogImpl::addPage(const QString & groupTitle, QWidget* page)
{
    if (!page)
        return;
    page->setParent(pClass_);
    QTreeWidgetItem * groupItem = nullptr;
    for (int i=0; i<list_->topLevelItemCount(); ++i) {
        QTreeWidgetItem * topLevel = list_->topLevelItem(i);
        if (topLevel->text(0) == groupTitle) {
            groupItem = topLevel;
            break;
        }
    }
    if (!groupItem) {
        groupItem = new QTreeWidgetItem(list_, QStringList() << groupTitle);
        QFont fnt = groupItem->font(0);
        fnt.setBold(true);
        groupItem->setFont(0, fnt);
        list_->addTopLevelItem(groupItem);
        groupItem->setExpanded(true);
        const Qt::ItemFlags flags = Qt::ItemIsEnabled;
        groupItem->setFlags(flags);
    }
    QTreeWidgetItem * item = new QTreeWidgetItem(groupItem);
    item->setText(0, page->windowTitle());
    item->setData(0, Qt::UserRole, pages_.size());
    groupItem->addChild(item);
    QWidget * pageContainer = new QWidget(pClass_);
    QVBoxLayout * layout = new QVBoxLayout;
    pageContainer->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);
    QLabel * pageLabel = new QLabel(pClass_);
    QFont fnt = pageLabel->font();
    fnt.setPointSize(fnt.pointSize() + 2);
    pageLabel->setFont(fnt);
    pageLabel->setText(groupTitle + " : " + page->windowTitle());
    pageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    layout->addWidget(pageLabel);
    QScrollArea * scroll = new QScrollArea(pClass_);
    scroll->setWidgetResizable(true);
    scroll->setWidget(page);
    layout->addWidget(scroll);
    stack_->addWidget(pageContainer);
    pages_ << page;
    updateListWidth();
}

void MultiPageDialogImpl::updateListWidth()
{
    int w = 240;
//    static const int Padding = 4;
//    for (int i=0; i<list_->count(); i++) {
//        QListWidgetItem * item = list_->item(i);
//        QFontMetrics fm = QFontMetrics(item->font());
//        w = qMax(w, 2 * Padding + fm.width(item->text()));
//    }
    list_->setFixedWidth(w);
}

} // namespace Widgets
