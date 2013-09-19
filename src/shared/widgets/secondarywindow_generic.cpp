#include "secondarywindow_generic.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QDebug>

namespace Widgets {

static const int S = 6;

SecondaryWindowGenericImplementation::SecondaryWindowGenericImplementation(QWidget * parent)
    : QWidget(parent)
    , centralWidget_(nullptr)
    , pairedContainer_(nullptr)
    , mousePressHit_(Nothing)
{
    createWidgets();
    createLayout();
    setupWidgetsAppearance();
    setupWindow();
    titleBox_->setFocus();
    startTimer(200);
}

QWidget * SecondaryWindowGenericImplementation::toWidget()
{
    return this;
}

bool SecondaryWindowGenericImplementation::hasWidgetOwnership() const
{
    return centralWidget_ != nullptr;
}

void SecondaryWindowGenericImplementation::setPairedContainer(SecondaryWindowImplementationInterface *pairedContainer)
{
    pairedContainer_ = pairedContainer;
    btnDock_->setVisible(pairedContainer_ != nullptr);
}

void SecondaryWindowGenericImplementation::createWidgets()
{
    btnStayOnTop_ = new QPushButton(this);
    btnStayOnTop_->setToolTip(tr("Toggle stay on top"));
    connect(btnStayOnTop_, SIGNAL(clicked(bool)),
            this, SLOT(toggleStayOnTop(bool)));
    btnStayOnTop_->setCheckable(true);

    btnMinimize_ = new QPushButton(this);
    btnMinimize_->setToolTip(tr("Minimize window"));
    connect(btnMinimize_, SIGNAL(clicked()),
            this, SLOT(minimizeWindow()));

    btnDock_ = new QPushButton(this);
    btnDock_->setToolTip(tr("Dock to main window"));
    connect(btnDock_, SIGNAL(clicked()),
            this, SLOT(toggleDockWindow()));
    btnDock_->setVisible(false);

    btnClose_ = new QPushButton(this);
    btnClose_->setToolTip(tr("Close window"));
    connect(btnClose_, SIGNAL(clicked()),
            this, SLOT(closeWindow()));

    windowTitle_ = new QLabel(this);
}

void SecondaryWindowGenericImplementation::setupWidgetsAppearance()
{
#ifndef Q_OS_MAC
    static const QSize buttonSize = QSize(24, 16);
    static const int titleHeight = 28;
    static const char * buttonCss = ""
            "QPushButton {"
            "   background: none;"
            "   border: 1px solid gray;"
            "   border-radius: 2px;"
            "}"
            "QPushButton:focus { background: none; }"
            "QPushButton:hover {"
            "   border: 1px solid white;"
            "}"
            "QPushButton:pressed {"
            "   background: black;"
            "}"
            ;
    static const char * titleCss = ""
            "QLabel {"
            "   font-weight: bold;"
            "}"
            "QLabel:active {"
            "   color: lightgray;"
            "}"
            ;
#else
    static const QSize buttonSize = QSize(18, 18);
    static const int titleHeight = 22;
    static const char * buttonCss = ""\
            "QPushButton {"
            "   border: 1px solid gray;"
            "   border-radius: 9px;"
            "}"
            "QPushButton:hover {"
            "   border: 1px solid white;"
            "}"
            "#close { background: red; }"
            "#close:hover { background: lightred; }"
            "#close:pressed { background: darkred; }"
            "#minimize { background: yellow; }"
            "#minimize:pressed { background: darkyellow; }"
            "#dock { background: green; }"
            "#dock:pressed { background: darkgreen; }"
            "#ontop { background: cyan; }"
            "#ontop:pressed { background: darkblue; }"
            ;
    static const char * titleCss = ""
            "QLabel {"
            "   font-weight: bold;"
            "   color: black;"
            "}"
            ;
#endif

    btnMinimize_->setObjectName("minimize");
    btnDock_->setObjectName("dock");
    btnClose_->setObjectName("close");
    btnStayOnTop_->setObjectName("ontop");

    btnMinimize_->setFixedSize(buttonSize);
    btnDock_->setFixedSize(buttonSize);
    btnClose_->setFixedSize(buttonSize);
    btnStayOnTop_->setFixedSize(buttonSize);
    titleBox_->setFixedHeight(titleHeight);

    btnMinimize_->setStyleSheet(buttonCss);
    btnDock_->setStyleSheet(buttonCss);
    btnClose_->setStyleSheet(buttonCss);
    btnStayOnTop_->setStyleSheet(buttonCss);
    windowTitle_->setStyleSheet(titleCss);

#ifdef Q_OS_WIN32
    windowTitle_->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
#else
    windowTitle_->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
#endif

#ifndef Q_OS_MAC
    static const QString iconPrefix = "white_";
#else
    static const QString iconPrefix = "black_";
#endif
    static const QString pixmapsPrefix =
            qApp->property("sharePath").toString()+"/widgets/secondarywindow/" +
            iconPrefix;

    QIcon minimizeIcon, dockIcon, closeIcon;
    minimizeIcon.addFile(pixmapsPrefix+"minimize.png");
    dockIcon.addFile(pixmapsPrefix+"dock.png");
    closeIcon.addFile(pixmapsPrefix+"close.png");
    stayOnTopNotToggled_.addFile(pixmapsPrefix+"stayontop.png");
    stayOnTopToggled_.addFile(pixmapsPrefix+"stayontop_toggled.png");

    btnMinimize_->setIcon(minimizeIcon);
    btnDock_->setIcon(dockIcon);
    btnClose_->setIcon(closeIcon);
    btnStayOnTop_->setIcon(stayOnTopNotToggled_);
}

void SecondaryWindowGenericImplementation::setupWindow()
{
    setAttribute(Qt::WA_Hover);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMouseTracking(true);
}

void SecondaryWindowGenericImplementation::createLayout()
{
    QVBoxLayout * l = new QVBoxLayout;
    setLayout(l);
    l->setSpacing(0);

    titleBox_ = new QWidget(this);
    QHBoxLayout * bl = new QHBoxLayout;
    titleBox_->setLayout(bl);
    l->addWidget(titleBox_);

    QList<QWidget*> items;

#ifdef Q_OS_MAC
    items << btnClose_ << btnDock_ << btnMinimize_ << windowTitle_ << btnStayOnTop_;
#else
    items << btnStayOnTop_ << windowTitle_ << btnMinimize_ << btnDock_ << btnClose_;
#endif

    foreach (QWidget * w, items) {
        bl->addWidget(w);
    }
    setResizeble(true);
}

void SecondaryWindowGenericImplementation::getWidgetOwnership(QWidget *w)
{
    centralWidget_ = w;
    w->setParent(this);
    layout()->addWidget(w);
    if (!isResizable()) {
        QSize sz = w->sizeHint();
        w->setFixedSize(sz);
        sz.rheight() += layout()->contentsMargins().top() +
                layout()->contentsMargins().bottom() +
                layout()->spacing() + titleBox_->height();
        setFixedSize(sz);
    }
}

QWidget * SecondaryWindowGenericImplementation::releaseWidgetOwnership()
{
    if (centralWidget_) {
        centralWidget_->setParent(0);
        layout()->removeWidget(centralWidget_);
    }
    QWidget * result = centralWidget_;
    centralWidget_ = 0;
    return result;
}


void SecondaryWindowGenericImplementation::setResizeble(bool v)
{
    if (v) {
        layout()->setContentsMargins(S, 0, S, S);
        titleBox_->layout()->setContentsMargins(0, 0, 0, 0);
    }
    else {
        titleBox_->layout()->setContentsMargins(S, 0, S, 0);
        layout()->setContentsMargins(0, 0, 0, 0);
    }
}

void SecondaryWindowGenericImplementation::activate(const QPoint & ps, const QSize &sz)
{
    if (centralWidget_) {
        QSize newSize;
        if (sz.isValid()) {
            newSize = sz;
            newSize.rwidth() +=
                    layout()->contentsMargins().left() +
                    layout()->contentsMargins().right();
            newSize.rheight() +=
                    layout()->contentsMargins().top() +
                    titleBox_->height() +
                    layout()->spacing() +
                    layout()->contentsMargins().bottom();
            resize(newSize);
        }

        if (!ps.isNull()) {
            QPoint newPos = ps;
            newPos.rx() -= layout()->contentsMargins().left();
            newPos.ry() -= layout()->contentsMargins().top() +
                    titleBox_->height() +
                    layout()->spacing();
            move(newPos);
        }
        setVisible(true);
        centralWidget_->setFocus();
        QSize minSize = minimumSizeHint();
        if (size().width() < minSize.width() || size().height() < minSize.height()) {
            newSize.rwidth() = qMax(size().width(), minSize.width());
            newSize.rheight() = qMax(size().height(), minSize.height());
            resize(newSize);
        }
        activateWindow();
    }
}

void SecondaryWindowGenericImplementation::deactivate()
{
    setVisible(false);
}

bool SecondaryWindowGenericImplementation::isDockable() const
{
    return btnDock_->isVisible();
}

bool SecondaryWindowGenericImplementation::isResizable() const
{
    return layout()->contentsMargins().left() > 0;
}

void SecondaryWindowGenericImplementation::setTitle(const QString &title)
{
    windowTitle_->setText(title);
    setWindowTitle(title);
}

QString SecondaryWindowGenericImplementation::title() const
{
    return windowTitle_->text();
}

void SecondaryWindowGenericImplementation::setIcon(const QIcon &icon)
{
    setWindowIcon(icon);
}

QIcon SecondaryWindowGenericImplementation::icon() const
{
    return windowIcon();
}

void SecondaryWindowGenericImplementation::closeWindow()
{
    titleBox_->setFocus();
    deactivate();
}

void SecondaryWindowGenericImplementation::minimizeWindow()
{
    titleBox_->setFocus();
    showMinimized();
}

void SecondaryWindowGenericImplementation::toggleStayOnTop(bool v)
{
    titleBox_->setFocus();
    if (v) {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        setVisible(true);
    }
    else if (windowFlags() & Qt::WindowStaysOnTopHint) {
        setWindowFlags(windowFlags() ^ Qt::WindowStaysOnTopHint);
        setVisible(true);
    }
    btnStayOnTop_->setIcon(v? stayOnTopToggled_ : stayOnTopNotToggled_);
    btnStayOnTop_->setChecked(v);
}

void SecondaryWindowGenericImplementation::toggleDockWindow()
{
    titleBox_->setFocus();
    if (pairedContainer_ && centralWidget_) {
        const QPoint ps = mapToGlobal(centralWidget_->pos());
        const QSize sz = centralWidget_->size();
        QWidget * w = releaseWidgetOwnership();
        deactivate();
        pairedContainer_->getWidgetOwnership(w);
        pairedContainer_->activate(ps, sz);
        notifyOnDocked();
    }
}

void SecondaryWindowGenericImplementation::paintWindowFrame()
{

    QPainter p(this);
    QColor bg;
#ifndef Q_OS_MAC
    if (isActiveWindow())
        bg = QColor("#32343C");
    else
        bg = QColor("#98999D");
#else
    bg = QColor(Qt::lightGray);
#endif
    p.setPen(Qt::NoPen);
    p.setBrush(bg);

    // top
    p.drawRect(0, 0, width(),
               layout()->contentsMargins().top() +
               layout()->spacing() +
               titleBox_->height());

    if (isResizable()) {
        // left
        p.drawRect(0, 0, layout()->contentsMargins().left(), height());
        // right
        p.drawRect(width()-layout()->contentsMargins().right(),
                   0, layout()->contentsMargins().right(), height());
        // bottom
        p.drawRect(0, height()-layout()->contentsMargins().bottom(),
                   width(), layout()->contentsMargins().bottom());
    }

#ifndef Q_OS_MAC
    p.setPen(QColor(isActiveWindow() ? Qt::black : Qt::gray));
#else
    p.setPen(QColor(Qt::gray));
#endif
    p.setBrush(Qt::NoBrush);
    p.drawRect(0, 0, width()-1, height()-1);
    if (centralWidget_ && isResizable()) {
        int x = layout()->contentsMargins().left() - 1;
        int y = layout()->contentsMargins().top() +
                layout()->spacing() +
                titleBox_->height() - 1;
        int w = width() - x - layout()->contentsMargins().right();
        int h = height() - y - layout()->contentsMargins().bottom();
        p.drawRect(x, y, w, h);
    }
    p.end();

}

SecondaryWindowGenericImplementation::HitTestResult
SecondaryWindowGenericImplementation::hitTest(const QPoint &mousePosition) const
{
    if (mousePosition.x() < 0 || mousePosition.x() >= width())
        return Nothing;
    if (mousePosition.y() < 0 || mousePosition.y() >= height())
        return Nothing;

    /*
     +----------------------------------+
     |                                  |
     +---+--------------------------+---+   <= y1
     |   |                          |   |
     |   |                          |   |
     +---+--------------------------+---+   <= y2
     |   |                          |   |
     +---+--------------------------+---+

         ^                          ^
         | <= x1              x2 => |

     */

    int y1 = layout()->contentsMargins().top() + titleBox_->height();
    int y2 = height() - layout()->contentsMargins().bottom();
    int x1 = layout()->contentsMargins().left();
    int x2 = width() - layout()->contentsMargins().right();

    if (mousePosition.y() < y1)
        return Title;

    if (!isResizable())
        return Nothing;

    if (mousePosition.y() < y2) {
        if (mousePosition.x() < x1)
            return LeftBorder;
        else if (mousePosition.x() >= x2)
            return RightBorder;
        else
            return Nothing;
    }

    if (mousePosition.y() >= y2) {
        if (mousePosition.x() < x1)
            return LeftCorner;
        else if (mousePosition.x() >= x2)
            return RightCorner;
        else
            return BottomBorder;
    }

    return Nothing;
}

bool SecondaryWindowGenericImplementation::event(QEvent *evt)
{
    if (evt->type() == QEvent::HoverLeave) {
        setProperCursor(Nothing);
    }
    else if (evt->type() == QEvent::HoverEnter) {
        QHoverEvent * e = static_cast<QHoverEvent*>(evt);
        setProperCursor(hitTest(e->pos()));
    }
    else if (evt->type() == QEvent::HoverMove) {
        QHoverEvent * e = static_cast<QHoverEvent*>(evt);
        setProperCursor(hitTest(e->pos()));
    }
    else if (evt->type() == QEvent::Paint) {
        paintWindowFrame();
    }
    else if (evt->type() == QEvent::MouseButtonPress) {
        QMouseEvent * e = static_cast<QMouseEvent*>(evt);
        mousePressPoint_ = e->globalPos();
        mousePressHit_ = hitTest(e->pos());
        setProperCursor(mousePressHit_);
    }
    else if (evt->type() == QEvent::MouseButtonRelease) {
        QMouseEvent * e = static_cast<QMouseEvent*>(evt);
        mousePressPoint_ = QPoint();
        mousePressHit_ = Nothing;
        setProperCursor(hitTest(e->pos()));
    }
    else if (evt->type() == QEvent::MouseMove) {
        QMouseEvent * e = static_cast<QMouseEvent*>(evt);
        if (mousePressHit_ == Title) {
            processMoveWindow(e->globalPos());
        }
        else if (mousePressHit_ != Nothing && isResizable()) {
            processResizeWindow(e->globalPos());
        }
    }
    else if (evt->type() == QEvent::Timer) {
        if (centralWidget_ && centralWidget_->windowTitle().length() > 0) {
            setTitle(centralWidget_->windowTitle());
        }
    }
    return QWidget::event(evt);
}

void SecondaryWindowGenericImplementation::setProperCursor(HitTestResult hit)
{
    if (mousePressHit_ != Nothing)
        hit = mousePressHit_;

    if (hit == LeftBorder || hit == RightBorder) {
        setCursor(Qt::SizeHorCursor);
    }
    else if (hit == BottomBorder) {
        setCursor(Qt::SizeVerCursor);
    }
    else if (hit == LeftCorner) {
        setCursor(Qt::SizeBDiagCursor);
    }
    else if (hit == RightCorner) {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if (mousePressHit_ == Title) {
        setCursor(Qt::SizeAllCursor);
    }
    else {
        unsetCursor();
    }
}

void SecondaryWindowGenericImplementation::processMoveWindow(const QPoint &mousePos)
{
    const QPoint diff = mousePos - mousePressPoint_;
    move(pos() + diff);
    mousePressPoint_ = mousePos;
}

void SecondaryWindowGenericImplementation::processResizeWindow(const QPoint &mousePos)
{
    QSize sizeDiff = QSize(0, 0);
    QPoint posDiff = QPoint(0, 0);
    if (mousePressHit_ == LeftBorder) {
        sizeDiff.rwidth() = mousePressPoint_.x() - mousePos.x();
        posDiff.rx() = mousePos.x() - mousePressPoint_.x();
        mousePressPoint_.rx() = mousePos.x();
    }
    else if (mousePressHit_ == RightBorder) {
        sizeDiff.rwidth() = mousePos.x() - mousePressPoint_.x();
        mousePressPoint_.rx() = mousePos.x();
    }
    else if (mousePressHit_ == BottomBorder) {
        sizeDiff.rheight() = mousePos.y() - mousePressPoint_.y();
        mousePressPoint_.ry() = mousePos.y();
    }
    else if (mousePressHit_ == RightCorner) {
        sizeDiff.rwidth() = mousePos.x() - mousePressPoint_.x();
        sizeDiff.rheight() = mousePos.y() - mousePressPoint_.y();
        mousePressPoint_.rx() = mousePos.x();
        mousePressPoint_.ry() = mousePos.y();
    }
    else if (mousePressHit_ == LeftCorner) {
        sizeDiff.rwidth() = mousePressPoint_.x() - mousePos.x();
        posDiff.rx() = mousePos.x() - mousePressPoint_.x();
        sizeDiff.rheight() = mousePos.y() - mousePressPoint_.y();
        mousePressPoint_.rx() = mousePos.x();
        mousePressPoint_.ry() = mousePos.y();
    }
    const QSize newSize= size() + sizeDiff;
    const QSize minSize = minimumSizeHint();
    if (newSize.width() >= minSize.width() &&
            newSize.height() >= minSize.height())
    {
        resize(newSize);
        move(pos() + posDiff);
    }
}

QSize SecondaryWindowGenericImplementation::minimumSizeHint() const
{
    int w = 200;
    int h = 120;
    if (centralWidget_) {
        int minWidgetW = centralWidget_->minimumSizeHint().width();
        int minWidgetH = centralWidget_->minimumSizeHint().height();
        w = qMax(w,
                 minWidgetW +
                 layout()->contentsMargins().left() +
                 layout()->contentsMargins().right()
                 );
        h = qMax(h,
                 minWidgetH +
                 layout()->contentsMargins().top() +
                 layout()->contentsMargins().bottom() +
                 layout()->spacing() +
                 titleBox_->height()
                 );
    }
    return QSize(w, h);
}

void SecondaryWindowGenericImplementation::notifyOnDocked()
{
    if (centralWidget_) {
        QObject * obj = centralWidget_;
        QMetaObject::invokeMethod(obj, "setDock", Q_ARG(bool, true));
    }
}

void SecondaryWindowGenericImplementation::handleResizeRequest(const QSize &newSize)
{
    if (centralWidget_) {
        int screenNumber = QApplication::desktop()->screenNumber(this);
        centralWidget_->resize(newSize);
        QSize sz = newSize;
        sz.rwidth() += layout()->contentsMargins().left() +
                layout()->contentsMargins().right();
        sz.rheight() += layout()->contentsMargins().top() +
                layout()->contentsMargins().bottom() +
                layout()->spacing() +
                titleBox_->height();
        resize(sz);

        const QRect screen = QApplication::desktop()->screenGeometry(screenNumber);
        int dx = qMax(0, rect().right() - screen.right());
        int dy = qMax(0, rect().bottom() - screen.bottom());
        if (dx > 0 || dy > 0) {
            move(x() + dx, y() + dy);
        }
    }
}

} // namespace Widgets
