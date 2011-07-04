#include "editor.h"
#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "utils.h"
#include "macro.h"
#include "settingspage.h"

namespace Editor {

using namespace Shared;

class EditorPrivate
        : public QObject
{
    Q_OBJECT;
public:
    AnalizerInterface * analizer;
    TextDocument * doc;
    TextCursor * cursor;
    EditorPlane * plane;

    static Clipboard * clipboard;
    QScrollBar * horizontalScrollBar;
    QScrollBar * verticalScrollBar;
    QSettings * settings;

    QLabel * keybStatus;
    QLabel * positionStatus;

    QAction * copy;
    QAction * paste;
    QAction * cut;
    QAction * selectAll;
    QAction * deleteLine;
    QAction * deleteTail;

    QMenu * menu_edit;
    QMenu * menu_insert;

    QList<Macro> systemMacros;
    QList<Macro> userMacros;

    int timerId;

    void loadMacros();
    void updateInsertMenu();
    void createActions();
    void updateFromAnalizer();
    static QList<Shared::ChangeTextTransaction> mergeTransactions(QList<Shared::ChangeTextTransaction>);
    static bool mergeTransaction(Shared::ChangeTextTransaction & one, const Shared::ChangeTextTransaction & other);
    void timerEvent(QTimerEvent *e);
public slots:
    void updatePosition(int row, int col);
    void handleLineAndTextChanged(const QStack<Shared::ChangeTextTransaction> & changes);
    void playMacro();
};

void Editor::setDocumentId(int id)
{
    d->doc->documentId = id;
}

void Editor::lock()
{
    d->cursor->setEnabled(false);
}

void Editor::unlock()
{
    d->cursor->setEnabled(true);
}

void Editor::setMarginText(int lineNo, const QString &text)
{
    if (lineNo>=0 && lineNo<d->doc->size())
        (*d->doc)[lineNo].marginText = text;
    update();
}

void Editor::clearMarginText()
{
    for (int i=0; i<d->doc->size(); i++) {
        (*d->doc)[i].marginText = "";
    }
    update();
}

void Editor::setLineHighlighted(int lineNo, const QColor &color)
{
    d->plane->setLineHighlighted(lineNo, color);
}

QList<QWidget*> Editor::statusbarWidgets()
{
    return QList<QWidget*>() << d->positionStatus << d->keybStatus;
}

void EditorPrivate::timerEvent(QTimerEvent *e)
{
    e->accept();
    if (keybStatus) {
        bool capsLock = Utils::isCapsLock();
        bool russian = Utils::isRussianLayout();
        if (Utils::temporaryLayoutSwitch)
            russian = !russian;
        QString abc = russian? QString::fromUtf8("рус") : "lat";
        if (capsLock)
            abc = abc.toUpper();
        if (Utils::temporaryLayoutSwitch)
            abc += "*";
        keybStatus->setText(tr("Keys: %1").arg(abc));
    }
}

void EditorPrivate::updatePosition(int row, int col)
{
    positionStatus->setText(tr("Row: %1, Col: %2").arg(row).arg(col));
}

void EditorPrivate::loadMacros()
{
    const QString sharePath = QCoreApplication::instance()->property("sharePath").toString();
    const QString systemMacrosPath = sharePath+"/editor/macros.json";
    systemMacros = loadFromFile(systemMacrosPath);
}

void EditorPrivate::updateInsertMenu()
{
    menu_insert->clear();
    for (int i=0; i<systemMacros.size(); i++) {
        Macro m = systemMacros[i];
        QKeySequence ks(
                    settings->value(SettingsPage::KeyPlayMacroShortcut, SettingsPage::DefaultPlayMacroShortcut).toString()
                    +", "+QString(m.key)
                    );
        m.action = new QAction(m.title, menu_insert);
        m.action->setShortcut(ks);
        systemMacros[i].action = m.action;
        menu_insert->addAction(m.action);
        connect(m.action, SIGNAL(triggered()), this, SLOT(playMacro()));
    }
    if (!userMacros.isEmpty())
        menu_insert->addSeparator();
    for (int i=0; i<userMacros.size(); i++) {
        Macro m = userMacros[i];
        QKeySequence ks(
                    settings->value(SettingsPage::KeyPlayMacroShortcut, SettingsPage::DefaultPlayMacroShortcut).toString()
                    +", "+QString(m.key)
                    );
        m.action = new QAction(m.title, menu_insert);
        m.action->setShortcut(ks);
        userMacros[i].action = m.action;
        menu_insert->addAction(m.action);
        connect(m.action, SIGNAL(triggered()), this, SLOT(playMacro()));
    }
}

void EditorPrivate::playMacro()
{
    QAction * a = qobject_cast<QAction*>(sender());
    Q_CHECK_PTR(a);
    Macro m;
    bool found = false;
    foreach (Macro mm, systemMacros) {
        if (mm.action==a) {
            found = true;
            m = mm;
            break;
        }
    }
    if (!found) {
        foreach (Macro mm, userMacros) {
            if (mm.action==a) {
                found = true;
                m = mm;
                break;
            }
        }
    }
    if (found) {
        for (int i=0; i<m.commands.size(); i++) {
            cursor->evaluateCommand(m.commands[i]);
        }
    }
}


void Editor::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    d->plane->setFocus();
}

enum TransactionType { TT_Insert, TT_Remove, TT_Replace };
TransactionType trType(const Shared::ChangeTextTransaction &tr)
{
    if (tr.removedLineNumbers.isEmpty())
        return TT_Insert;
    else if (tr.newLines.isEmpty())
        return TT_Remove;
    else
        return TT_Replace;
}

int trStart(const Shared::ChangeTextTransaction &tr)
{
    if (tr.removedLineNumbers.isEmpty())
        return 0;
    else
        return tr.removedLineNumbers.toList().first();
}

int trLen(const Shared::ChangeTextTransaction &tr)
{
    if (tr.removedLineNumbers.isEmpty())
        return 0;
    else
        return tr.removedLineNumbers.toList().last()-tr.removedLineNumbers.toList().first()+1;
}

bool EditorPrivate::mergeTransaction(Shared::ChangeTextTransaction &one, const Shared::ChangeTextTransaction &other)
{
    TransactionType t = trType(other);
    if (trType(one)==TT_Insert) {
        if (t==TT_Insert) {
            one.newLines += other.newLines;
            return true;
        }
        else if (t==TT_Remove) {
            int start = trStart(other);
            int len = trLen(other);
            if (start>=one.newLines.size() || start+len>=one.newLines.size())
                return false;
            one.newLines = one.newLines.mid(0, start) + one.newLines.mid(start+len);
            return true;
        }
        else if (t==TT_Replace) {
            int start = trStart(other);
            int len = trLen(other);
            if (start>=one.newLines.size() || start+len>=one.newLines.size())
                return false;
            one.newLines = one.newLines.mid(0, start)+other.newLines+one.newLines.mid(start+len);
            return true;
        }
    }
    else if (trType(one)==TT_Remove) {
        if (t==TT_Insert) {
            one.removedLineNumbers = other.removedLineNumbers;
            return true;
        }
        else if (t==TT_Remove) {
            return false; // TODO maybe implement me?
        }
        else if (t==TT_Replace) {
            return false; // TODO maybe implement me?
        }
    }
    else if (trType(one)==TT_Replace) {
        if (t==TT_Insert) {
            return false; // TODO maybe implement me?
        }
        else if (t==TT_Remove) {
            return false; // TODO maybe implement me?
        }
        else if (t==TT_Replace) {
            if (one.removedLineNumbers==other.removedLineNumbers && one.removedLineNumbers.size()==one.newLines.size()) {
                one = other;
                return true;
            }
            return false; // TODO maybe implement me?
        }
    }
    return false;
}

QList<Shared::ChangeTextTransaction> EditorPrivate::mergeTransactions(QList<Shared::ChangeTextTransaction> s)
{
    if (s.size()==1) {
        return s;
    }
    QList<Shared::ChangeTextTransaction> result;
    Shared::ChangeTextTransaction curTrans = s.first();
    s.pop_front();
    while (!s.isEmpty()) {
        Shared::ChangeTextTransaction otherTrans = s.first();
        s.pop_front();
        if (!mergeTransaction(curTrans, otherTrans)) {
            result << curTrans;
            curTrans = otherTrans;
        }
    }
    result << curTrans;
    return result;
}

void EditorPrivate::handleLineAndTextChanged(const QStack<Shared::ChangeTextTransaction> & changes)
{
    if (!analizer) {
        return;
    }
    analizer->changeSourceText(doc->documentId, mergeTransactions(changes.toList()));
    updateFromAnalizer();
}

void EditorPrivate::updateFromAnalizer()
{
    QList<Shared::LineProp> props = analizer->lineProperties(doc->documentId);
    QList<QPoint> ranks = analizer->lineRanks(doc->documentId);
    QList<Error> errors = analizer->errors(doc->documentId);
    for (int i=0; i<doc->size(); i++) {
        TextLine tl = doc->at(i);
        if (i<ranks.size()) {
            tl.indentStart = ranks[i].x();
            tl.indentEnd = ranks[i].y();
        }
        if (i<props.size()) {
            tl.highlight = props[i].toList();
        }
        tl.errors.clear();
        (*doc)[i] = tl;
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
    QWidget()
{
    setParent(parent);
    d = new EditorPrivate;
    if (!d->clipboard)
        d->clipboard = new Clipboard;
    d->doc = new TextDocument;
    d->cursor = new TextCursor(d->doc, d->clipboard, analizer);
    d->analizer = analizer;
    d->doc->documentId = documentId;
    d->settings = settings;
    d->horizontalScrollBar = new QScrollBar(Qt::Horizontal, this);
    d->verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    QList<QRegExp> fileNamesToOpen = QList<QRegExp>() << QRegExp("*", Qt::CaseSensitive, QRegExp::Wildcard);
    if (d->analizer)
        fileNamesToOpen = d->analizer->supportedFileNamePattern();
    d->plane = new EditorPlane(d->doc, d->cursor, d->clipboard, fileNamesToOpen, d->settings, d->horizontalScrollBar, d->verticalScrollBar, d->analizer!=NULL, this);

    d->keybStatus = new QLabel(0);
    d->keybStatus->setFixedWidth(90);
    d->positionStatus = new QLabel(0);
    d->positionStatus->setFixedWidth(120);
    d->timerId = d->startTimer(50);

    connect(d->cursor, SIGNAL(positionChanged(int,int)),
            d, SLOT(updatePosition(int,int)));
    d->updatePosition(d->cursor->row(), d->cursor->column());

    d->createActions();

    connect(d->cursor, SIGNAL(lineAndTextChanged(QStack<Shared::ChangeTextTransaction>)),
            d, SLOT(handleLineAndTextChanged(QStack<Shared::ChangeTextTransaction>)));

    QGridLayout * l = new QGridLayout();
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(d->plane, 0, 0);
    l->addWidget(d->verticalScrollBar, 0, 1);
    l->addWidget(d->horizontalScrollBar, 1, 0);
    connect(d->plane, SIGNAL(urlsDragAndDropped(QList<QUrl>)), this, SIGNAL(urlsDragAndDropped(QList<QUrl>)));
    d->loadMacros();
    d->updateInsertMenu();
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
    selectAll->setIcon(QIcon::fromTheme("edit-select-all", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-select-all.png")));
    selectAll->setShortcut(QKeySequence(QKeySequence::SelectAll));
    QObject::connect(selectAll, SIGNAL(triggered()), plane, SLOT(selectAll()));

    copy = new QAction(plane);
    copy->setText(QObject::tr("Copy selection to clipboard"));
    copy->setIcon(QIcon::fromTheme("edit-copy", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-copy.png")));
    copy->setShortcut(QKeySequence(QKeySequence::Copy));
    QObject::connect(copy, SIGNAL(triggered()), plane, SLOT(copy()));

    cut = new QAction(plane);
    cut->setText(QObject::tr("Cut selection to clipboard"));
    cut->setIcon(QIcon::fromTheme("edit-cut", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-cut.png")));
    cut->setShortcut(QKeySequence(QKeySequence::Cut));
    QObject::connect(cut, SIGNAL(triggered()), plane, SLOT(cut()));

    paste = new QAction(plane);
    paste->setText(QObject::tr("Paste from clipboard"));
    paste->setIcon(QIcon::fromTheme("edit-paste", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-paste.png")));
    paste->setShortcut(QKeySequence(QKeySequence::Paste));
    QObject::connect(paste, SIGNAL(triggered()), plane, SLOT(paste()));

    deleteLine = new QAction(plane);
    deleteLine->setText(QObject::tr("Delete line under cursor"));
    deleteLine->setIcon(QIcon::fromTheme("edit-delete", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-delete.png")));
    deleteLine->setShortcut(QKeySequence("Ctrl+Y"));
    QObject::connect(deleteLine, SIGNAL(triggered()), plane, SLOT(removeLine()));

    deleteTail = new QAction(plane);
    deleteTail->setText(QObject::tr("Delete text from cursor to end of line"));
    deleteTail->setIcon(QIcon::fromTheme("edit-clear", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-clear.png")));
    deleteTail->setShortcut(QKeySequence("Ctrl+K"));
    QObject::connect(deleteTail, SIGNAL(triggered()), plane, SLOT(removeLineTail()));

    menu_edit = new QMenu(tr("Edit"), 0);
    menu_edit->addAction(selectAll);
    menu_edit->addSeparator();
    menu_edit->addAction(cut);
    menu_edit->addAction(copy);
    menu_edit->addAction(paste);
    menu_edit->addSeparator();
    menu_edit->addAction(deleteLine);
    menu_edit->addAction(deleteTail);

    menu_insert = new QMenu(tr("Insert"), 0);
}

Editor::~Editor()
{
    delete d->doc;
    d->plane->deleteLater();
    d->killTimer(d->timerId);
    d->keybStatus->deleteLater();
    d->positionStatus->deleteLater();
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

QList<QMenu*> Editor::menuActions()
{
    QList<QMenu*> result;
    result << d->menu_edit << d->menu_insert;
    return result;
}

void Editor::setText(const QString & text)
{
    d->doc->setPlainText(text);
    if (d->analizer) {
        d->analizer->setSourceText(d->doc->documentId, text);
        d->updateFromAnalizer();
    }
    d->plane->update();
}

void Editor::ensureAnalized()
{
    d->cursor->flushTransaction();
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
