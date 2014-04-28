#include "dockwindowplace.h"
#include "dockwindowplace_container.h"
#include "extensionsystem/pluginmanager.h"

#include <QApplication>
#include <QIcon>
#include <QVBoxLayout>

namespace Widgets {

DockWindowPlaceContainer::DockWindowPlaceContainer(DockWindowPlace *parent)
    : SecondaryWindowGenericImplementation(parent)
    , place_(parent)
{
    setParent(parent);
    setupWidgetsAppearance();
    setupWindow();
    setAutoFillBackground(true);
}

void DockWindowPlaceContainer::activate(const QPoint &, const QSize & sz)
{
    int tabIndex = -1;
    for (int i=0; i<place_->count(); i++) {
        if (place_->widget(i) == this) {
            tabIndex = i;
            break;
        }
    }
    if (tabIndex == -1) {
        QSize dockSize = sz;
        dockSize.rwidth() += layout()->contentsMargins().left() +
                layout()->contentsMargins().right();
        dockSize.rheight() += layout()->contentsMargins().top() +
                layout()->contentsMargins().bottom() +
                layout()->spacing() +
                titleBox_->height();
        place_->setPreferredItemSize(dockSize);
        tabIndex = place_->addTab(this, title());
    }
    setVisible(true);
    place_->setCurrentIndex(tabIndex);
    place_->activate(sz);
    centralWidget_->setFocus();
}

void DockWindowPlaceContainer::deactivate()
{
    int tabIndex = -1;
    for (int i=0; i<place_->count(); i++) {
        if (place_->widget(i) == this) {
            tabIndex = i;
            break;
        }
    }
    if (tabIndex != -1) {
        place_->removeTab(tabIndex);
    }
    setVisible(false);
}

void DockWindowPlaceContainer::paintWindowFrame()
{

}

void DockWindowPlaceContainer::setupWidgetsAppearance()
{
    static const QSize buttonSize = QSize(24, 16);
    static const int titleHeight = 28;
    static const char * buttonCss = ""
            "QPushButton {"
            "   background: none;"
            "   border-radius: 2px;"
            "}"
            "QPushButton:focus { background: none; }"
            "QPushButton:hover {"
            "   border: 1px solid black;"
            "}"
            "QPushButton:pressed {"
            "   border: 1px solid black;"
            "}"
            ;
    static const char * titleCss = ""
            "QLabel {"
            "   font-weight: bold;"
            "}"
            ;

    btnDock_->setFixedSize(buttonSize);
    btnClose_->setFixedSize(buttonSize);
    titleBox_->setFixedHeight(titleHeight);

    btnDock_->setStyleSheet(buttonCss);
    btnClose_->setStyleSheet(buttonCss);
    windowTitle_->setStyleSheet(titleCss);

    windowTitle_->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    static const QString iconPrefix = "black_";
    static const QString pixmapsPrefix =
            ExtensionSystem::PluginManager::instance()->sharePath()+"/widgets/secondarywindow/" +
            iconPrefix;

    QIcon dockIcon, closeIcon;
    dockIcon.addFile(pixmapsPrefix+"dock.png");
    closeIcon.addFile(pixmapsPrefix+"close.png");

    btnDock_->setToolTip(tr("Detach to separate window"));
    btnDock_->setIcon(dockIcon);
    btnClose_->setIcon(closeIcon);
}

void DockWindowPlaceContainer::setupWindow()
{
    btnMinimize_->setVisible(false);
    btnStayOnTop_->setVisible(false);
}

SecondaryWindowGenericImplementation::HitTestResult
DockWindowPlaceContainer::hitTest(const QPoint &) const
{
    return Nothing;
}

void DockWindowPlaceContainer::notifyOnDocked()
{
    if (centralWidget_) {
        QObject * obj = centralWidget_;
        QMetaObject::invokeMethod(obj, "setDock", Q_ARG(bool, false));
    }
}

} // namespace Widgets
