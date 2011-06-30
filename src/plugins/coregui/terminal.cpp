#include "terminal.h"
#include "terminal_plane.h"
#include "terminal_onesession.h"

namespace Terminal {


Terminal::Terminal(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    setMinimumWidth(450);
    QGridLayout * l = new QGridLayout();
    setLayout(l);
    m_plane = new Plane(this);
    l->addWidget(m_plane, 0, 1, 1, 1);
    sb_vertical = new QScrollBar(Qt::Vertical, this);
    l->addWidget(sb_vertical, 0, 2, 1, 1);
    sb_horizontal = new QScrollBar(Qt::Horizontal, this);
    l->addWidget(sb_horizontal, 1, 1, 1, 1);
    QToolBar * tb = new QToolBar(this);
    tb->setOrientation(Qt::Vertical);
    l->addWidget(tb, 0, 0, 2, 1);

    a_saveLast = new QAction(tr("Save last output"), this);
    a_saveLast->setIcon(QIcon::fromTheme("document-save", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/document-save.png")));
    connect(a_saveLast, SIGNAL(triggered()), this, SLOT(saveLast()));
    tb->addAction(a_saveLast);

    a_editLast = new QAction(tr("Open last output in editor"), this);
    a_editLast->setIcon(QIcon::fromTheme("document-edit", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/document-edit.png")));
    connect(a_editLast, SIGNAL(triggered()), this, SLOT(editLast()));
    tb->addAction(a_editLast);

    tb->addSeparator();

    a_saveAll = new QAction(tr("Save all output"), this);
    a_saveAll->setIcon(QIcon::fromTheme("document-save-all", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/document-save-all.png")));
    connect(a_saveAll, SIGNAL(triggered()), this, SLOT(saveAll()));
    tb->addAction(a_saveAll);

    tb->addSeparator();

    a_clear = new QAction(tr("Clear output"), this);
    a_clear->setIcon(QIcon::fromTheme("edit-delete", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-delete.png")));
    connect(a_clear, SIGNAL(triggered()), this, SLOT(clear()));

    tb->addAction(a_clear);
    m_plane->updateScrollBars();

    connect(sb_vertical,SIGNAL(valueChanged(int)),m_plane, SLOT(update()));
    connect(sb_horizontal,SIGNAL(valueChanged(int)),m_plane, SLOT(update()));


    connect(m_plane, SIGNAL(inputTextChanged(QString)),
            this, SLOT(handleInputTextChanged(QString)));

    connect(m_plane, SIGNAL(inputCursorPositionChanged(quint16)),
            this, SLOT(handleInputCursorPositionChanged(quint16)));

    connect(m_plane, SIGNAL(inputFinishRequest()),
            this, SLOT(handleInputFinishRequested()));
//    start("debug");
//    output("this is output");
//    output("this is another output");
//    error("this is error");
}

void Terminal::handleInputTextChanged(const QString &text)
{
    if (l_sessions.isEmpty())
        return;
    OneSession * last = l_sessions.last();
    last->changeInputText(text);
}

void Terminal::handleInputCursorPositionChanged(quint16 pos)
{
    if (l_sessions.isEmpty())
        return;
    OneSession * last = l_sessions.last();
    last->changeCursorPosition(pos);
}

void Terminal::handleInputFinishRequested()
{
    if (l_sessions.isEmpty())
        return;
    OneSession * last = l_sessions.last();
    last->tryFinishInput();
}

void Terminal::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    m_plane->setFocus();
}

void Terminal::focusOutEvent(QFocusEvent *e)
{
    QWidget::focusOutEvent(e);
    m_plane->clearFocus();
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
    OneSession * session = new OneSession(fixedWidth, QFileInfo(fileName).fileName(), m_plane);
    connect(session, SIGNAL(updateRequest()), m_plane, SLOT(update()));
    l_sessions << session;
    connect (l_sessions.last(), SIGNAL(inputDone(QVariantList)),
             this, SIGNAL(inputFinished(QVariantList)));
    connect (l_sessions.last(), SIGNAL(message(QString)),
             this, SIGNAL(message(QString)));
    connect (l_sessions.last(), SIGNAL(inputDone(QVariantList)),
             this, SLOT(handleInputDone()));
    m_plane->updateScrollBars();
    if (sb_vertical->isEnabled())
        sb_vertical->setValue(sb_vertical->maximum());
    m_plane->update();
}

void Terminal::finish()
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", m_plane);

    l_sessions.last()->finish();
    m_plane->updateScrollBars();
    if (sb_vertical->isEnabled())
        sb_vertical->setValue(sb_vertical->maximum());
}

void Terminal::terminate()
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", m_plane);
    l_sessions.last()->terminate();
    m_plane->updateScrollBars();
    if (sb_vertical->isEnabled())
        sb_vertical->setValue(sb_vertical->maximum());
    m_plane->setInputMode(false);
}

void Terminal::output(const QString & text)
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", m_plane);
    l_sessions.last()->output(text);
    qDebug() << "output " << text;
    m_plane->updateScrollBars();
    if (sb_vertical->isEnabled())
        sb_vertical->setValue(sb_vertical->maximum());
}

void Terminal::input(const QString & format)
{
    if (l_sessions.isEmpty()) {
        l_sessions << new OneSession(-1,"unknown", m_plane);
        connect (l_sessions.last(), SIGNAL(inputDone(QVariantList)),
                 this, SIGNAL(inputFinished(QVariantList)));
        connect (l_sessions.last(), SIGNAL(message(QString)),
                 this, SIGNAL(message(QString)));
        connect (l_sessions.last(), SIGNAL(inputDone(QVariantList)),
                 this, SLOT(handleInputDone()));
    }
    OneSession * lastSession = l_sessions.last();



    lastSession->input(format);
    m_plane->updateScrollBars();
    if (sb_vertical->isEnabled())
        sb_vertical->setValue(sb_vertical->maximum());
    m_plane->setInputMode(true);

    m_plane->setFocus();
}

void Terminal::handleInputDone()
{
    m_plane->setInputMode(false);
}

void Terminal::error(const QString & message)
{
    if (l_sessions.isEmpty())
        l_sessions << new OneSession(-1,"unknown", m_plane);
    l_sessions.last()->error(message);
    m_plane->updateScrollBars();
    if (sb_vertical->isEnabled())
        sb_vertical->setValue(sb_vertical->maximum());
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
