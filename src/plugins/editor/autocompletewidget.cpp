#include "autocompletewidget.h"

#define EXTRA_ROWS 3

namespace Editor {

AutoCompleteWidget::AutoCompleteWidget(
    QWidget *parent
    ) :
    QWidget(parent)
{
    pnt_mousePress = QPoint(-1000, -1000);
    i_currentIndex = -1;
    i_listViewOffset = 0;
    i_dY = 0;
    m_animation = new QPropertyAnimation(this, "listViewOffset", this);
    m_animation->setDuration(300);
    setCursor(Qt::OpenHandCursor);
    QAction * a = new QAction(this);
    a->setShortcut(QKeySequence("Esc"));
    connect(a, SIGNAL(triggered()), this, SLOT(reject()));
    addAction(a);
}

void AutoCompleteWidget::init(
    const QFont &font,
    QWidget * focusOnCloseWidget,
    const QString &source,
    const QStringList &algorhitms,
    const QStringList &locals,
    const QStringList &globals)
{    
//    setWindowFlags(Qt::Popup);
//    setWindowFlags(Qt::FramelessWindowHint);
    i_currentIndex = 0;
    w_focusOnCloseWidget = focusOnCloseWidget;
    QFontMetrics fm(font);
    int dY = fm.lineSpacing();
    setFixedHeight(dY*(1+EXTRA_ROWS*2));

    int w = 0;
    l_items.clear();

    for (int i=0; i<globals.size(); i++) {
        if (globals[i].startsWith(source) || source.isEmpty()) {
            AutoCompleteItem item;
            item.text = globals[i];
            item.type = AutoCompleteItem::Global;
            w = qMax(w, fm.width(item.text));
            l_items << item;
        }
    }

    for (int i=0; i<locals.size(); i++) {
        if (locals[i].startsWith(source) || source.isEmpty()) {
            AutoCompleteItem item;
            item.text = locals[i];
            item.type = AutoCompleteItem::Local;
            w = qMax(w, fm.width(item.text));
            l_items << item;
        }
    }

    i_dY = dY;

    i_currentIndex = l_items.size();
    i_listViewOffset = EXTRA_ROWS*i_dY - i_currentIndex*i_dY;

    for (int i=0; i<algorhitms.size(); i++) {
        if (algorhitms[i].startsWith(source) || source.isEmpty()) {
            AutoCompleteItem item;
            item.text = algorhitms[i];
            w = qMax(w, fm.width(item.text));
            item.type = AutoCompleteItem::Algorhitm;
            l_items << item;
        }
    }

    w = qMin(w+20, 200);
    setFixedWidth(w);

    setFont(font);

    pnt_mousePress = QPoint(-1000, -1000);
    s_source = source;
}

void AutoCompleteWidget::setCurrentIndex(int index)
{
    if (index<0 || index>=l_items.size())
        return;
//    qDebug() << "setCurrentIndex("<< index << ")";
    i_currentIndex = index;
//    qDebug() << l_items[i_currentIndex].text;
    int offset = EXTRA_ROWS*i_dY - i_currentIndex*i_dY;
    if (m_animation->state()==QPropertyAnimation::Running)
        m_animation->stop();
    if (pnt_mousePress.x()==-1000) {
        m_animation->setStartValue(listViewOffset());
//        qDebug() << "offset = " << offset;
        m_animation->setEndValue(offset);
        m_animation->start();
    }
}

bool AutoCompleteWidget::event(QEvent *e)
{
    if (e->type()==QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent*>(e);
        keyPressEvent(ke);
        return true;
    }
    return QWidget::event(e);
}

void AutoCompleteWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key()==Qt::Key_Up) {
        setCurrentIndex(i_currentIndex-1);
    }
    else if (e->key()==Qt::Key_Down) {
        setCurrentIndex(i_currentIndex+1);
    }
    else if (e->key()==Qt::Key_PageUp) {
        setCurrentIndex(qMax(0, i_currentIndex-5));
    }
    else if (e->key()==Qt::Key_PageDown) {
        setCurrentIndex(qMin(l_items.size()-1, i_currentIndex+5));
    }
    else if (e->key()==Qt::Key_Home) {
        setCurrentIndex(0);
    }
    else if (e->key()==Qt::Key_End) {
        setCurrentIndex(l_items.size()-1);
    }
    else if (e->key()==Qt::Key_Enter || e->key()==Qt::Key_Space || e->key()==Qt::Key_Return) {
        accept();
    }
    else if (e->key()==Qt::Key_Tab || e->key()==Qt::Key_Escape) {
        reject();
    }

    e->accept();
}

int AutoCompleteWidget::offsetY() const
{
    return -1 * (EXTRA_ROWS+1) * i_dY;
}

void AutoCompleteWidget::mousePressEvent(QMouseEvent *e)
{
    setFocus();
    pnt_mousePress = e->pos();
    b_doubleClicked = false;
    setCursor(Qt::ClosedHandCursor);
    if (e->button()==Qt::RightButton)
        reject();
    e->accept();
}

void AutoCompleteWidget::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    pnt_mousePress = QPoint(-1000,-1000);
    int index = -1*(i_listViewOffset-EXTRA_ROWS*i_dY) / i_dY;
    index = qMax(0, qMin(l_items.size()-1, index));
    if (!b_doubleClicked)
        setCurrentIndex(index);
    setCursor(Qt::OpenHandCursor);
}

void AutoCompleteWidget::mouseMoveEvent(QMouseEvent *e)
{
    int delta = e->pos().y()-pnt_mousePress.y();
    i_listViewOffset += delta;
    pnt_mousePress = e->pos();
    update();
    e->accept();
}

void AutoCompleteWidget::wheelEvent(QWheelEvent *e)
{
    int delta = 0;
    if (e->orientation()==Qt::Vertical) {
        delta = e->delta() / 8;
    }
    if (delta!=0) {
        i_listViewOffset += delta;
        update();
        int index = -1*(i_listViewOffset-EXTRA_ROWS*i_dY) / i_dY;
        index = qMax(0, qMin(l_items.size()-1, index));
        setCurrentIndex(index);
    }
    e->accept();
}

void AutoCompleteWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    int row = e->pos().y() / i_dY;
    int index = i_currentIndex - EXTRA_ROWS + row;
    index = qMax(0, qMin(l_items.size()-1, index));
    qDebug() << "new index: " << index;
    b_doubleClicked = true;
    if (i_currentIndex!=index) {
        setCurrentIndex(index);
    }
    else {
        accept();
    }
    e->accept();
}

void AutoCompleteWidget::paintEvent(QPaintEvent *e)
{
    QImage img(width(), height(), QImage::Format_ARGB32);
    img.fill(0);
    QPainter p(&img);
    p.setPen(Qt::NoPen);
    p.setBrush(palette().brush(QPalette::Window));
    p.drawRect(0, 0, width(), height());
    p.setFont(font());
    p.setPen(QPen(palette().brush(QPalette::Highlight), 2, Qt::SolidLine));
    p.setBrush(palette().brush(QPalette::Base));
    p.drawRect(6, i_dY*EXTRA_ROWS-2, width()-12, i_dY+8);

    for (int i=0; i<l_items.size(); i++) {
        int y = (i+1)*i_dY + i_listViewOffset;
        p.setPen(QColor(Qt::black));
        p.drawText(12, y, l_items[i].text);
    }

    for (int i=0; i<i_dY*EXTRA_ROWS; i++) {
        quint8 alpha = 255 * i / (i_dY*EXTRA_ROWS);
        quint32 alphaMask = alpha << 24;
        for (int j=0; j<width(); j++) {
            quint32 rgb = img.pixel(j, i);
            rgb = (rgb << 8) >> 8;
            rgb = rgb | alphaMask;
            img.setPixel(j, i, rgb);
        }
    }

    for (int i=0; i<i_dY*EXTRA_ROWS; i++) {
        quint8 alpha = 255 - ( 255 * i / (i_dY*EXTRA_ROWS));
        quint32 alphaMask = alpha << 24;
        for (int j=0; j<width(); j++) {
            quint32 rgb = img.pixel(j, i+height()-EXTRA_ROWS*i_dY);
            rgb = (rgb << 8) >> 8;
            rgb = rgb | alphaMask;
            img.setPixel(j, i+height()-EXTRA_ROWS*i_dY, rgb);
        }
    }

    QPainter pp(this);
    pp.drawImage(0,0,img);

    e->accept();
}


void AutoCompleteWidget::accept()
{
    emit accepted(s_source, l_items[i_currentIndex].text);
    hide();
    w_focusOnCloseWidget->setFocus();
}

void AutoCompleteWidget::reject()
{
    hide();
    w_focusOnCloseWidget->setFocus();
}

} // namespace Editor
