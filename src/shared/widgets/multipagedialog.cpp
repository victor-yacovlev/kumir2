#include "multipagedialog.h"
#include "multipagedialog_impl.h"

namespace Widgets {

MultiPageDialog::MultiPageDialog(QWidget *parent)
    : QDialog(parent)
    , pImpl_(new MultiPageDialogImpl(this))
{  
    pImpl_->setupUi();
}

void MultiPageDialog::addPage(QWidget *page)
{
    pImpl_->addPage(page);
}

int MultiPageDialog::exec()
{
    pImpl_->init();
    return QDialog::exec();
}

void MultiPageDialog::accept()
{
    pImpl_->acceptAllPages();
    QDialog::accept();
}

} // namespace ExtensionSystem
