#include "editor.h"
#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "utils.h"
#include "statusbar.h"

namespace Editor {

using namespace Shared;

class EditorPrivate
{
public:
    AnalizerInterface * analizer;
    TextDocument * doc;
    TextCursor * cursor;
    QScrollArea * scrollArea;
    EditorPlane * plane;
    StatusBar * statusBar;
    QScrollBar * horizontalScrollBar;
    static Clipboard * clipboard;
    int documentId;

    QAction * copy;
    QAction * paste;
    QAction * cut;
    QAction * selectAll;
    QAction * deleteLine;
    QAction * deleteTail;

    void createActions();
};


Clipboard * EditorPrivate::clipboard = 0;

Editor::Editor(AnalizerInterface * analizer, int documentId, QWidget *parent) :
    VisualComponent()
{
    setParent(parent);
    d = new EditorPrivate;
    d->doc = new TextDocument;
    d->cursor = new TextCursor(d->doc);
    d->analizer = analizer;
    d->documentId = documentId;
    if (!d->clipboard)
        d->clipboard = new Clipboard;
    QVBoxLayout * l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    setLayout(l);
    d->plane = new EditorPlane(d->doc, d->cursor, d->clipboard, this);
    d->scrollArea = new QScrollArea(this);
    d->scrollArea->setWidget(d->plane);
    d->scrollArea->setWidgetResizable(true);
    d->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    l->addWidget(d->scrollArea);
    d->horizontalScrollBar = new QScrollBar(Qt::Horizontal);
    l->addWidget(d->horizontalScrollBar);
    d->statusBar = new StatusBar;
    l->addWidget(d->statusBar);
    connect(d->cursor, SIGNAL(positionChanged(int,int)),
            d->statusBar, SLOT(handleCursorPositionChanged(int,int)));
    d->statusBar->handleCursorPositionChanged(d->cursor->row(), d->cursor->column());
    connect(d->clipboard, SIGNAL(bufferEntriesCountChanged(int)),
            d->statusBar, SLOT(handleClipboardChanged(int)));
    d->statusBar->handleClipboardChanged(d->clipboard->entriesCount());
    d->createActions();
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
    d->scrollArea->deleteLater();
    d->horizontalScrollBar->deleteLater();
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
    return result;
}

void Editor::setText(const QString & text)
{
    d->doc->setPlainText(text);
    if (d->analizer) {
        d->analizer->setSourceText(d->documentId, text);
        QList<Shared::LineProp> props = d->analizer->lineProperties(d->documentId);
        QList<QPoint> ranks = d->analizer->lineRanks(d->documentId);
        for (int i=0; i<d->doc->size(); i++) {
            (*d->doc)[i].indentStart = ranks[i].x();
            (*d->doc)[i].indentEnd = ranks[i].y();
            (*d->doc)[i].highlight = props[i].toList();
        }
    }
    update();
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
