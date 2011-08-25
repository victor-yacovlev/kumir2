#include "editor.h"
#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "utils.h"
#include "macro.h"
#include "settingspage.h"
#include "editcommands.h"

namespace Editor {

using namespace Shared;

class EditorPrivate
        : public QObject
{
    Q_OBJECT;
public:
    class Editor * q;
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

    QAction * undo;
    QAction * redo;

    QMenu * menu_edit;
    QMenu * menu_insert;

    QAction * separator;

    QList<Macro> systemMacros;
    QList<Macro> userMacros;

    bool teacherMode;

    int timerId;

    bool notSaved;

    void loadMacros();
    void updateInsertMenu();
    void createActions();
    void updateFromAnalizer();
    static QList<Shared::ChangeTextTransaction> mergeTransactions(QList<Shared::ChangeTextTransaction>);
    static bool mergeTransaction(Shared::ChangeTextTransaction & one, const Shared::ChangeTextTransaction & other);
    void timerEvent(QTimerEvent *e);
public slots:
    void updatePosition(int row, int col);
    void handleCompleteCompilationRequiest(
        const QStringList & visibleText,
        const QStringList & hiddenText,
        int hiddenBaseLine
        );
    void handleLineAndTextChanged(const QStack<Shared::ChangeTextTransaction> & changes);
    void playMacro();
};


void Editor::setTeacherMode(bool v)
{
    d->teacherMode = v;
    d->plane->setTeacherMode(v);
    d->cursor->setTeacherMode(v);
}

bool Editor::isTeacherMode() const
{
    return d->teacherMode;
}

void Editor::unsetAnalizer()
{
    d->analizer = 0;
}

void Editor::setDocumentId(int id)
{
    d->doc->documentId = id;
}

void Editor::lock()
{
    d->cursor->setEnabled(false);
    d->cut->setEnabled(false);
    d->paste->setEnabled(false);
    d->deleteLine->setEnabled(false);
    d->deleteTail->setEnabled(false);
    for (int i=0; i<d->userMacros.size(); i++) {
        d->userMacros[i].action->setEnabled(false);
    }
    for (int i=0; i<d->systemMacros.size(); i++) {
        d->systemMacros[i].action->setEnabled(false);
    }
}

void Editor::unlock()
{
    d->cursor->setEnabled(true);
    d->paste->setEnabled(d->clipboard->hasContent());
    d->cut->setEnabled(true);
    d->deleteLine->setEnabled(true);
    d->deleteTail->setEnabled(true);
    for (int i=0; i<d->userMacros.size(); i++) {
        d->userMacros[i].action->setEnabled(true);
    }
    for (int i=0; i<d->systemMacros.size(); i++) {
        d->systemMacros[i].action->setEnabled(true);
    }
}

void Editor::appendMarginText(int lineNo, const QString &text)
{
    if (lineNo>=0 && lineNo<d->doc->linesCount()) {
        if (!d->doc->marginTextAt(lineNo).isEmpty()) {
            d->doc->setMarginTextAt(lineNo, d->doc->marginTextAt(lineNo)+"; ");
        }
        d->doc->setMarginTextAt(lineNo, d->doc->marginTextAt(lineNo) + text);
    }
    update();
}

void Editor::setMarginText(int lineNo, const QString &text)
{
    if (lineNo>=0 && lineNo<d->doc->linesCount())
        d->doc->setMarginTextAt(lineNo, text);
    update();
}

void Editor::clearMarginText()
{
    for (int i=0; i<d->doc->linesCount(); i++) {
        d->doc->setMarginTextAt(i, "");
    }
    update();
}

void Editor::clearMarginText(int fromLine, int toLine)
{
    int start = qMin(qMax(0, fromLine), d->doc->linesCount()-1);
    int end = qMin(qMax(0, toLine), d->doc->linesCount()-1);
    for (int i=start; i<=end; i++) {
        d->doc->setMarginTextAt(i, "");
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

void EditorPrivate::handleCompleteCompilationRequiest(
    const QStringList & visibleText,
    const QStringList & hiddenText,
    int hiddenBaseLine
    )
{
    if (!analizer) {
        return;
    }
    QString vt;
    for (int i=0; i<visibleText.size(); i++) {
        vt += visibleText[i];
        if (i<visibleText.size()-1)
            vt += "\n";
    }
    analizer->setSourceText(doc->documentId, vt);
    if (teacherMode) {
        QString ht;
        for (int i=0; i<hiddenText.size(); i++) {
            ht += hiddenText[i];
            if (i<hiddenText.size()-1)
                ht += "\n";
        }
        analizer->setHiddenText(doc->documentId, ht, hiddenBaseLine);
    }
    updateFromAnalizer();
}

void EditorPrivate::handleLineAndTextChanged(const QStack<Shared::ChangeTextTransaction> & changes)
{
    if (!analizer) {
        return;
    }
    analizer->setHiddenTextBaseLine(doc->documentId, doc->hiddenLineStart());
    analizer->changeSourceText(doc->documentId, mergeTransactions(changes.toList()));
    updateFromAnalizer();
}

void EditorPrivate::updateFromAnalizer()
{
    QList<Shared::LineProp> props = analizer->lineProperties(doc->documentId);
    QList<QPoint> ranks = analizer->lineRanks(doc->documentId);
    QList<Error> errors = analizer->errors(doc->documentId);
    for (int i=0; i<doc->linesCount(); i++) {
        int oldIndent = doc->indentAt(i);
        if (i<ranks.size()) {
            doc->setIndentRankAt(i, ranks[i]);
        }
        if (i<props.size()) {
            doc->setHighlightAt(i, props[i].toList());
        }
        doc->clearErrorsAt(i);
        int newIndent = doc->indentAt(i);
        int diffIndent = newIndent - oldIndent;
        if (cursor->row()==i) {
            cursor->setColumn(qMax(cursor->column()+diffIndent,0));
        }
    }
    for (int i=0; i<errors.size(); i++) {
        Error err = errors[i];
        int lineNo = err.line;
        if (lineNo>=0) {
            doc->setErrorsAt(lineNo, doc->errorsAt(lineNo) << err.code);
        }
    }
    plane->update();
}

Clipboard * EditorPrivate::clipboard = 0;

Editor::Editor(bool initiallyNotSaved, QSettings * settings, AnalizerInterface * analizer, int documentId, QWidget *parent) :
    QWidget()
{
    setParent(parent);
    d = new EditorPrivate;
    d->q = this;
    d->teacherMode = false;
    d->notSaved = initiallyNotSaved;
    if (!d->clipboard)
        d->clipboard = new Clipboard;
    d->doc = new TextDocument(this, settings);
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

    connect(d->doc, SIGNAL(compilationRequest(QStack<Shared::ChangeTextTransaction>)),
            d, SLOT(handleLineAndTextChanged(QStack<Shared::ChangeTextTransaction>)));
    connect(d->doc, SIGNAL(completeCompilationRequest(QStringList,QStringList,int)),
            d, SLOT(handleCompleteCompilationRequiest(QStringList,QStringList,int)));

    connect(d->doc->undoStack(), SIGNAL(cleanChanged(bool)), this, SIGNAL(documentCleanChanged(bool)));

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

    connect(d->doc->undoStack(), SIGNAL(canRedoChanged(bool)), d->cursor, SLOT(handleRedoChanged(bool)));
    connect(d->doc->undoStack(), SIGNAL(canUndoChanged(bool)), d->cursor, SLOT(handleUndoChanged(bool)));
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

    undo = new QAction(plane);
    undo->setEnabled(false);
    undo->setText(QObject::tr("Undo last action"));
    undo->setIcon(QIcon::fromTheme("edit-undo", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-undo.png")));
    undo->setShortcut(QKeySequence::Undo);
    connect(cursor, SIGNAL(undoAvailable(bool)), undo, SLOT(setEnabled(bool)));
    QObject::connect(undo, SIGNAL(triggered()), q, SLOT(undo()));

    redo = new QAction(plane);
    redo->setEnabled(false);
    redo->setText(QObject::tr("Redo last undoed action"));
    redo->setIcon(QIcon::fromTheme("edit-redo", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-redo.png")));
    redo->setShortcut(QKeySequence::Redo);
    connect(cursor, SIGNAL(redoAvailable(bool)), redo, SLOT(setEnabled(bool)));
    QObject::connect(redo, SIGNAL(triggered()), q, SLOT(redo()));

    separator = new QAction(this);
    separator->setSeparator(true);

    menu_edit = new QMenu(tr("Edit"), 0);
    menu_edit->addAction(selectAll);
    menu_edit->addSeparator();
    menu_edit->addAction(cut);
    menu_edit->addAction(copy);
    menu_edit->addAction(paste);
    menu_edit->addSeparator();
    menu_edit->addAction(deleteLine);
    menu_edit->addAction(deleteTail);
    menu_edit->addSeparator();
    menu_edit->addAction(undo);
    menu_edit->addAction(redo);

    menu_insert = new QMenu(tr("Insert"), 0);
}

const TextCursor * Editor::cursor() const
{
    return d->cursor;
}

const TextDocument * Editor::document() const
{
    return d->doc;
}

TextCursor * Editor::cursor()
{
    return d->cursor;
}

TextDocument * Editor::document()
{
    return d->doc;
}

const Shared::AnalizerInterface * Editor::analizer() const
{
    return d->analizer;
}

Shared::AnalizerInterface * Editor::analizer()
{
    return d->analizer;
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

void Editor::setSettings(QSettings *s)
{
    d->settings = s;
    d->plane->m_settings = s;
}

QList<QAction*> Editor::toolbarActions()
{
    QList<QAction*> result;
    result << d->cut;
    result << d->copy;
    result << d->paste;
    result << d->separator;
    result << d->undo;
    result << d->redo;

    return result;
}

void Editor::undo()
{
    d->cursor->undo();
}

void Editor::redo()
{
    d->cursor->redo();
}

QList<QMenu*> Editor::menuActions()
{
    QList<QMenu*> result;
    result << d->menu_edit << d->menu_insert;
    return result;
}

QByteArray Editor::saveState() const
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << (*this);
    QByteArray data = buffer.data();
    QByteArray md5 = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    QByteArray version =
            QCoreApplication::instance()->applicationVersion().leftJustified(20, ' ')
            .toAscii();
    return md5 + version + data;
}

void Editor::restoreState(const QByteArray &data)
{
    if (data.size()>=36) {
        QByteArray checksum = data.mid(0,16);
        QString version = QString::fromAscii(data.mid(16,20)).trimmed();
        QString myVersion = QCoreApplication::instance()->applicationVersion();
        QByteArray d = data.mid(36);
        QByteArray md5 = QCryptographicHash::hash(d, QCryptographicHash::Md5);
        bool equal = true;
        for (int i=0; i<16; i++) {
            if (checksum[i]!=md5[i]) {
                equal = false;
                break;
            }
        }
        if (version>myVersion || (myVersion.contains("alpha") && myVersion!=version)) {
            qWarning() << "Can't restore state: version mismatch (my: "
                       << myVersion << ", required: " << version << ")";
        }
        else if (!equal) {
            qWarning() << "Can't restore state: MD5 checksum mismatch";
        }
        else {
            QDataStream stream(d);
            stream >> (*this);
        }
    }
    else {
        qWarning() << "Can't restore state: not enought data";
    }
    checkForClean();
}

void Editor::setKumFile(const KumFile::Data &data)
{
    d->doc->setKumFile(data, d->teacherMode);
    if (d->analizer && !d->teacherMode) {
        // Set hidden part manually, because of editor will not emit hidden text to analizer
        d->analizer->setSourceText(d->doc->documentId, data.visibleText);
        int hbl = -1;
        if (d->teacherMode) {
            hbl = data.visibleText.split("\n").size();
        }
        d->analizer->setHiddenText(d->doc->documentId, data.hiddenText, hbl);
        d->updateFromAnalizer();
    }
    d->plane->update();
    checkForClean();
}

KumFile::Data Editor::toKumFile() const
{
    return d->doc->toKumFile();
}

void Editor::ensureAnalized()
{
    d->doc->flushTransaction();
}

void Editor::setNotModified()
{
    d->doc->undoStack()->setClean();
    d->notSaved = false;
    emit documentCleanChanged(true);
}

bool Editor::isModified() const
{
    return d->notSaved || ! d->doc->undoStack()->isClean();
}

void Editor::checkForClean()
{
    emit documentCleanChanged(!isModified());
}

bool Editor::forceNotSavedFlag() const
{
    return d->notSaved;
}

void Editor::setForceNotSavedFlag(bool v)
{
    d->notSaved = v;
    checkForClean();
}

QDataStream & operator<< (QDataStream & stream, const Editor & editor)
{
    stream << KumFile::toString(editor.toKumFile());
    stream << editor.cursor()->row();
    stream << editor.cursor()->column();
    stream << quint8(editor.forceNotSavedFlag());
    stream << editor.document()->undoStack()->count();
    stream << editor.document()->undoStack()->cleanIndex();
    stream << editor.document()->undoStack()->index();
    for (int i=0; i<editor.document()->undoStack()->count(); i++) {
        const QUndoCommand * cmd = editor.document()->undoStack()->command(i);
        stream << cmd->id();
        if (cmd->id()==1) {
            const InsertCommand * insertCommand =
                    static_cast<const InsertCommand*>(cmd);
            stream << (*insertCommand);
        }
        if (cmd->id()==2) {
            const RemoveCommand * removeCommand =
                    static_cast<const RemoveCommand*>(cmd);
            stream << (*removeCommand);
        }
        if (cmd->id()==3) {
            const InsertBlockCommand * insertCommand =
                    static_cast<const InsertBlockCommand*>(cmd);
            stream << (*insertCommand);
        }
        if (cmd->id()==4) {
            const RemoveBlockCommand * removeCommand =
                    static_cast<const RemoveBlockCommand*>(cmd);
            stream << (*removeCommand);
        }
        if (cmd->id()==0xA0) {
            const ToggleLineProtectedCommand * toggleCommand =
                    static_cast<const ToggleLineProtectedCommand*>(cmd);
            stream << (*toggleCommand);
        }
    }

    return stream;
}

QDataStream & operator>> (QDataStream & stream, Editor & editor)
{
    QString txt;
    int row, col;
    stream >> txt;
    stream >> row;
    stream >> col;
    editor.setKumFile(KumFile::fromString(txt));
    editor.cursor()->setRow(row);
    editor.cursor()->setColumn(col);
    quint8 notsaved;
    stream >> notsaved;
    editor.setForceNotSavedFlag(bool(notsaved));
    int undoCount, cleanIndex, undoIndex;
    stream >> undoCount >> cleanIndex >> undoIndex;
    QUndoStack * undo = editor.document()->undoStack();
    TextDocument::noUndoRedo = true;
    for (int i=0; i<undoCount; i++) {
        if (i==cleanIndex)
            undo->setClean();
        int id;
        stream >> id;
        if (id==1) {
            InsertCommand * cmd = new InsertCommand(editor.document(),
                                                    editor.cursor(),
                                                    editor.analizer());
            stream >> (*cmd);
            undo->push(cmd);
        }
        if (id==2) {
            RemoveCommand * cmd = new RemoveCommand(editor.document(),
                                                    editor.cursor(),
                                                    editor.analizer());
            stream >> (*cmd);
            undo->push(cmd);
        }
        if (id==3) {
            InsertBlockCommand * cmd = new InsertBlockCommand(editor.document(),
                                                              editor.cursor(),
                                                              editor.analizer());
            stream >> (*cmd);
            undo->push(cmd);
        }
        if (id==4) {
            RemoveBlockCommand * cmd = new RemoveBlockCommand(editor.document(),
                                                              editor.cursor(),
                                                              editor.analizer());
            stream >> (*cmd);
            undo->push(cmd);
        }
        if (id==0xA0) {
            ToggleLineProtectedCommand * cmd = new ToggleLineProtectedCommand(editor.document(), -1);
            stream >> (*cmd);
            undo->push(cmd);
        }
    }
    undo->setIndex(undoIndex);
    TextDocument::noUndoRedo = false;
    return stream;
}

} // namespace Editor

#include "editor.moc"
