#include "multipagedialog.h"
#include "multipagedialog_impl.h"

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QListWidgetItem>
#include <QGridLayout>
#include <QPushButton>

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
    list_ = new QListWidget(pClass_);
    grid->addWidget(list_, 0, 0);
    stack_ = new QStackedWidget(pClass_);
    grid->addWidget(stack_, 0, 1);
    connect(list_, SIGNAL(currentRowChanged(int)),
            this, SLOT(handleGroupSelected(int)));    
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

void MultiPageDialogImpl::resetAllPages()
{
    for (int i=0; i<pages_.size(); i++) {
        const QMetaObject * mo = pages_[i]->metaObject();
        QMetaMethod m;
        bool found = false;
        for (int j=0; j<mo->methodCount(); j++) {
            const QString signature = mo->method(j).signature();
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
    init();
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
