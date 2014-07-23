#include "toolbarcontextmenu.h"

#include <QCheckBox>
#include <QFrame>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionMenuItem>

namespace CoreGUI {

static const int MaxColumns = 2;

ToolbarContextMenu::ToolbarContextMenu(QWidget *parent)
    : QWidget(parent)
    , grid_(new QGridLayout)
    , currentRow_(0)
    , currentColumn_(0)
    , btnClose_(0)
    , btnReset_(0)
    , btnShowAll_(0)
    , ignoreStateChange_(true)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setWindowFlags(Qt::Popup);
//    setWindowFlags(Qt::Window|
//                   Qt::CustomizeWindowHint|
//                   Qt::WindowTitleHint|
//                   Qt::WindowCloseButtonHint);
    setWindowTitle(tr("Customize tool bar icons"));
//    grid_->setContentsMargins(0, 0, 0, 0);
    grid_->setSpacing(0);
    setLayout(grid_);
}

void ToolbarContextMenu::addProxy(Widgets::ActionProxy *proxyItem)
{
    items_.append(proxyItem);
    QCheckBox * toggle = new QCheckBox(proxyItem->text(), this);
    toggle->setIcon(proxyItem->icon());
    quintptr targetPtr = quintptr(proxyItem);
    quintptr togglePtr = quintptr(toggle);
    proxyItem->setProperty("toggle", togglePtr);
    toggle->setProperty("target", targetPtr);
    grid_->addWidget(toggle, currentRow_, currentColumn_, 1, 1, Qt::AlignLeft|Qt::AlignVCenter);
    currentColumn_ ++;
    if (currentColumn_ >= MaxColumns) {
        currentRow_ ++;
        currentColumn_ = 0;
    }
    toggle->setChecked(proxyItem->isVisible());
    connect(toggle, SIGNAL(stateChanged(int)), this, SLOT(toggleProxyButtonVisible(int)));
}

void ToolbarContextMenu::addSeparator()
{
    if (currentColumn_ > 0) {
        currentRow_ ++;
        currentColumn_ = 0;
    }
    QFrame * line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFixedHeight(3);
    line->setMinimumWidth(480);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    grid_->addWidget(line, currentRow_, 0, 1, MaxColumns, Qt::AlignCenter);
    currentRow_++;
    currentColumn_ = 0;
}

void ToolbarContextMenu::finalize()
{
    addSeparator();
    QWidget * box = new QWidget(this);
    QHBoxLayout * l = new QHBoxLayout;
    box->setLayout(l);
    btnClose_ = new QPushButton(tr("Close"), this);
    btnReset_ = new QPushButton(tr("Reset to default"), this);
    btnShowAll_ = new QPushButton(tr("Check all"), this);
    l->setContentsMargins(0, 4, 0, 0);
    l->addWidget(btnClose_);
    l->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    l->addWidget(btnReset_);
    l->addWidget(btnShowAll_);
    grid_->addWidget(box, currentRow_, 0, 1, MaxColumns, Qt::AlignCenter);

    connect(btnClose_, SIGNAL(clicked()), this, SLOT(hide()));
    connect(btnReset_, SIGNAL(clicked()), this, SLOT(reset()));
    connect(btnShowAll_, SIGNAL(clicked()), this, SLOT(showAll()));

    loadSettings();
    ignoreStateChange_ = false;
}

void ToolbarContextMenu::setExplicitImportNames(const QSet<QString> explicitItemNames)
{
    explicitImportNames_ = explicitItemNames;
    ignoreStateChange_ = true;
    Q_FOREACH(Widgets::ActionProxy * target, items_) {
        quintptr togglePtr = target->property("toggle").toULongLong();
        QCheckBox * toggle = reinterpret_cast<QCheckBox*>(togglePtr);
        target->setVisible(isItemVisible(toggle->checkState(), target->objectName()));
    }
    saveSettings();
    ignoreStateChange_ = false;
}

void ToolbarContextMenu::toggleProxyButtonVisible(int iState)
{
    if (ignoreStateChange_)
        return;
    Qt::CheckState state = Qt::CheckState(iState);
    quintptr targetPtr = sender()->property("target").toULongLong();
    Widgets::ActionProxy * target = reinterpret_cast<Widgets::ActionProxy*>(targetPtr);
    target->setVisible(isItemVisible(state, target->objectName()));
    saveSettings();
}

Qt::CheckState ToolbarContextMenu::defaultVisible(const QString &objectName)
{
    static const QStringList standardItems = QStringList()
             << "file-new" << "file-open" << "file-save"
             << "edit-cut" << "edit-copy" << "edit-paste"
             << "edit-undo" << "edit-redo"
             << "run-regular" << "run-blind" << "run-stop"
             << "run-step-over" << "run-step-in" << "run-step-out"
             << "window-courses"
             << "window-actor-robot" << "window-control-robot"
             << "window-actor-draw"
                ;
    if (standardItems.contains(objectName))
        return Qt::Checked;
    if (objectName.startsWith("window-actor-") || objectName.startsWith("window-control-"))
        return Qt::PartiallyChecked;
    return Qt::Unchecked;
}

void ToolbarContextMenu::setSettingsObject(ExtensionSystem::SettingsPtr settings, const QString &settingsPrefix)
{
    settings_ = settings;
    settingsPrefix_ = settingsPrefix;
    loadSettings();
}

float ToolbarContextMenu::cs2f(const Qt::CheckState state) {
    switch (state) {
    case Qt::PartiallyChecked:  return 0.5f;
    case Qt::Checked:           return 1.0f;
    default:                    return 0.0f;
    }
}

Qt::CheckState ToolbarContextMenu::f2cs(const float value) {
    if (value > 0.9f) {
        return Qt::Checked;
    }
    else if (0.4f < value && value < 0.6f) {
        return Qt::PartiallyChecked;
    }
    else {
        return Qt::Unchecked;
    }
}

void ToolbarContextMenu::loadSettings()
{
    ignoreStateChange_ = true;
    if (!settings_) return;
    Q_FOREACH(Widgets::ActionProxy * target, items_) {
        quintptr togglePtr = target->property("toggle").toULongLong();
        QCheckBox * toggle = reinterpret_cast<QCheckBox*>(togglePtr);
        if (target->objectName().startsWith("window-actor-") ||
                target->objectName().startsWith("window-control-"))
        {
            toggle->setTristate(true);
        }
        const QString key = settingsPrefix_ + "/" + (
                    target->objectName().isEmpty()
                    ? QString("unknown")
                    : target->objectName()
                      );
        const Qt::CheckState state = f2cs(
                settings_->value(key, cs2f(defaultVisible(target->objectName())))
                .toFloat()
                    );
        toggle->setCheckState(state);
        target->setVisible(isItemVisible(toggle->checkState(), target->objectName()));
    }
    ignoreStateChange_ = false;
}

void ToolbarContextMenu::saveSettings() const
{
    if (!settings_) return;
    Q_FOREACH(Widgets::ActionProxy * target, items_) {
        quintptr togglePtr = target->property("toggle").toULongLong();
        QCheckBox * toggle = reinterpret_cast<QCheckBox*>(togglePtr);
        const QString key = settingsPrefix_ + "/" + (
                    target->objectName().isEmpty()
                    ? QString("unknown")
                    : target->objectName()
                      );
        const double value = cs2f(toggle->checkState());
        settings_->setValue(key, value);
    }
}

void ToolbarContextMenu::reset()
{
    ignoreStateChange_ = true;
    Q_FOREACH(Widgets::ActionProxy * target, items_) {
        quintptr togglePtr = target->property("toggle").toULongLong();
        QCheckBox * toggle = reinterpret_cast<QCheckBox*>(togglePtr);
        toggle->setCheckState(defaultVisible(target->objectName()));
        target->setVisible(isItemVisible(toggle->checkState(), target->objectName()));
    }
    saveSettings();
    ignoreStateChange_ = false;
}

bool ToolbarContextMenu::isItemVisible(Qt::CheckState state, const QString &objectName) const
{
    return
            Qt::Checked == state ||
            (Qt::PartiallyChecked == state && explicitImportNames_.contains(objectName));
}

void ToolbarContextMenu::showAll()
{
    ignoreStateChange_ = true;
    Q_FOREACH(Widgets::ActionProxy * target, items_) {
        quintptr togglePtr = target->property("toggle").toULongLong();
        QCheckBox * toggle = reinterpret_cast<QCheckBox*>(togglePtr);
        toggle->setCheckState(Qt::Checked);
        target->setVisible(true);
    }
    saveSettings();
    ignoreStateChange_ = false;
}

void ToolbarContextMenu::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QStyleOptionMenuItem menuOpt;
    QRegion emptyArea = QRegion(rect());
    menuOpt.initFrom(this);
    menuOpt.state = QStyle::State_None;
    menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
    menuOpt.maxIconWidth = 0;
    menuOpt.tabWidth = 0;
    style()->drawPrimitive(QStyle::PE_PanelMenu, &menuOpt, &p, this);
    QWidget::paintEvent(event);
    const int fw = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, this);
    QRegion borderReg;
    borderReg += QRect(0, 0, fw, height()); //left
    borderReg += QRect(width()-fw, 0, fw, height()); //right
    borderReg += QRect(0, 0, width(), fw); //top
    borderReg += QRect(0, height()-fw, width(), fw); //bottom
    p.setClipRegion(borderReg);
    emptyArea -= borderReg;
    QStyleOptionFrame frame;
    frame.rect = rect();
    frame.palette = palette();
    frame.state = QStyle::State_None;
    frame.lineWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth);
    frame.midLineWidth = 0;
    style()->drawPrimitive(QStyle::PE_FrameMenu, &frame, &p, this);
}


} // namespace CoreGUI
