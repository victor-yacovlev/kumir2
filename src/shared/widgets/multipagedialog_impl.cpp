#include "multipagedialog.h"
#include "multipagedialog_impl.h"

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QListWidgetItem>
#include <QHBoxLayout>

namespace Widgets {

MultiPageDialogImpl::MultiPageDialogImpl(class MultiPageDialog * parent)
    : QObject(parent)
    , pClass_(parent)
{
}

void MultiPageDialogImpl::setupUi()
{
    pClass_->setMinimumSize(400, 300);
    pClass_->setLayout(new QHBoxLayout);
    list_ = new QListWidget(pClass_);
    pClass_->layout()->addWidget(list_);
    stack_ = new QStackedWidget(pClass_);
    pClass_->layout()->addWidget(stack_);
    connect(list_, SIGNAL(currentRowChanged(int)),
            this, SLOT(handleGroupSelected(int)));    
}

void MultiPageDialogImpl::acceptAllPages()
{
    for (int i=0; i<pages_.size(); i++) {
        const QMetaObject * mo = pages_[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
            const QString signature = mo->method(j).signature();
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

void MultiPageDialogImpl::init()
{
    for (int i=0; i<pages_.size(); i++) {
        const QMetaObject * mo = pages_[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
            const QString signature = mo->method(j).signature();
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

void MultiPageDialogImpl::handleGroupSelected(int index)
{
    stack_->setCurrentIndex(index);
}

void MultiPageDialogImpl::addPage(QWidget* page)
{
    if (!page)
        return;
    page->setParent(pClass_);
    QListWidgetItem * item = new QListWidgetItem(list_);
    item->setText(page->windowTitle());
    list_->addItem(item);
    QScrollArea * scroll = new QScrollArea(pClass_);
    scroll->setWidgetResizable(true);
    scroll->setWidget(page);
    stack_->addWidget(scroll);
    pages_ << page;
    updateListWidth();
}

void MultiPageDialogImpl::updateListWidth()
{
    int w = 100;
    static const int Padding = 4;
    for (int i=0; i<list_->count(); i++) {
        QListWidgetItem * item = list_->item(i);
        QFontMetrics fm = QFontMetrics(item->font());
        w = qMax(w, 2 * Padding + fm.width(item->text()));
    }
    list_->setFixedWidth(w);
}

} // namespace Widgets
