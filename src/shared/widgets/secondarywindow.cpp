#include "secondarywindow.h"

namespace Widgets {

class SecondaryWindow;

#define Left 'l'
#define Right 'r'
#define Top 't'
#define Bottom 'b'

class SecondaryWindowPrivate :
        public QObject
{
public:
    void init(QWidget *centralWidget,
              QWidget *dockPlace,
              QSettings * settings,
              const QString &settingsKey,
              bool resizableX, bool resizableY);

    QWidget* w_centralWidget;
    QWidget* w_dockPlace;

    class BorderWidget* w_topBorder;
    class BorderWidget* w_bottomBorder;
    class BorderWidget* w_leftBorder;
    class BorderWidget* w_rightBorder;

    QAbstractButton* btn_stayOnTop;
    QAbstractButton* btn_minimize;
    QAbstractButton* btn_toggleDocked;
    QAbstractButton* btn_close;

    QAction* a_toggleVisible;

    class SecondaryWindowTitle *lbl_title;

    QList<QPixmap*> px_topBorder;
    QList<QPixmap*> px_topDockedBorder;
    QList<QPixmap*> px_bottomBorder;
    QList<QPixmap*> px_leftBorder;
    QList<QPixmap*> px_rightBorder;

    QList<QPixmap*> px_stayOnTopButton;
    QList<QPixmap*> px_minimizeButton;
    QList<QPixmap*> px_toggleDocked;
    QList<QPixmap*> px_closeButton;

    QString css_title;
    QString css_titleDocked;

    SecondaryWindow *q;
    QString s_settingsKey;
    QSettings * m_settings;

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

    char e_border;
    QList<QPixmap*> px_border;
    bool b_resizable;
    bool b_moving;
    QPoint p_start;
    class SecondaryWindowPrivate *d;
    QWidget *w_window;
};

class SecondaryWindowButton :
        public QAbstractButton
{
public:
    explicit SecondaryWindowButton(QWidget *parent, bool checkable, const QString &name,
                          const QList<QPixmap*> &pixmaps, const QString &toolTip);
    inline bool isHovered() const { return b_hovered; }
    inline bool isPressed() const { return b_pressed; }
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent (QPaintEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool b_hovered;
    bool b_pressed;
    QList<QPixmap*> px;
};

class SecondaryWindowTitle :
        public QWidget
{
public:
    explicit SecondaryWindowTitle(QWidget *parent, const QString &style);
    void setStyle(const QString & style);
    void setText(const QString &v);
    inline QString text() const { return s_text; }
protected:
    void paintEvent(QPaintEvent *e);
    QFont f_font;
    QColor col_foreground;
    Qt::AlignmentFlag fl_textAlignment;
    int i_padding;
    QString s_text;

};


SecondaryWindow::SecondaryWindow(QWidget *centralComponent,
                                 QWidget * dockPlace,
                                 QSettings * settings,
                           const QString &settingsKey,
                           bool resizableX, bool resizableY) :
        QWidget(0, Qt::FramelessWindowHint)
{
    d = new SecondaryWindowPrivate;
    d->q = this;
    d->init(centralComponent, dockPlace, settings, settingsKey, resizableX, resizableY);

}

void SecondaryWindow::setWindowTitle(const QString &title)
{
    d->lbl_title->setText(title);
    d->a_toggleVisible->setText(title);
    QWidget::setWindowTitle(title);
}

QString SecondaryWindow::windowTitle()
{
    return d->lbl_title->text();
}

void SecondaryWindow::setStayOnTop(bool v)
{
    if (v) {
        setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
        setVisible(true);
    }
    else {
        setWindowFlags(Qt::FramelessWindowHint);
        setVisible(true);
    }
    d->w_centralWidget->setFocus();
}


bool SecondaryWindow::isStayOnTop()
{
    return windowFlags() & Qt::WindowStaysOnTopHint;
}

void SecondaryWindow::show()
{
    setVisible(true);
    d->w_centralWidget->setFocus(Qt::ActiveWindowFocusReason);
}

void SecondaryWindow::close()
{  
    setVisible(false);
}

void SecondaryWindow::closeEvent(QCloseEvent *e)
{
    QRect r(x(), y(), width(), height());
    if (r.width()>0 && r.height()>0)
        d->m_settings->setValue("Windows/"+d->s_settingsKey+"/Geometry",r);
    QWidget::closeEvent(e);

}

QAction * SecondaryWindow::toggleViewAction() const
{
    return d->a_toggleVisible;
}


void SecondaryWindow::showMinimized()
{
    QWidget::showMinimized();
}

void SecondaryWindow::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    d->w_centralWidget->setVisible(visible);
    if (visible)
        d->w_centralWidget->setFocus();
    d->a_toggleVisible->setChecked(visible);
}

bool SecondaryWindow::isFloating() const
{
    return parentWidget()==0;
}

void SecondaryWindow::toggleDocked()
{
    if (!d->w_dockPlace)
        return;
    if (!d->w_dockPlace->layout()) {
        QHBoxLayout * l = new QHBoxLayout(d->w_dockPlace);
        l->setContentsMargins(0,0,0,0);
        d->w_dockPlace->setLayout(l);
    }
    if (isFloating()) {
        d->w_bottomBorder->setVisible(false);
        d->w_leftBorder->setVisible(false);
        d->w_rightBorder->setVisible(false);
        d->btn_stayOnTop->setVisible(false);
        d->btn_minimize->setVisible(false);
        d->w_topBorder->switchPixmaps(d->px_topDockedBorder);
        d->lbl_title->setStyle(d->css_titleDocked);
        setParent(d->w_dockPlace);
        d->w_dockPlace->layout()->addWidget(this);

    }
    else {
        QPoint ps = mapToGlobal(pos());
        d->w_bottomBorder->setVisible(true);
        d->w_leftBorder->setVisible(true);
        d->w_rightBorder->setVisible(true);
        d->btn_stayOnTop->setVisible(true);
        d->btn_minimize->setVisible(true);
        d->w_topBorder->switchPixmaps(d->px_topBorder);
        d->lbl_title->setStyle(d->css_title);
        d->w_dockPlace->layout()->removeWidget(this);
        setParent(0);
        move(ps);
        setVisible(true);
    }
}


// class SecondaryWindowPrivate

void SecondaryWindowPrivate::init(QWidget *centralWidget,
                                  QWidget *placeWidget,
                                  QSettings * settings,
                               const QString &settingsKey,
                               bool resizableX, bool resizableY)
{
    w_centralWidget = centralWidget;
    w_dockPlace = placeWidget;
    m_settings = settings;
    s_settingsKey = settingsKey;
    QVBoxLayout *l0 = new QVBoxLayout;
    l0->setContentsMargins(0,0,0,0);
    l0->setSpacing(0);
    q->setLayout(l0);

    QString pixmapsPath = qApp->property("sharePath").toString()+"/widgets/secondarywindow/";

    QString themeName = qApp->style()->objectName().toLower();

    if (QDir(pixmapsPath+themeName).exists())
        pixmapsPath += themeName+"/";
    else
        pixmapsPath += "generic/";

    for (int i=0; i<6; i++) {
        if (i<3) {
            px_topBorder << new QPixmap(pixmapsPath+QString("top%1.png").arg(i));
            px_topDockedBorder << new QPixmap(pixmapsPath+QString("top_docked%1.png").arg(i));
            px_bottomBorder << new QPixmap(pixmapsPath+QString("bottom%1.png").arg(i));
            px_leftBorder << new QPixmap(pixmapsPath+QString("left%1.png").arg(i));
            px_rightBorder << new QPixmap(pixmapsPath+QString("right%1.png").arg(i));
        }


        px_stayOnTopButton << new QPixmap(pixmapsPath+QString("stayontop%1.png").arg(i));

        if (i<3) {
            px_minimizeButton << new QPixmap(pixmapsPath+QString("minimize%1.png").arg(i));
            px_toggleDocked << new QPixmap(pixmapsPath+QString("dock%1.png").arg(i));
            px_closeButton << new QPixmap(pixmapsPath+QString("close%1.png").arg(i));
        }
    }


    w_topBorder = new BorderWidget(q, q, this, Top, px_topBorder, false);

    l0->addWidget(w_topBorder);

    QWidget *www = new QWidget;
    QHBoxLayout *l1 = new QHBoxLayout;
    l1->setContentsMargins(0,0,0,0);
    l1->setSpacing(0);
    www->setLayout(l1);
    l0->addWidget(www);

    w_leftBorder = new BorderWidget(q, q, this, Left, px_leftBorder, resizableX);
    l1->addWidget(w_leftBorder);
    l1->addWidget(w_centralWidget);

    w_rightBorder = new BorderWidget(q, q, this, Right, px_rightBorder, resizableX);
    l1->addWidget(w_rightBorder);

    w_bottomBorder = new BorderWidget(q, q, this, Bottom, px_bottomBorder, resizableY);
    l0->addWidget(w_bottomBorder);

    QHBoxLayout *ll = new QHBoxLayout;
    ll->setContentsMargins(4, 0, 4, 0);
    ll->setAlignment(Qt::AlignVCenter);
    w_topBorder->setLayout(ll);

    btn_stayOnTop = new SecondaryWindowButton(w_topBorder, true,
                                     "Stay on top",
                                     px_stayOnTopButton,
                                     q->tr("Toggle stay on top"));

    QObject::connect ( btn_stayOnTop, SIGNAL(clicked(bool)),
                       q, SLOT( setStayOnTop(bool)) );

    ll->addWidget(btn_stayOnTop);


    QFile labelStyleFile(pixmapsPath+"title.properties");
    if (labelStyleFile.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&labelStyleFile);
        ts.setAutoDetectUnicode(true);
        css_title = ts.readAll();
        labelStyleFile.close();
    }

    QFile labelStyleFile2(pixmapsPath+"title_docked.properties");
    if (labelStyleFile2.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream ts(&labelStyleFile2);
        ts.setAutoDetectUnicode(true);
        css_titleDocked = ts.readAll();
        labelStyleFile2.close();
    }

    lbl_title = new SecondaryWindowTitle(w_topBorder, css_title);

    ll->addWidget(lbl_title);



    btn_minimize = new SecondaryWindowButton(w_topBorder, false,
                                    "Minimize",
                                    px_minimizeButton,
                                    q->tr("Minimize"));

    QObject::connect ( btn_minimize, SIGNAL(clicked()),
                       q, SLOT(showMinimized()) );

    ll->addWidget(btn_minimize);

    if (placeWidget) {
        btn_toggleDocked = new SecondaryWindowButton(w_topBorder, false,
                                        "Docking",
                                        px_toggleDocked,
                                        q->tr("Docking"));

        QObject::connect ( btn_toggleDocked, SIGNAL(clicked()),
                           q, SLOT(toggleDocked()) );

        ll->addWidget(btn_toggleDocked);
    }

    btn_close = new SecondaryWindowButton(w_topBorder, false,
                                 "Close",
                                 px_closeButton,
                                 q->tr("Close"));

    QObject::connect(btn_close, SIGNAL(clicked()),
                     q, SLOT(hide()));

    ll->addWidget(btn_close);

    int maxButtonHeight = qMax(btn_stayOnTop->height(),
                               qMax(btn_minimize->height(), btn_close->height()));
    int topBorderHeight = w_topBorder->height();
    w_topBorder->setFixedHeight(qMax(maxButtonHeight, topBorderHeight));

    a_toggleVisible = new QAction(this);
    a_toggleVisible->setCheckable(true);
    connect(a_toggleVisible, SIGNAL(triggered(bool)), q, SLOT(setVisible(bool)));

    startTimer(500);

    QRect r = settings->value("Windows/"+s_settingsKey+"/Geometry",
                                          QRect(-1,-1,-1,-1)).toRect();
    if (r.width()>0 && r.height()>0) {
        q->resize(r.size());
        q->move(r.topLeft());
    }
}

void SecondaryWindowPrivate::timerEvent(QTimerEvent *e)
{
    e->accept();
    if (!q->isFloating())
        return;
    QString newTitle = w_centralWidget->windowTitle();
    QString oldTitle = lbl_title->text();
    if (newTitle!=oldTitle && !newTitle.trimmed().isEmpty()) {
        q->setWindowTitle(newTitle);
    }
    QSize newSize;
    newSize.setWidth(w_leftBorder->width()+
                     w_centralWidget->width()+
                     w_rightBorder->width());
    newSize.setHeight(w_topBorder->height()+
                      w_centralWidget->height()+
                      w_bottomBorder->height());
    if (q->size()!=newSize) {
        q->resize(newSize);
    }
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
                               , e_border(border)
                               , b_resizable(resizable)
                               , b_moving(false)
{
    d = dd;
    px_border = pixmaps;
    w_window = window;
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
    px_border = pixmaps;
    update();
}

void BorderWidget::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    // left or top corner
    int first_w = px_border[0]->width();
    int first_h = px_border[0]->height();
    QRect firstRect(0,0,first_w,first_h);
    if (e->rect().intersects(firstRect))
        p.drawPixmap(firstRect, *(px_border[0]));

    // middle of border
    QRect midRect;
    int w = width();
    int h = height();
    int x = (e_border=='t'||e_border=='b')? first_w : 0;
    int y = (e_border=='t'||e_border=='b')? 0 : first_h;
    int mid_w = px_border[1]->width();
    int mid_h = px_border[1]->height();
    while (x<w && y<h) {
        midRect = QRect(x,y,mid_w,mid_h);
        if (e->rect().intersects(midRect))
            p.drawPixmap(midRect, *(px_border[1]));
        if (e_border=='t'||e_border=='b') {
            x += mid_w;
        }
        else {
            y += mid_h;
        }
    }

    // right or bottom corner
    int last_w = px_border[2]->width();
    int last_h = px_border[2]->height();
    QRect lastRect;
    if (e_border=='t'||e_border=='b') {
        lastRect = QRect(w-last_w, 0, last_w, last_h);
    }
    else {
        lastRect = QRect(0, h-last_h, last_w, last_h);
    }
    if (e->rect().intersects(lastRect))
        p.drawPixmap(lastRect, *(px_border[2]));

    e->accept();
}

void BorderWidget::mousePressEvent(QMouseEvent *e)
{
    if (!d->q->isFloating()) {
        e->ignore();
        return;
    }
    if (e_border=='t') {
        setCursor(Qt::SizeAllCursor);
        b_moving  = true;
        p_start = e->globalPos();
        e->accept();
    }
    else if (b_resizable) {
        b_moving  = true;
        p_start = e->globalPos();
        e->accept();
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (b_moving) {
        QPoint d = e->globalPos() - p_start;
        int distance = (int) ( sqrt( (double) (d.x()*d.x() + d.y()*d.y()) ) );
        p_start = e->globalPos();
        if ( b_moving || distance >= QApplication::startDragDistance() ) {
            mouseDragAction(d);
        }
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (b_moving) {
        b_moving = false;
        if (e_border=='t')
            unsetCursor();
        e->accept();
    }
    else {
        e->ignore();
    }
}

void BorderWidget::mouseDragAction(const QPoint &offset)
{
    if (b_resizable) {
        Q_CHECK_PTR(w_window);
        if (e_border=='l') {
            QPoint newPos = w_window->pos();
            newPos.setX(newPos.x()+offset.x());
            w_window->move(newPos);
            w_window->setFixedWidth(w_window->width()-offset.x());
            d->w_centralWidget->setFixedWidth(w_window->width()-
                                              d->w_leftBorder->width()-
                                              d->w_rightBorder->width());
        }
        if (e_border=='r') {
            w_window->setFixedWidth(w_window->width()+offset.x());
            d->w_centralWidget->setFixedWidth(w_window->width()-
                                              d->w_leftBorder->width()-
                                              d->w_rightBorder->width());
        }
        if (e_border=='b') {
            w_window->setFixedHeight(w_window->height()+offset.y());
            d->w_centralWidget->setFixedHeight(w_window->height()-
                                               d->w_topBorder->height()-
                                               d->w_bottomBorder->height());
        }
        update();
    }
    else if (e_border=='t') {
        Q_CHECK_PTR(w_window);
        w_window->move(w_window->pos()+offset);
    }
}

SecondaryWindowButton::SecondaryWindowButton(QWidget *parent, bool checkable,
                           const QString &name,
                           const QList<QPixmap*> &pixmaps,
                           const QString &toolTip)
                               : QAbstractButton(parent)
{
    setText(name);
    setCheckable(checkable);
    setToolTip(toolTip);
    px = pixmaps;
    b_pressed = b_hovered = false;
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
        index = 0;
    }
    else {
        index = 0;
    }
    if (isCheckable()) {
        if (!isChecked())
            index += 3;
    }

    if (index<px.size()) {
        QPixmap *pixmap = px[index];
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
    b_hovered = true;
}

void SecondaryWindowButton::leaveEvent(QEvent *e)
{
    e->accept();
    b_hovered = false;
}

void SecondaryWindowButton::mousePressEvent(QMouseEvent *e)
{
    b_pressed = true;
    QAbstractButton::mousePressEvent(e);
}

void SecondaryWindowButton::mouseReleaseEvent(QMouseEvent *e)
{
    b_pressed = false;
    QAbstractButton::mouseReleaseEvent(e);
}

// class CustomLabel

SecondaryWindowTitle::SecondaryWindowTitle(QWidget *parent, const QString &style)
    : QWidget(parent)
{
    setStyle(style);
}

void SecondaryWindowTitle::setStyle(const QString &style)
{
    QStringList props = style.split(";");
    QString name, value;
    QStringList p;
    i_padding = 0;
    fl_textAlignment = Qt::AlignLeft;
    f_font = font();
    col_foreground = QColor("black");
    for (int i=0; i<props.size(); i++) {
        p = props.at(i).split(":");
        if (p.size()==2) {
            name = p[0].toLower().trimmed(); value = p[1].trimmed();
            if (name=="color") {
                col_foreground = QColor(value);
            }
            if (name=="size") {
                QString measure = "pt";
                if (value.endsWith("px")) {
                    measure = "px";
                }
                value.remove("pt");
                value.remove("px");
                int v = value.toInt();
                if (measure=="px")
                    f_font.setPixelSize(v);
                if (measure=="pt")
                    f_font.setPointSize(v);
            }
            if (name=="alignment") {
                value = value.toLower();
                if (value=="center")
                    fl_textAlignment = Qt::AlignHCenter;
                else if (value=="right")
                    fl_textAlignment = Qt::AlignRight;
                else
                    fl_textAlignment = Qt::AlignLeft;
            }
            if (name=="padding") {
                i_padding = value.toInt();
            }
        }
    }
    update();
}

void SecondaryWindowTitle::setText(const QString &v)
{
    s_text = v;
    update();
}

void SecondaryWindowTitle::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setFont(f_font);
    p.setPen(QColor(col_foreground));
    QTextOption opt;
    opt.setAlignment(fl_textAlignment|Qt::AlignVCenter);
    opt.setWrapMode(QTextOption::NoWrap);
    p.drawText(QRect(i_padding,0,width()-2*i_padding,height()),s_text,opt);
    e->accept();

}

}
