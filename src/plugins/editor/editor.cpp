#include "editor.h"
#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "utils.h"
#include "statusbar.h"

namespace Editor {

using namespace Shared;

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
            QPainter p (this);
            p.setPen(Qt::NoPen);
            p.setBrush(palette().brush(QPalette::Window));
            p.drawRect(e->rect());
            e->accept();
        }
    }

};


class EditorPrivate
        : public QObject
{
    Q_OBJECT;
public:
    AnalizerInterface * analizer;
    TextDocument * doc;
    TextCursor * cursor;
    EditorPlane * plane;
    StatusBar * statusBar;
    static Clipboard * clipboard;
    QScrollBar * horizontalScrollBar;
    QScrollBar * verticalScrollBar;
    QSettings * settings;
    int documentId;

    QAction * copy;
    QAction * paste;
    QAction * cut;
    QAction * selectAll;
    QAction * deleteLine;
    QAction * deleteTail;

    void createActions();
    void updateFromAnalizer();
public slots:
    void handleLineAndTextChanged(const QList<int> &, const QList<int> &);
};


void EditorPrivate::handleLineAndTextChanged(const QList<int> &r, const QList<int> &n)
{
    if (!analizer) {
        return;
    }
    QStringList newLines;
    foreach (int l, n) {
        newLines << doc->at(l).text;
    }
//    qDebug() << "Removed " << r.count() << " lines, inserted " << n.count() << " lines";
    analizer->changeSourceText(documentId, r, newLines);
    updateFromAnalizer();
}

void EditorPrivate::updateFromAnalizer()
{
    QList<Shared::LineProp> props = analizer->lineProperties(documentId);
    QList<QPoint> ranks = analizer->lineRanks(documentId);
    QList<Error> errors = analizer->errors(documentId);
    for (int i=0; i<doc->size(); i++) {
        if (i<ranks.size()) {
            (*doc)[i].indentStart = ranks[i].x();
            (*doc)[i].indentEnd = ranks[i].y();
        }
        if (i<props.size()) {
            (*doc)[i].highlight = props[i].toList();
        }
        (*doc)[i].errors.clear();
    }
    for (int i=0; i<errors.size(); i++) {
        Error err = errors[i];
        int lineNo = err.line;
        if (lineNo>=0) {
            (*doc)[lineNo].errors << err.code;
        }
    }
    plane->update();
}

Clipboard * EditorPrivate::clipboard = 0;

Editor::Editor(QSettings * settings, AnalizerInterface * analizer, int documentId, QWidget *parent) :
    VisualComponent()
{
    setParent(parent);
    d = new EditorPrivate;
    d->doc = new TextDocument;
    d->cursor = new TextCursor(d->doc);
    d->analizer = analizer;
    d->documentId = documentId;
    d->settings = settings;
    if (!d->clipboard)
        d->clipboard = new Clipboard;
    d->horizontalScrollBar = new ScrollBar(Qt::Horizontal, this);
    d->verticalScrollBar = new ScrollBar(Qt::Vertical, this);
    d->plane = new EditorPlane(d->doc, d->cursor, d->clipboard, d->settings, d->horizontalScrollBar, d->verticalScrollBar, this);
    d->statusBar = new StatusBar;
    connect(d->cursor, SIGNAL(positionChanged(int,int)),
            d->statusBar, SLOT(handleCursorPositionChanged(int,int)));
    d->statusBar->handleCursorPositionChanged(d->cursor->row(), d->cursor->column());
    connect(d->clipboard, SIGNAL(bufferEntriesCountChanged(int)),
            d->statusBar, SLOT(handleClipboardChanged(int)));
    d->statusBar->handleClipboardChanged(d->clipboard->entriesCount());
    d->createActions();

    connect(d->cursor, SIGNAL(lineAndTextChanged(QList<int>,QList<int>)),
            d, SLOT(handleLineAndTextChanged(QList<int>,QList<int>)));

    QGridLayout * l = new QGridLayout();
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(d->plane, 0, 0);
    l->addWidget(d->verticalScrollBar, 0, 1);
    l->addWidget(d->horizontalScrollBar, 1, 0);
    l->addWidget(d->statusBar, 2, 0, 1, 2);
}



QString Editor::text() const
{
    QString result;
    for (int i=0; i<d->doc->size(); i++) {
        result += d->doc->at(i).text;
        if (i<d->doc->size()-1)
            result += "\n";
    }
    return result;
}

void EditorPrivate::createActions()
{
    selectAll = new QAction(plane);
    selectAll->setText(QObject::tr("Select all text in editor"));
    selectAll->setIcon(QIcon::fromTheme("edit-select-all"));
    selectAll->setShortcut(QKeySequence(QKeySequence::SelectAll));
    QObject::connect(selectAll, SIGNAL(triggered()), plane, SLOT(selectAll()));

    copy = new QAction(plane);
    copy->setText(QObject::tr("Copy selection to clipboard"));
    copy->setIcon(QIcon::fromTheme("edit-copy"));
    copy->setShortcut(QKeySequence(QKeySequence::Copy));
    QObject::connect(copy, SIGNAL(triggered()), plane, SLOT(copy()));

    cut = new QAction(plane);
    cut->setText(QObject::tr("Cut selection to clipboard"));
    cut->setIcon(QIcon::fromTheme("edit-cut"));
    cut->setShortcut(QKeySequence(QKeySequence::Cut));
    QObject::connect(cut, SIGNAL(triggered()), plane, SLOT(cut()));

    paste = new QAction(plane);
    paste->setText(QObject::tr("Paste from clipboard"));
    paste->setIcon(QIcon::fromTheme("edit-paste"));
    paste->setShortcut(QKeySequence(QKeySequence::Paste));
    QObject::connect(paste, SIGNAL(triggered()), plane, SLOT(paste()));

    deleteLine = new QAction(plane);
    deleteLine->setText(QObject::tr("Delete line under cursor"));
    deleteLine->setIcon(QIcon::fromTheme("edit-delete"));
    deleteLine->setShortcut(QKeySequence("Ctrl+Y"));
    QObject::connect(deleteLine, SIGNAL(triggered()), plane, SLOT(removeLine()));

    deleteTail = new QAction(plane);
    deleteTail->setText(QObject::tr("Delete text from cursor to end of line"));
    deleteTail->setIcon(QIcon::fromTheme("edit-clear"));
    deleteTail->setShortcut(QKeySequence("Ctrl+K"));
    QObject::connect(deleteTail, SIGNAL(triggered()), plane, SLOT(removeLineTail()));
}

Editor::~Editor()
{
    delete d->doc;
    d->plane->deleteLater();
    d->statusBar->deleteLater();
    delete d;
}

QList<QAction*> Editor::toolbarActions()
{
    QList<QAction*> result;
    result << d->cut;
    result << d->copy;
    result << d->paste;
    return result;
}

QList<MenuActionsGroup> Editor::menuActions()
{
    QList<MenuActionsGroup> result;
    MenuActionsGroup edit;
    QAction * separator = new QAction(this);
    separator->setSeparator(true);
    edit.menuText = tr("Edit");
    edit.actions << d->selectAll;
    edit.actions << separator;
    edit.actions << d->cut;
    edit.actions << d->copy;
    edit.actions << d->paste;
    edit.actions << separator;
    edit.actions << d->deleteLine;
    edit.actions << d->deleteTail;
    result << edit;
    return result;
}

void Editor::setText(const QString & text)
{
    d->doc->setPlainText(text);
    if (d->analizer) {
        d->analizer->setSourceText(d->documentId, text);
        d->updateFromAnalizer();
    }
    d->plane->update();
}

void Editor::setNotModified()
{
    d->cursor->clearUndoRedoStacks();
}

bool Editor::isModified() const
{
    return d->cursor->isModified();
}

} // namespace Editor

#include "editor.moc"
