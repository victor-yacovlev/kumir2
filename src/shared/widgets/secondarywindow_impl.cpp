#include "secondarywindow_impl.h"
#include "secondarywindow.h"

#include "dockwindowplace.h"
#include "dockwindowplace_impl.h"

#include <QtGui>

namespace Widgets {

static const char Left = 'l';
static const char Right = 'r';
static const char Top = 't';
static const char Bottom = 'b';

// class SecondaryWindowPrivate

void SecondaryWindowImpl::init(QWidget *centralWidget,
                                  QSharedPointer<DockWindowPlaceImpl> placeWidget,
                                  QMainWindow * mainWindow,
                                  ExtensionSystem::SettingsPtr settings,
                               const QString &settingsKey,
                               bool resizableX, bool resizableY)
{
    floating_ = true;
    visibleFlag_ = false;
    centralWidget_ = centralWidget;
    dockPlace_ = placeWidget;
    mainWindow_ = mainWindow;
    settings_ = settings;
    settingsKey_ = settingsKey;
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->setContentsMargins(0,0,0,0);
    l0->setSpacing(0);
    pClass_->setLayout(l0);

    QString pixmapsPath = qApp->property("sharePath").toString()+"/widgets/secondarywindow/";

    QString themeName = qApp->style()->objectName().toLower();
    if (themeName.contains(" "))
        themeName = themeName.split(" ").at(0);

    if (QDir(pixmapsPath+themeName).exists())
        pixmapsPath += themeName+"/";
    else
        pixmapsPath += "generic/";

    for (int i=0; i<6; i++) {
        if (i<3) {
            topBorderPixmaps_ << new QPixmap(pixmapsPath+QString("top%1.png").arg(i));
            topDockedBorderPixmaps_ << new QPixmap(pixmapsPath+QString("top_docked%1.png").arg(i));
            bottomBorderPixmaps_ << new QPixmap(pixmapsPath+QString("bottom%1.png").arg(i));
            leftBorderPixmaps_ << new QPixmap(pixmapsPath+QString("left%1.png").arg(i));
            rightBorderPixmaps_ << new QPixmap(pixmapsPath+QString("right%1.png").arg(i));
        }


        stayOnTopButtonPixmaps_ << new QPixmap(pixmapsPath+QString("stayontop%1.png").arg(i));

        if (i<3) {
            minimizeButtonPixmaps_ << new QPixmap(pixmapsPath+QString("minimize%1.png").arg(i));
            toggleDockedButtonPixmaps_ << new QPixmap(pixmapsPath+QString("dock%1.png").arg(i));
            closeButtonPixmaps_ << new QPixmap(pixmapsPath+QString("close%1.png").arg(i));
        }
    }


    topBorder_ = new BorderWidget(pClass_, pClass_, this, Top, topBorderPixmaps_, false);

    l0->addWidget(topBorder_);

    QWidget *www = new QWidget;
    QHBoxLayout *l1 = new QHBoxLayout;
    l1->setContentsMargins(0,0,0,0);
    l1->setSpacing(0);
    www->setLayout(l1);
    l0->addWidget(www);

    leftBorder_ = new BorderWidget(pClass_, pClass_, this, Left, leftBorderPixmaps_, resizableX);
    l1->addWidget(leftBorder_);
    l1->addWidget(centralWidget_);

    rightBorder_ = new BorderWidget(pClass_, pClass_, this, Right, rightBorderPixmaps_, resizableX);
    l1->addWidget(rightBorder_);

    bottomBorder_ = new BorderWidget(pClass_, pClass_, this, Bottom, bottomBorderPixmaps_, resizableY);
    l0->addWidget(bottomBorder_);

    QHBoxLayout *ll = new QHBoxLayout;
    ll->setContentsMargins(4, 0, 4, 0);
    ll->setAlignment(Qt::AlignVCenter);
    topBorder_->setLayout(ll);

    buttonStayOnTop_ = new SecondaryWindowButton(topBorder_, true,
                                     "Stay on top",
                                     QStyle::SP_TitleBarShadeButton,
                                     stayOnTopButtonPixmaps_,
                                     tr("Toggle stay on top"));

    QObject::connect ( buttonStayOnTop_, SIGNAL(clicked(bool)),
                       pClass_, SLOT( setStayOnTop(bool)) );



    QFile labelStyleFile(pixmapsPath+"title.properties");
    if (labelStyleFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&labelStyleFile);
        ts.setAutoDetectUnicode(true);
        windowTitleStylesheet_ = ts.readAll();
        labelStyleFile.close();
    }

    QFile labelStyleFile2(pixmapsPath+"title_docked.properties");
    if (labelStyleFile2.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&labelStyleFile2);
        ts.setAutoDetectUnicode(true);
        dockedWindowTitleStylesheet_ = ts.readAll();
        labelStyleFile2.close();
    }



    buttonMinimize_ = new SecondaryWindowButton(topBorder_, false,
                                    "Minimize",
                                    QStyle::SP_TitleBarMinButton,
                                    minimizeButtonPixmaps_,
                                    tr("Minimize"));

    QObject::connect ( buttonMinimize_, SIGNAL(clicked()),
                       pClass_, SLOT(showMinimized()) );


    if (placeWidget) {
        buttonToggleDocked_ = new SecondaryWindowButton(topBorder_, false,
                                        "Docking",
                                        QStyle::SP_TitleBarNormalButton,
                                        toggleDockedButtonPixmaps_,
                                        tr("Docking"));

        QObject::connect ( buttonToggleDocked_, SIGNAL(clicked()),
                           pClass_, SLOT(toggleDocked()) );

    }

    buttonClose_ = new SecondaryWindowButton(topBorder_, false,
                                 "Close",
                                 QStyle::SP_TitleBarCloseButton,
                                 closeButtonPixmaps_,
                                 tr("Close"));

    QObject::connect(buttonClose_, SIGNAL(clicked()),
                     pClass_, SLOT(close()));

#ifndef Q_OS_MAC
    // Buttons at right to title
    ll->addWidget(buttonStayOnTop_);
    ll->addStretch();
    ll->addWidget(buttonMinimize_);
    if (placeWidget)
        ll->addWidget(buttonToggleDocked_);
    ll->addWidget(buttonClose_);
#else
    // Buttons at left to title
    ll->addWidget(buttonClose_);
    if (placeWidget)
        ll->addWidget(buttonToggleDocked_);
    ll->addWidget(buttonMinimize_);
    ll->addStretch();
    ll->addWidget(buttonStayOnTop_);
#endif

    int maxButtonHeight = qMax(buttonStayOnTop_->height(),
                               qMax(buttonMinimize_->height(), buttonClose_->height()));
    int topBorderHeight = topBorder_->height();
    topBorder_->setFixedHeight(qMax(maxButtonHeight, topBorderHeight));

    toggleVisibleAction_ = new QAction(this);
    toggleVisibleAction_->setCheckable(false);
    connect(toggleVisibleAction_, SIGNAL(triggered()), pClass_, SLOT(activate()));

    startTimer(500);

    QRect r = settings->value("Windows/"+settingsKey_+"/Geometry",
                                          QRect(-1,-1,-1,-1)).toRect();
    if (r.width()>0 && r.height()>0) {
        pClass_->resize(r.size());
        pClass_->move(r.topLeft());
    }
}

void SecondaryWindowImpl::setFloating(bool v)
{
    if (v) {
        QPoint ps = pClass_->mapToGlobal(pClass_->pos());
        bottomBorder_->setVisible(true);
        leftBorder_->setVisible(true);
        rightBorder_->setVisible(true);
        buttonStayOnTop_->setVisible(true);
        buttonMinimize_->setVisible(true);
        topBorder_->switchPixmaps(topBorderPixmaps_);
        dockPlace_->undockWindow(pClass_);
        setParent(D_PARENT);
        pClass_->setWindowFlags(pClass_->windowFlags() | Qt::Window);
        pClass_->move(ps);
        floating_ = true;
    }
    else {
        setParent(dockPlace_->widget());
        dockPlace_->dockWindow(pClass_, visibleFlag_);
        if (pClass_->windowFlags() & Qt::Window)
            pClass_->setWindowFlags(pClass_->windowFlags() ^ Qt::Window);
        bottomBorder_->setVisible(false);
        leftBorder_->setVisible(false);
        rightBorder_->setVisible(false);
        buttonStayOnTop_->setVisible(false);
        buttonMinimize_->setVisible(false);
        topBorder_->switchPixmaps(topDockedBorderPixmaps_);
        centralWidget_->setFixedHeight(QWIDGETSIZE_MAX);
        centralWidget_->setFixedWidth(QWIDGETSIZE_MAX);
        pClass_->setFixedHeight(QWIDGETSIZE_MAX);
        pClass_->setFixedWidth(QWIDGETSIZE_MAX);
        floating_ = false;
    }
}

void SecondaryWindowImpl::timerEvent(QTimerEvent *e)
{
    e->accept();
    if (!pClass_->isFloating())
        return;
    QString newTitle = centralWidget_->windowTitle();
    QString oldTitle = pClass_->windowTitle();
    if (newTitle!=oldTitle && !newTitle.trimmed().isEmpty()) {
        pClass_->setWindowTitle(newTitle);
    }
}

// class BorderWidget

BorderWidget::BorderWidget(QWidget *parent,
                           QWidget *window,
                           class SecondaryWindowImpl *dd,
                           char border,
                           const QList<QPixmap*> &pixmaps, bool resizable)
                               : QWidget(parent)
                               , border_(border)
                               , resizableFlag_(resizable)
                               , movingFlag_(false)
{
    pWindowImpl_ = dd;
    pixmaps_ = pixmaps;
    window_ = window;
    int minSize0, minSize1, minSize2, minSizeMax;
    if (border=='t' || border=='b') {
        minSize0 = pixmaps[0]->height();
        minSize1 = pixmaps[1]->height();
        minSize2 = pixmaps[2]->height();
    }
    else {
        minSize0 = pixmaps[0]->width();
        minSize1 = pixmaps[1]->width();
        minSize2 = pixmaps[2]->width();
    }
    minSizeMax = qMax(minSize0, qMax(minSize1, minSize2));
    if (border=='t' || border=='b') {
        setFixedHeight(minSizeMax);
    }
    else {
        setFixedWidth(minSizeMax);
    }
    if (resizable && (border=='l' || border=='r')) {
        setCursor(Qt::SizeHorCursor);
    }
    else if (resizable && border=='b') {
        setCursor(Qt::SizeVerCursor);
    }
}

void BorderWidget::switchPixmaps(const QList<QPixmap *> &pixmaps)
{
    pixmaps_ = pixmaps;
    update();
}

void BorderWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    if (border_=='t') {
        QStyleOptionTitleBar opt;
        opt.initFrom(this);
        opt.titleBarFlags = Qt::Window
                | Qt::WindowTitleHint;
        opt.text = parentWidget()->windowTitle();
#ifdef Q_WS_WIN
        if (parentWidget()->isTopLevel())
            opt.text = "      " + opt.text;
#endif
        if (isActiveWindow() && pWindowImpl_->pClass_->isFloating()) {
            opt.state |= QStyle::State_Active;
            opt.titleBarState |= QStyle::State_Active;
            opt.palette.setCurrentColorGroup(QPalette::Active);
        }
        else {
            opt.state &= ~QStyle::State_Active;
            opt.titleBarState &= ~QStyle::State_Active;
            opt.palette.setCurrentColorGroup(QPalette::Inactive);
        }
#ifndef APPLE
        style()->drawComplexControl(QStyle::CC_TitleBar, &opt, &p, this);
#else
        p.setPen(Qt::NoPen);
        p.setBrush(palette().brush(QPalette::Window));
        p.drawRect(rect());
        p.setPen(QPen(Qt::black));
        QFontMetrics fm(font());
        int titleWidth = fm.width(parentWidget()->windowTitle());
        int titleHeight = fm.height();
        p.setFont(font());
        int textX = (width() - titleWidth) / 2;
        int textY = titleHeight + (height() - titleHeight) / 2 - 2;
        p.drawText(textX, textY, parentWidget()->windowTitle());
#endif
    }
    e->accept();
}

void SecondaryWindow::paintEvent(QPaintEvent *e)
{
#ifdef Q_WS_X11
    QPainter p(this);
    QStyleOptionFrame opt;
    opt.initFrom(this);
    opt.rect.setY(opt.rect.y()+pImpl_->topBorder_->height());
    if (isActiveWindow()) {
        opt.state |= QStyle::State_Active;
    }
    else {
        opt.state &= ~QStyle::State_Active;
    }
    style()->drawPrimitive(QStyle::PE_FrameWindow, &opt, &p, this);
#endif
#ifdef Q_WS_WIN
    QPainter p(this);
    p.setPen(QPen(QColor(Qt::black)));
    p.drawLine(0, 16, 0, height());
    p.drawLine(width()-1, 16, width()-1, height());
    p.drawLine(0, height()-1, width()-1, height()-1);
#endif
    QWidget::paintEvent(e);
}

void BorderWidget::mousePressEvent(QMouseEvent *e)
{
    if (!pWindowImpl_->pClass_->isFloating()) {
        e->ignore();
        return;
    }
    if (border_=='t') {
        setCursor(Qt::SizeAllCursor);
        movingFlag_  = true;
        p_start = e->globalPos();
        e->accept();
    }
    else if (resizableFlag_) {
        movingFlag_  = true;
        p_start = e->globalPos();
        e->accept();
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (movingFlag_) {
        QPoint d = e->globalPos() - p_start;
        int distance = (int) ( sqrt( (double) (d.x()*d.x() + d.y()*d.y()) ) );
        p_start = e->globalPos();
        if ( movingFlag_ || distance >= QApplication::startDragDistance() ) {
            if (mouseDragAction(d)) {
                e->accept();
            }
            else {
                e->ignore();
            }
        }
        else {
            e->ignore();
        }
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (movingFlag_) {
        movingFlag_ = false;
        if (border_=='t')
            unsetCursor();
        e->accept();
    }
    else {
        e->ignore();
    }
}

bool BorderWidget::mouseDragAction(const QPoint &offset)
{
    const QSize minSize = pWindowImpl_->pClass_->minimumSizeHint();
    if (resizableFlag_) {
        Q_CHECK_PTR(window_);
        if (border_=='l') {
            if (window_->width()-offset.x() >= minSize.width()) {
                QPoint newPos = window_->pos();
                newPos.setX(newPos.x()+offset.x());
                window_->move(newPos);
                window_->setFixedWidth(window_->width()-offset.x());
                pWindowImpl_->centralWidget_->setFixedWidth(window_->width()-
                                                  pWindowImpl_->leftBorder_->width()-
                                                  pWindowImpl_->rightBorder_->width());
            }
            else {
                return false;
            }
        }
        if (border_=='r') {
            if (window_->width() + offset.x() >= minSize.width()) {
                window_->setFixedWidth(window_->width()+offset.x());
                pWindowImpl_->centralWidget_->setFixedWidth(window_->width()-
                                                  pWindowImpl_->leftBorder_->width()-
                                                  pWindowImpl_->rightBorder_->width());
            }
            else {
                return false;
            }
        }
        if (border_=='b') {
            if (window_->height() + offset.y() >= minSize.height()) {
                window_->setFixedHeight(window_->height()+offset.y());
                pWindowImpl_->centralWidget_->setFixedHeight(window_->height()-
                                                   pWindowImpl_->topBorder_->height()-
                                                   pWindowImpl_->bottomBorder_->height());
            }
            else {
                return false;
            }
        }
        update();
    }
    else if (border_=='t') {
        Q_CHECK_PTR(window_);
        window_->move(window_->pos()+offset);
    }
    return true;
}

SecondaryWindowButton::SecondaryWindowButton(QWidget *parent, bool checkable,
                           const QString &name,
                           QStyle::StandardPixmap role,
                           const QList<QPixmap*> &pixmaps,
                           const QString &toolTip)
                               : QAbstractButton(parent)
{
    setText(name);
    setCheckable(checkable);
    setToolTip(toolTip);
    setMouseTracking(true);
    pixmaps_ = pixmaps;
    pressedFlag_ = hoveredFlag_ = false;
    role_ = role;
    int maxW = 0, maxH = 0;
    for (int i=0; i<pixmaps.size(); i++) {
        maxW = qMax(maxW, pixmaps[i]->width());
        maxH = qMax(maxH, pixmaps[i]->height());
    }
    setFixedSize(maxW, maxH);
    setMouseTracking(true);
}

void SecondaryWindowButton::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    int index;
    if (isPressed()) {
        index = 2;
    }
    else if (isHovered()) {
        index = 1;
    }
    else {
        index = 0;
    }
    if (isCheckable()) {
        if (!isChecked())
            index += 3;
    }

    if (index<pixmaps_.size()) {
        QPixmap *pixmap = pixmaps_[index];
        Q_CHECK_PTR(pixmap);
        p.drawPixmap(0, 0, pixmap->width(), pixmap->height(), *pixmap);
        e->accept();
    }
    else {
        e->ignore();
    }
}

void SecondaryWindowButton::enterEvent(QEvent *e)
{
    e->accept();
    hoveredFlag_ = true;
    update();
}

void SecondaryWindowButton::leaveEvent(QEvent *e)
{
    e->accept();
    hoveredFlag_ = false;
    update();
}

void SecondaryWindowButton::mousePressEvent(QMouseEvent *e)
{
    pressedFlag_ = true;
    QAbstractButton::mousePressEvent(e);
}

void SecondaryWindowButton::mouseReleaseEvent(QMouseEvent *e)
{
    pressedFlag_ = false;
    QAbstractButton::mouseReleaseEvent(e);
}

} // namespace Widgets
