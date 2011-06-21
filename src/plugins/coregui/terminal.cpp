#include "terminal.h"
#include "terminal_plane.h"
#include "terminal_onesession.h"

namespace Terminal {

class ScrollBar
        : public QScrollBar
{
public:
    inline explicit ScrollBar(Qt::Orientation orientation, QWidget * parent) : QScrollBar(orientation, parent) {}
protected:
    inline void paintEvent(QPaintEvent *e)
    {
        if (isEnabled()) {
            QScrollBar::paintEvent(e);
        }
        else {
            QPainter p(this);
            p.setPen(Qt::NoPen);
            p.setBrush(palette().brush(QPalette::Normal, QPalette::Window));
            p.drawRect(e->rect());
            p.end();
            e->accept();
        }
    }

};

Terminal::Terminal(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    setMinimumWidth(450);
    QGridLayout * l = new QGridLayout();
    setLayout(l);
    m_plane = new Plane(this);
    l->addWidget(m_plane, 0, 1, 1, 1);
    sb_vertical = new ScrollBar(Qt::Vertical, this);
    l->addWidget(sb_vertical, 0, 2, 1, 1);
    sb_horizontal = new ScrollBar(Qt::Horizontal, this);
    l->addWidget(sb_horizontal, 1, 1, 1, 1);
    QToolBar * tb = new QToolBar(this);
    tb->setOrientation(Qt::Vertical);
    l->addWidget(tb, 0, 0, 2, 1);

    a_saveLast = new QAction(tr("Save last output"), this);
    a_saveLast->setIcon(QIcon::fromTheme("document-save"));
    connect(a_saveLast, SIGNAL(triggered()), this, SLOT(saveLast()));
    tb->addAction(a_saveLast);

    a_editLast = new QAction(tr("Open last output in editor"), this);
    a_editLast->setIcon(QIcon::fromTheme("document-edit"));
    connect(a_editLast, SIGNAL(triggered()), this, SLOT(editLast()));
    tb->addAction(a_editLast);

    tb->addSeparator();

    a_saveAll = new QAction(tr("Save all output"), this);
    a_saveAll->setIcon(QIcon::fromTheme("document-save-all"));
    connect(a_saveAll, SIGNAL(triggered()), this, SLOT(saveAll()));
    tb->addAction(a_saveAll);

    tb->addSeparator();

    a_clear = new QAction(tr("Clear output"), this);
    a_clear->setIcon(QIcon::fromTheme("edit-delete"));
    connect(a_clear, SIGNAL(triggered()), this, SLOT(clear()));

    tb->addAction(a_clear);
    m_plane->updateScrollBars();

    connect(sb_vertical,SIGNAL(valueChanged(int)),m_plane, SLOT(update()));
    connect(sb_horizontal,SIGNAL(valueChanged(int)),m_plane, SLOT(update()));

//    start("debug");
//    output("this is output");
//    output("this is another output");
//    error("this is error");
}

void Terminal::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    m_plane->setFocus();
}

void Terminal::clear()
{
    for (int i=0; i<l_sessions.size(); i++) {
        l_sessions[i]->deleteLater();
    }
    l_sessions.clear();
    m_plane->update();
}

void Terminal::start(const QString & fileName)
{
    int fixedWidth = -1;
    OneSession * session = new OneSession(fixedWidth, QFileInfo(fileName).fileName(), this);
    connect(session, SIGNAL(updateRequest()), m_plane, SLOT(update()));
    l_sessions << session;
    m_plane->update();
}

void Terminal::finish()
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", this);

    l_sessions.last()->finish();
    m_plane->updateScrollBars();
}

void Terminal::terminate()
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", this);
    l_sessions.last()->terminate();
    m_plane->updateScrollBars();
}

void Terminal::output(const QString & text)
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", this);
    l_sessions.last()->output(text);
    m_plane->updateScrollBars();
}

void Terminal::input(const QString & format)
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", this);
    l_sessions.last()->input(format);
    m_plane->updateScrollBars();
}

void Terminal::error(const QString & message)
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", this);
    l_sessions.last()->error(message);
    m_plane->updateScrollBars();
}

void Terminal::saveAll()
{
    // TODO implement me
}

void Terminal::saveLast()
{
    // TODO implement me
}

void Terminal::editLast()
{
    Q_ASSERT(!l_sessions.isEmpty());
    emit openTextEditor(l_sessions.last()->plainText());
}

} // namespace Terminal
