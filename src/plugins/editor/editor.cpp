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
    return QList<QAction*>();
}

QList<MenuActionsGroup> Editor::menuActions()
{
    return QList<MenuActionsGroup>();
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
