#include "secondarywindow.h"

namespace Widgets {

class SecondaryWindow;

#define Left 'l'
#define Right 'r'
#define Top 't'
#define Bottom 'b'

#ifdef Q_OS_MAC
#define STAY_ON_TOP_FLAGS Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Window
#define REGULAR_FLAGS Qt::FramelessWindowHint|Qt::Window
#define PARENT mainWindow
#define D_PARENT pImpl_->mainWindow_
#else
#define STAY_ON_TOP_FLAGS Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Window
#define REGULAR_FLAGS Qt::FramelessWindowHint|Qt::Window
#define PARENT mainWindow
#define D_PARENT pImpl_->mainWindow_
#endif


class SecondaryWindowPrivate :
        public QObject
{
public:
    void init(QWidget *centralWidget,
              QWidget *dockPlace,
              QMainWindow * mainWindow,
              QSettings * settings,
              const QString &settingsKey,
              bool resizableX, bool resizableY);

    QWidget* centralWidget_;
    QWidget* dockPlace_;
    QMainWindow * mainWindow_;

    class BorderWidget* topBorder_;
    class BorderWidget* bottomBorder_;
    class BorderWidget* leftBorder_;
    class BorderWidget* rightBorder_;

    QAbstractButton* buttonStayOnTop_;
    QAbstractButton* buttonMinimize_;
    QAbstractButton* buttonToggleDocked_;
    QAbstractButton* buttonClose_;

    QAction* toggleVisibleAction_;

    QList<QPixmap*> topBorderPixmaps_;
    QList<QPixmap*> topDockedBorderPixmaps_;
    QList<QPixmap*> bottomBorderPixmaps_;
    QList<QPixmap*> leftBorderPixmaps_;
    QList<QPixmap*> rightBorderPixmaps_;

    QList<QPixmap*> stayOnTopButtonPixmaps_;
    QList<QPixmap*> minimizeButtonPixmaps_;
    QList<QPixmap*> toggleDockedButtonPixmaps_;
    QList<QPixmap*> closeButtonPixmaps_;

    QString windowTitleStylesheet_;
    QString dockedWindowTitleStylesheet_;

    SecondaryWindow *pClass_;
    bool visibleFlag_;
    QString settingsKey_;
    QSettings * settings_;

    void timerEvent(QTimerEvent *);
};


class BorderWidget :
        public QWidget
{
public:
    explicit BorderWidget(
            QWidget *parent
            , QWidget *window
            , class SecondaryWindowPrivate *q
            , char border
            , const QList<QPixmap*> &pixmaps
            , bool resizable
            );
    void switchPixmaps(const QList<QPixmap*> & pixmaps);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

    void mouseDragAction(const QPoint &offset);

    char border_;
    QList<QPixmap*> pixmaps_;
    bool resizableFlag_;
    bool movingFlag_;
    QPoint p_start;
    class SecondaryWindowPrivate *pWindowImpl_;
    QWidget *window_;
};

class SecondaryWindowButton :
        public QAbstractButton
{
public:
    explicit SecondaryWindowButton(QWidget *parent, bool checkable, const QString &name, QStyle::StandardPixmap role,
                          const QList<QPixmap*> &pixmaps, const QString &toolTip);
    inline bool isHovered() const { return hoveredFlag_; }
    inline bool isPressed() const { return pressedFlag_; }
    inline void forceUnhighlight() { pressedFlag_ = hoveredFlag_ = false; update(); }
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent (QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool hoveredFlag_;
    bool pressedFlag_;
    QList<QPixmap*> pixmaps_;
    QStyle::StandardPixmap role_;

};

class SecondaryWindowTitle :
        public QWidget
{
public:
    explicit SecondaryWindowTitle(QWidget *parent, const QString &style);
    void setStyle(const QString & style);
    void setText(const QString &v);
    inline QString text() const { return titleText_; }
protected:
    void paintEvent(QPaintEvent *e);
    QFont font_;
    QColor foregroundColor_;
    Qt::AlignmentFlag textAlignment_;
    int padding_;
    QString titleText_;

};


SecondaryWindow::SecondaryWindow(QWidget *centralComponent,
                                 QWidget * dockPlace,
                                 QMainWindow * mainWindow,
                                 QSettings * settings,
                           const QString &settingsKey,
                           bool resizableX, bool resizableY) :

        QWidget(PARENT, REGULAR_FLAGS)
{
    pImpl_ = new SecondaryWindowPrivate;
    pImpl_->pClass_ = this;
    pImpl_->init(centralComponent,
                 dockPlace,
                 mainWindow,
                 settings, settingsKey,
                 resizableX, resizableY);
    if (dockPlace)
        dockPlace->installEventFilter(this);
    connect(pImpl_->toggleVisibleAction_, SIGNAL(triggered(bool)),
            this, SLOT(checkForPlaceVisible(bool)));
}

bool SecondaryWindow::eventFilter(QObject *obj, QEvent *evt)
{
//    if (obj == pImpl_->dockPlace_ && !isFloating()) {

//        // If splitter changed dock place to hide, then
//        // there is MoveEvent to (-1, something)
//        // TODO check this assumption for various Qt versions!
//        if (evt->type() == QEvent::Move) {
//            const QMoveEvent * event = static_cast<const QMoveEvent*>(evt);
//            const QPoint & newPos = event->pos();

//            if (newPos.x()==-1) {
//                // Matched pseudo-hide event

//                // Close window the same way as 'close' button
//                close();

//                // Prevent to process this event
//                return true;
//            }
//        }
//    }
    return QWidget::eventFilter(obj, evt);
}

void SecondaryWindow::checkForPlaceVisible(bool show)
{
    if (show) {
        if (!isFloating()) {
            pImpl_->dockPlace_->setVisible(true);
        }
    }
}

void SecondaryWindow::restoreState()
{
    bool mustDocked = pImpl_->settings_->value(
                "WindowDocked/"+pImpl_->settingsKey_,
                false
                ).toBool();

    if (isFloating() && mustDocked) {
        toggleDocked();
    }

    bool mustVisible = pImpl_->settings_->value(
                "WindowVisible/"+pImpl_->settingsKey_,
                false
                ).toBool();

    if (!isFloating()) {
        setVisible(mustVisible);
        pImpl_->dockPlace_->setVisible(mustVisible);
    }
}

void SecondaryWindow::saveState()
{
    const QRect r(x(), y(), width(), height());

    if (r.width()>0 && r.height()>0) {
        pImpl_->settings_->setValue(
                    "Windows/"+pImpl_->settingsKey_+"/Geometry", r);
    }

    if (pImpl_->dockPlace_) {

        pImpl_->settings_->setValue("DockPlace/"+pImpl_->settingsKey_+"/Size",
                                    pImpl_->dockPlace_->size());

        pImpl_->settings_->setValue("WindowDocked/"+pImpl_->settingsKey_,
                                    !isFloating());
    }

    pImpl_->settings_->setValue("WindowVisible/"+pImpl_->settingsKey_,
                                pImpl_->visibleFlag_);
}

QSize SecondaryWindow::minimumSizeHint() const
{
    const QSize minimumChildSize = pImpl_->centralWidget_->minimumSizeHint();

    const QSize myMinimumSize = minimumChildSize +
            QSize(pImpl_->leftBorder_->width(), 0) +
            QSize(pImpl_->rightBorder_->width(), 0) +
            QSize(0, pImpl_->topBorder_->height()) +
            QSize(0, pImpl_->bottomBorder_->height());

    // Reasonable minimum size in case if there is no size hints
    const QSize reasonableMinumumSize = QSize(
                qMax(300, myMinimumSize.width()),
                qMax(200, myMinimumSize.height())
                );

    return reasonableMinumumSize;
}

void SecondaryWindow::setWindowTitle(const QString &title)
{
    pImpl_->toggleVisibleAction_->setText(title);
    QWidget::setWindowTitle(title);
}

QString SecondaryWindow::windowTitle()
{
    return QWidget::windowTitle();
}

void SecondaryWindow::setStayOnTop(bool v)
{
    if (v) {
        setWindowFlags(STAY_ON_TOP_FLAGS);
        setVisible(true);
    }
    else {
        setWindowFlags(REGULAR_FLAGS);
        setVisible(true);
    }
    pImpl_->centralWidget_->setFocus();
}


bool SecondaryWindow::isStayOnTop()
{
    return windowFlags() & Qt::WindowStaysOnTopHint;
}

void SecondaryWindow::close()
{  
    setVisible(false);
    if (!isFloating()) {
        pImpl_->dockPlace_->setVisible(false);
    }
}

void SecondaryWindow::closeEvent(QCloseEvent *e)
{
    QRect r(x(), y(), width(), height());
    if (r.width()>0 && r.height()>0) {
        pImpl_->settings_->setValue(
                    "Windows/"+pImpl_->settingsKey_+"/Geometry", r);
    }

    if (pImpl_->dockPlace_) {

        pImpl_->settings_->setValue("DockPlace/"+pImpl_->settingsKey_+"/Size",
                                    pImpl_->dockPlace_->size());

        pImpl_->settings_->setValue("WindowDocked/"+pImpl_->settingsKey_,
                                    false);
    }

    pImpl_->visibleFlag_ = false;
    QWidget::closeEvent(e);
}


QAction * SecondaryWindow::toggleViewAction() const
{
    return pImpl_->toggleVisibleAction_;
}


void SecondaryWindow::showMinimized()
{
    QWidget::showMinimized();
}

void SecondaryWindow::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    pImpl_->centralWidget_->setVisible(visible);
    if (visible)
        pImpl_->centralWidget_->setFocus();
    else {
        QRect r(x(), y(), width(), height());
        if (r.width()>0 && r.height()>0)
            pImpl_->settings_->setValue("Windows/"+pImpl_->settingsKey_+"/Geometry",r);
        if (pImpl_->dockPlace_) {
            pImpl_->settings_->setValue("DockPlace/"+pImpl_->settingsKey_+"/Size", pImpl_->dockPlace_->size());
            pImpl_->settings_->setValue("WindowDocked/"+pImpl_->settingsKey_, !isFloating());
            if (!isFloating())
                pImpl_->dockPlace_->setVisible(false);
        }
    }
    pImpl_->toggleVisibleAction_->setChecked(visible);
    pImpl_->settings_->setValue("WindowVisible/"+pImpl_->settingsKey_, visible);
    pImpl_->visibleFlag_ = visible; // required for saving state
}

bool SecondaryWindow::isFloating() const
{
    return parentWidget()==0 || parentWidget()==pImpl_->mainWindow_;
}

void SecondaryWindow::toggleDocked()
{
    if (!pImpl_->dockPlace_)
        return;
    if (!pImpl_->dockPlace_->layout()) {
        QHBoxLayout * l = new QHBoxLayout(pImpl_->dockPlace_);
        l->setContentsMargins(0,0,0,0);
        pImpl_->dockPlace_->setLayout(l);
    }
    pImpl_->dockPlace_->layout()->setContentsMargins(0,0,0,0);
    pImpl_->dockPlace_->layout()->setSpacing(0);
    bool wasVisible = isVisible();
    bool wasVisibleFlag = pImpl_->settings_->value("WindowVisible/"+pImpl_->settingsKey_, false).toBool();
    if (isFloating()) {
        setVisible(false);
        pImpl_->bottomBorder_->setVisible(false);
        pImpl_->leftBorder_->setVisible(false);
        pImpl_->rightBorder_->setVisible(false);
        pImpl_->buttonStayOnTop_->setVisible(false);
        pImpl_->buttonMinimize_->setVisible(false);
        pImpl_->topBorder_->switchPixmaps(pImpl_->topDockedBorderPixmaps_);
        QSize dockSize = pImpl_->settings_->value("DockPlace/"+pImpl_->settingsKey_+"/Size",
                                              size()
                                              ).toSize();
        pImpl_->dockPlace_->setVisible(wasVisible);
        setParent(pImpl_->dockPlace_);
        pImpl_->dockPlace_->layout()->addWidget(this);
        pImpl_->centralWidget_->setFixedHeight(QWIDGETSIZE_MAX);
        pImpl_->centralWidget_->setFixedWidth(QWIDGETSIZE_MAX);
        setFixedHeight(QWIDGETSIZE_MAX);
        setFixedWidth(QWIDGETSIZE_MAX);
        setVisible(wasVisible);
        pImpl_->dockPlace_->resize(dockSize);
        pImpl_->settings_->setValue("WindowDocked/"+pImpl_->settingsKey_, true);
    }
    else {
        pImpl_->settings_->setValue("DockPlace/"+pImpl_->settingsKey_+"/Size", pImpl_->dockPlace_->size());
        QPoint ps = mapToGlobal(pos());
        pImpl_->bottomBorder_->setVisible(true);
        pImpl_->leftBorder_->setVisible(true);
        pImpl_->rightBorder_->setVisible(true);
        pImpl_->buttonStayOnTop_->setVisible(true);
        pImpl_->buttonMinimize_->setVisible(true);
        pImpl_->topBorder_->switchPixmaps(pImpl_->topBorderPixmaps_);

        pImpl_->dockPlace_->layout()->removeWidget(this);
        setParent(D_PARENT);
        setWindowFlags(windowFlags() | Qt::Window);
        move(ps);
        setVisible(wasVisible);
        pImpl_->dockPlace_->setVisible(false);
        pImpl_->settings_->setValue("WindowDocked/"+pImpl_->settingsKey_, false);
    }
    pImpl_->settings_->setValue("WindowVisible/"+pImpl_->settingsKey_, wasVisibleFlag); // required for restoring state
    pImpl_->visibleFlag_ = wasVisibleFlag; // required for restoring state
    SecondaryWindowButton * btn = static_cast<SecondaryWindowButton*>(pImpl_->buttonToggleDocked_);
    btn->forceUnhighlight();
}

void SecondaryWindow::setSettingsObject(QSettings *settings)
{
    pImpl_->settings_ = settings;
    restoreState();
}

// class SecondaryWindowPrivate

void SecondaryWindowPrivate::init(QWidget *centralWidget,
                                  QWidget *placeWidget,
                                  QMainWindow * mainWindow,
                                  QSettings * settings,
                               const QString &settingsKey,
                               bool resizableX, bool resizableY)
{
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
                                     pClass_->tr("Toggle stay on top"));

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
                                    pClass_->tr("Minimize"));

    QObject::connect ( buttonMinimize_, SIGNAL(clicked()),
                       pClass_, SLOT(showMinimized()) );


    if (placeWidget) {
        buttonToggleDocked_ = new SecondaryWindowButton(topBorder_, false,
                                        "Docking",
                                        QStyle::SP_TitleBarNormalButton,
                                        toggleDockedButtonPixmaps_,
                                        pClass_->tr("Docking"));

        QObject::connect ( buttonToggleDocked_, SIGNAL(clicked()),
                           pClass_, SLOT(toggleDocked()) );

    }

    buttonClose_ = new SecondaryWindowButton(topBorder_, false,
                                 "Close",
                                 QStyle::SP_TitleBarCloseButton,
                                 closeButtonPixmaps_,
                                 pClass_->tr("Close"));

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
    toggleVisibleAction_->setCheckable(true);
    connect(toggleVisibleAction_, SIGNAL(triggered(bool)), pClass_, SLOT(setVisible(bool)));

    startTimer(500);

    QRect r = settings->value("Windows/"+settingsKey_+"/Geometry",
                                          QRect(-1,-1,-1,-1)).toRect();
    if (r.width()>0 && r.height()>0) {
        pClass_->resize(r.size());
        pClass_->move(r.topLeft());
    }
    if (dockPlace_)
        dockPlace_->setMinimumWidth(centralWidget_->minimumWidth());
}

void SecondaryWindowPrivate::timerEvent(QTimerEvent *e)
{
    e->accept();
    if (!pClass_->isFloating())
        return;
    QString newTitle = centralWidget_->windowTitle();
    QString oldTitle = pClass_->windowTitle();
    if (newTitle!=oldTitle && !newTitle.trimmed().isEmpty()) {
        pClass_->setWindowTitle(newTitle);
    }
//    QSize newSize;
//    newSize.setWidth(w_leftBorder->width()+
//                     w_centralWidget->width()+
//                     w_rightBorder->width());
//    newSize.setHeight(w_topBorder->height()+
//                      w_centralWidget->height()+
//                      w_bottomBorder->height());
//    if (q->size()!=newSize) {
//        q->resize(newSize);
//    }
//    qDebug() << "Custom window central widget size: " << w_centralWidget->size();
//    qDebug() << "This size" << q->size();
}

// class BorderWidget

BorderWidget::BorderWidget(QWidget *parent,
                           QWidget *window,
                           class SecondaryWindowPrivate *dd,
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
        if (isActiveWindow()) {
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
            mouseDragAction(d);
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

void BorderWidget::mouseDragAction(const QPoint &offset)
{
    if (resizableFlag_) {
        Q_CHECK_PTR(window_);
        if (border_=='l') {
            QPoint newPos = window_->pos();
            newPos.setX(newPos.x()+offset.x());
            window_->move(newPos);
            window_->setFixedWidth(window_->width()-offset.x());
            pWindowImpl_->centralWidget_->setFixedWidth(window_->width()-
                                              pWindowImpl_->leftBorder_->width()-
                                              pWindowImpl_->rightBorder_->width());
        }
        if (border_=='r') {
            window_->setFixedWidth(window_->width()+offset.x());
            pWindowImpl_->centralWidget_->setFixedWidth(window_->width()-
                                              pWindowImpl_->leftBorder_->width()-
                                              pWindowImpl_->rightBorder_->width());
        }
        if (border_=='b') {
            window_->setFixedHeight(window_->height()+offset.y());
            pWindowImpl_->centralWidget_->setFixedHeight(window_->height()-
                                               pWindowImpl_->topBorder_->height()-
                                               pWindowImpl_->bottomBorder_->height());
        }
        update();
    }
    else if (border_=='t') {
        Q_CHECK_PTR(window_);
        window_->move(window_->pos()+offset);
    }
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

}
