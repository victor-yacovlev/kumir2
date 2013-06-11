#include "editor.h"
#include "editor_p.h"
#include "editorplane.h"
#include "textcursor.h"
#include "textdocument.h"
#include "clipboard.h"
#include "utils.h"
#include "macro.h"
#include "settingspage.h"
#include "editcommands.h"
#include "widgets/cyrillicmenu.h"
#include "interfaces/actorinterface.h"
#include "extensionsystem/pluginmanager.h"

namespace Editor {

using namespace Shared;


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
    d->doc->id_ = id;
}

void Editor::lock()
{
    d->cursor->setEnabled(false);
    d->cut->setEnabled(false);
    d->paste->setEnabled(false);
    d->deleteLine->setEnabled(false);
    d->deleteTail->setEnabled(false);
    if (d->toggleComment)
        d->toggleComment->setEnabled(false);
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
    if (d->toggleComment)
        d->toggleComment->setEnabled(true);
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
        TextLine::Margin & margin = d->doc->marginAt(lineNo);
        if (!margin.text.isEmpty()) {
            margin.text += "; ";
        }
        margin.text += text;
    }
    update();
}

void Editor::setMarginText(int lineNo, const QString &text, const QColor & fgColor)
{
    if (lineNo>=0 && lineNo<d->doc->linesCount()) {
        TextLine::Margin & margin = d->doc->marginAt(lineNo);
        margin.text = text;
        margin.color = fgColor;
    }
    update();
}

void Editor::clearMarginText()
{
    for (uint i=0; i<d->doc->linesCount(); i++) {
        TextLine::Margin & margin = d->doc->marginAt(i);
        margin.text.clear();
    }
    update();
}

void Editor::clearMarginText(uint fromLine, uint toLine)
{
    uint start = qMin(qMax(0u, fromLine), d->doc->linesCount()-1);
    uint end = qMin(qMax(0u, toLine), d->doc->linesCount()-1);
    for (uint i=start; i<=end; i++) {
        TextLine::Margin & margin = d->doc->marginAt(i);
        margin.text.clear();
    }
    update();
}

void Editor::setLineHighlighted(int lineNo, const QColor &color, quint32 colStart, quint32 colEnd)
{
    d->plane->setLineHighlighted(lineNo, color, colStart, colEnd);
}

QList<QWidget*> Editor::statusbarWidgets()
{
    return QList<QWidget*>() << d->positionStatus << d->keybStatus;
}

void EditorPrivate::disableInsertActions()
{
    foreach (Macro m , userMacros) {
        m.action->setEnabled(false);
    }
    foreach (Macro m , systemMacros) {
        m.action->setEnabled(false);
    }
}

void EditorPrivate::enableInsertActions()
{
    foreach (Macro m , userMacros) {
        m.action->setEnabled(true);
    }
    foreach (Macro m , systemMacros) {
        m.action->setEnabled(true);
    }
}

void EditorPrivate::timerEvent(QTimerEvent *e)
{
    if (e->timerId()==timerId) {
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
    else if (e->timerId()==autoScrollTimerId) {
        e->accept();
        if (autoScrollStateY==-1) {
            if (verticalScrollBar->value()>0) {
                verticalScrollBar->setValue(verticalScrollBar->value()-verticalScrollBar->singleStep());
            }
        }
        else if (autoScrollStateY==1) {
            if (verticalScrollBar->value()<verticalScrollBar->maximum()) {
                verticalScrollBar->setValue(verticalScrollBar->value()+verticalScrollBar->singleStep());
            }
        }
        if (autoScrollStateX==-1) {
            if (horizontalScrollBar->value()>0) {
                horizontalScrollBar->setValue(horizontalScrollBar->value()-horizontalScrollBar->singleStep());
            }
        }
        else if (autoScrollStateX==1) {
            if (horizontalScrollBar->value()<horizontalScrollBar->maximum()) {
                horizontalScrollBar->setValue(horizontalScrollBar->value()+horizontalScrollBar->singleStep());
            }
        }
    }
}

void EditorPrivate::handleAutoScrollChange(char a)
{
    autoScrollStateY = a;
}

void EditorPrivate::handleAutoScrollChangeX(char a)
{
    autoScrollStateX = a;
}

void EditorPrivate::updatePosition(int row, int col)
{
    positionStatus->setText(tr("Row: %1, Col: %2").arg(row+1).arg(col+1));
}

void EditorPrivate::loadMacros()
{
    using namespace Shared;
    using namespace ExtensionSystem;
    const QString sharePath = QCoreApplication::instance()->property("sharePath").toString();
    const QString systemMacrosPath = sharePath+"/editor/macros.json";
    systemMacros = loadFromFile(systemMacrosPath);

    const QList<const KPlugin*> actorPlugins =
            PluginManager::instance()->loadedConstPlugins("Actor*");

    std::deque<QString> availableActorNames;

    foreach (const KPlugin* plugin, actorPlugins) {
        ActorInterface * actor = qobject_cast<ActorInterface*>(plugin);
        if (actor) {
            availableActorNames.push_back(actor->name());
        }
    }

    if (!availableActorNames.empty()) {
        for (size_t i=0; i<qMin(size_t(9), availableActorNames.size()); i++) {
            const QString & actorName = availableActorNames.at(i);
            const QString insertText = tr("import %1").arg(actorName);
            Macro macro;
            macro.title = insertText;
            macro.key = QString::number(i+1).at(0);
            macro.commands.push_back(
                        KeyCommand(KeyCommand::InsertImport, actorName)
                        );
            systemMacros.push_back(macro);
        }
    }

}

void EditorPrivate::updateInsertMenu()
{
    menu_insert->clear();
    const QString escComa = settings->value(SettingsPage::KeyPlayMacroShortcut, SettingsPage::DefaultPlayMacroShortcut).toString()+", ";
    for (int i=0; i<systemMacros.size(); i++) {
        Macro m = systemMacros[i];
        QKeySequence ks(escComa+QString(Utils::latinKey(m.key)));
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
        QKeySequence ks(escComa+QString(Utils::latinKey(m.key)));
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
//        if (i<visibleText.size()-1)
            vt += "\n";
    }
    for (int i=0; i<hiddenText.size(); i++) {
        vt += hiddenText[i];
        if (i<hiddenText.size()-1)
            vt += "\n";
    }
    analizer->setSourceText(doc->id_, vt);    
    updateFromAnalizer();
}



void EditorPrivate::updateFromAnalizer()
{
    QList<Shared::LineProp> props = analizer->lineProperties(doc->id_);
    QList<QPoint> ranks = analizer->lineRanks(doc->id_);
    QList<Error> errors = analizer->errors(doc->id_);
    std::vector<int> oldIndents(doc->linesCount(), 0);
    for (int i=0; i<doc->linesCount(); i++) {
        oldIndents[i] = doc->indentAt(i);
    }
    for (int i=0; i<doc->linesCount(); i++) {
        int oldIndent = oldIndents[i];
        if (i<ranks.size()) {
            doc->setIndentRankAt(i, ranks[i]);
        }
        if (i<props.size()) {
            doc->setHighlightAt(i, props[i].toList());
        }
        doc->marginAt(i).errors.clear();
        int newIndent = doc->indentAt(i);
        int diffIndent = newIndent - oldIndent;
        if (cursor->row()==i) {
            cursor->setColumn(qMax(cursor->column()+2*diffIndent, 0u));
        }
    }
    for (int i=0; i<errors.size(); i++) {
        Error err = errors[i];
        int lineNo = err.line;
        if (lineNo>=0) {
            doc->marginAt(lineNo).errors.append(err.code);
        }
    }
    plane->update();
}

Clipboard * EditorPrivate::clipboard = 0;

Editor::Editor(bool initiallyNotSaved, ExtensionSystem::SettingsPtr settings, AnalizerInterface * analizer, int documentId, QWidget *parent) :
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
    d->doc->setAnalizer(analizer);
    d->cursor = new TextCursor(d->doc, d->clipboard, analizer, settings);
    d->analizer = analizer;
    d->doc->id_ = documentId;
    d->settings = settings;
    d->horizontalScrollBar = new QScrollBar(Qt::Horizontal, this);
    d->verticalScrollBar = new QScrollBar(Qt::Vertical, this);
    d->plane = new EditorPlane(
                d->doc,
                d->analizer,
                this,
                d->cursor,
                d->clipboard,
                d->settings,
                d->horizontalScrollBar,
                d->verticalScrollBar,
                d->analizer!=NULL,
                this);

    d->findReplace = new FindReplace(d->doc, d->cursor, d->plane);

    d->keybStatus = new QLabel(0);
    d->keybStatus->setFixedWidth(90);
    d->positionStatus = new QLabel(0);
    d->positionStatus->setFixedWidth(120);
    d->timerId = d->startTimer(50);
    d->autoScrollTimerId = d->startTimer(100);
    d->autoScrollStateY = 0;
    d->autoScrollStateX = 0;

    connect(d->cursor, SIGNAL(positionChanged(int,int)),
            d, SLOT(updatePosition(int,int)));

    connect(d->plane, SIGNAL(requestAutoScroll(char)),
            d, SLOT(handleAutoScrollChange(char)));

    connect(d->plane, SIGNAL(requestAutoScrollX(char)),
            d, SLOT(handleAutoScrollChangeX(char)));

    d->updatePosition(d->cursor->row(), d->cursor->column());

    d->createActions();
    d->plane->addContextMenuAction(d->cut);
    d->plane->addContextMenuAction(d->copy);
    d->plane->addContextMenuAction(d->paste);


    connect(d->doc, SIGNAL(completeCompilationRequest(QStringList,QStringList,int)),
            d, SLOT(handleCompleteCompilationRequiest(QStringList,QStringList,int)), Qt::DirectConnection);

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
    connect(d->cursor, SIGNAL(signalizeNotEditable()), d->plane, SLOT(signalizeNotEditable()));

    connect(d->plane, SIGNAL(enableInsertActions()), d, SLOT(enableInsertActions()));
    connect(d->plane, SIGNAL(disableInsertActions()), d, SLOT(disableInsertActions()));
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

    find = new QAction(plane);
    find->setText(QObject::tr("Find..."));
    find->setIcon(QIcon::fromTheme("edit-find", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-find.png")));
    find->setShortcut(QKeySequence(QKeySequence::Find));
    QObject::connect(find, SIGNAL(triggered()),
                     findReplace, SLOT(showFind()));

    replace = new QAction(plane);
    replace->setText(QObject::tr("Replace..."));
    replace->setIcon(QIcon::fromTheme("edit-find-replace", QIcon(QApplication::instance()->property("sharePath").toString()+"/icons/edit-find-replace.png")));
    replace->setShortcut(QKeySequence(QKeySequence::Replace));
    QObject::connect(replace, SIGNAL(triggered()),
                     findReplace, SLOT(showReplace()));



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

    if (analizer) {
        toggleComment = new QAction(plane);
        toggleComment->setText(QObject::tr("(Un)Comment lines"));
        toggleComment->setShortcut(QKeySequence("Ctrl+/"));
        QObject::connect(toggleComment, SIGNAL(triggered()),
                         cursor, SLOT(toggleComment()));
    }
    else {
        toggleComment = 0;
    }

    separator = new QAction(this);
    separator->setSeparator(true);

    menu_edit = new QMenu(tr("Edit"), 0);
    menu_edit->addAction(undo);
    menu_edit->addAction(redo);
    menu_edit->addSeparator();
    menu_edit->addAction(selectAll);
    menu_edit->addSeparator();
    menu_edit->addAction(cut);
    menu_edit->addAction(copy);
    menu_edit->addAction(paste);
    menu_edit->addSeparator();
    menu_edit->addAction(find);
    menu_edit->addAction(replace);
    menu_edit->addSeparator();
    menu_edit->addAction(deleteLine);
    menu_edit->addAction(deleteTail);
    menu_edit->addSeparator();
    if (analizer) {
        menu_edit->addAction(toggleComment);
    }

    menu_insert = new Widgets::CyrillicMenu(tr("Insert"), 0);
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


void Editor::setSettings(ExtensionSystem::SettingsPtr s)
{
    d->settings = s;
    d->plane->settings_ = s;
    QFont defaultFont;
    defaultFont.setFamily(s->value(SettingsPage::KeyFontName, SettingsPage::defaultFontFamily()).toString());
    defaultFont.setPointSize(s->value(SettingsPage::KeyFontSize, SettingsPage::defaultFontSize).toInt());
    d->plane->setFont(defaultFont);
    d->plane->update();
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
    if (d->analizer) {
        d->analizer->setSourceText(
                    d->doc->id_,
                    data.visibleText + "\n" + data.hiddenText
                    );
        d->updateFromAnalizer();
    }
    d->plane->update();
    checkForClean();
}

void  Editor::setPlainText(const QString & data)
{
    d->doc->setPlainText(data);
    if (d->analizer) {
        d->analizer->setSourceText(d->doc->id_, data);
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
    d->doc->forceCompleteRecompilation(QPoint(d->cursor->column(), d->cursor->row()));
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


