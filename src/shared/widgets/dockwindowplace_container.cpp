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
    const QPalette & pal = palette();
    const QString fgColor = pal.color(QPalette::Foreground).name();

    static const QSize buttonSize = QSize(24, 16);
    static const int titleHeight = 28;
    static const char * buttonCss = ""
            "QPushButton {"
            "   background: none;"
            "   border-radius: 2px;"
            "}"
            "QPushButton:focus { background: none; }"
            "QPushButton:hover {"
            "   border: 1px solid %fg;"
            "}"
            "QPushButton:pressed {"
            "   border: 1px solid %fg;"
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

    btnDock_->setStyleSheet(QString(buttonCss).replace("%fg", fgColor));
    btnClose_->setStyleSheet(QString(buttonCss).replace("%fg", fgColor));
    windowTitle_->setStyleSheet(titleCss);

    windowTitle_->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

    static const QString iconPrefix = "black_";
    static const QString pixmapsPrefix =
            ExtensionSystem::PluginManager::instance()->sharePath()+"/widgets/secondarywindow/" +
            iconPrefix;

    QIcon dockIcon, closeIcon;
    dockIcon = createIconMathcingColorTheme(pixmapsPrefix+"dock.png");
    closeIcon = createIconMathcingColorTheme(pixmapsPrefix+"close.png");

    btnDock_->setToolTip(tr("Detach to separate window"));
    btnDock_->setIcon(dockIcon);
    btnClose_->setIcon(closeIcon);
}

QIcon DockWindowPlaceContainer::createIconMathcingColorTheme(const QString &fileName) const
{
    const QPalette & pal = palette();
    const QColor fgColor = pal.color(QPalette::Foreground).toHsv();
    const bool invertedTheme = fgColor.value() > 127;
    const QImage sourceImage = QImage(fileName);
    QImage target(sourceImage.size(), QImage::Format_ARGB32);
    target.fill(0);
    for (int y=0; y<sourceImage.height(); y++) {
        for (int x=0; x<sourceImage.width(); x++) {
            const QRgb sourceRGB = sourceImage.pixel(x, y);
            const QColor sourceColor = QColor::fromRgba(sourceRGB).toHsv();
            const int h = fgColor.hue();
            const int s = fgColor.saturation();
            const int themeValue = fgColor.value();
            const int sourceValue = sourceColor.value();
            int v = int(qreal(sourceValue) * (themeValue / 255.0f));
            if (invertedTheme) {
                v = themeValue - v;
            }
            const int a = sourceColor.alpha();
            const QColor targetColor = QColor::fromHsv(h, s, v, a);
            const QRgb targetRGB = targetColor.toRgb().rgba();
            target.setPixel(x, y, targetRGB);
        }
    }
    QIcon result;
    result.addPixmap(QPixmap::fromImage(target));
    return result;
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
        if (-1 != obj->metaObject()->indexOfMethod("setDock(bool)")) {
            QMetaObject::invokeMethod(obj, "setDock", Q_ARG(bool, false));
        }
    }
}

} // namespace Widgets
