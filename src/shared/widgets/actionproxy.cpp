#include "actionproxy.h"

namespace Widgets {

ActionProxy::ActionProxy(QAction* target, QObject *parent)
    : QAction(parent)
    , target_(target)
{
    connect(target, SIGNAL(changed()), this, SLOT(handleTargetChanged()));
    connect(this, SIGNAL(triggered()), target, SLOT(trigger()));
    setCheckable(target->isCheckable());
    setEnabled(target->isEnabled());
    setIcon(target->icon());
    setIconText(target->iconText());
    setToolTip(target->toolTip());
    setSeparator(target->isSeparator());
    if (isCheckable()) {
        setChecked(target->isChecked());
    }
    setObjectName(target->objectName());
}

void ActionProxy::handleTargetChanged()
{
    setEnabled(target_->isEnabled());
    if (isCheckable()) {
        setChecked(target_->isChecked());
    }
}

} // namespace Widgets
