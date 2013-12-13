#include "suggestionswindow.h"
#include "editor.h"
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
#include "findreplace.h"
#include "macroeditor.h"
#include "macrolisteditor.h"
#include "extensionsystem/pluginmanager.h"
namespace Editor {

using namespace Shared;

QSize Editor::minimumSizeHint() const
{
    int minW = 400;
    int minH = 0;
    if (horizontalScrollBar_->isVisible()) {
        minH = horizontalScrollBar_->height();
    }
    QFontMetrics fm(plane_->font());
    minH += fm.lineSpacing() + fm.height(); // at least 1 line of text
    return QSize(minW, minH);
}


void Editor::lock()
{
    cursor_->setEnabled(false);
    cut_->setEnabled(false);
    paste_->setEnabled(false);
    deleteLine_->setEnabled(false);
    deleteTail_->setEnabled(false);
    if (toggleComment_)
        toggleComment_->setEnabled(false);
    for (int i=0; i<userMacros_.size(); i++) {
        userMacros_[i].action->setEnabled(false);
    }
    for (int i=0; i<systemMacros_.size(); i++) {
        systemMacros_[i].action->setEnabled(false);
    }
}

void Editor::unlock()
{
    cursor_->setEnabled(true);
    paste_->setEnabled(Clipboard::instance()->hasContent());
    cut_->setEnabled(true);
    deleteLine_->setEnabled(true);
    deleteTail_->setEnabled(true);
    if (toggleComment_)
        toggleComment_->setEnabled(true);
    for (int i=0; i<userMacros_.size(); i++) {
        userMacros_[i].action->setEnabled(true);
    }
    for (int i=0; i<systemMacros_.size(); i++) {
        systemMacros_[i].action->setEnabled(true);
    }
}

void Editor::appendMarginText(int lineNo, const QString &text)
{
    if (lineNo>=0 && lineNo<doc_->linesCount()) {
        TextLine::Margin & margin = doc_->marginAt(lineNo);
        if (!margin.text.isEmpty()) {
            margin.text += "; ";
        }
        margin.text += text;
    }
    update();
}

void Editor::loadDocument(QIODevice *device, const QString &fileNameSuffix,
                          const QString &sourceEncoding, const QUrl &sourceUrl)
{
    Shared::AnalizerInterface * analizerPlugin = nullptr;

    ExtensionSystem::PluginManager * manager =
            ExtensionSystem::PluginManager::instance();

    QList<Shared::AnalizerInterface*> analizers =
            manager->findPlugins<Shared::AnalizerInterface>();

    for (int i=0; i<analizers.size(); i++) {
        if (analizers[i]->defaultDocumentFileNameSuffix() == fileNameSuffix) {
            analizerPlugin = analizers[i];
            break;
        }
    }

    bool keepIndents = analizerPlugin==nullptr || analizerPlugin->indentsSignificant();

    const QByteArray bytes = device->readAll();

    KumFile::Data data = KumFile::fromString(
                KumFile::readRawDataAsString(bytes, sourceEncoding, fileNameSuffix),
                keepIndents
                );
    data.canonicalSourceLanguageName = fileNameSuffix;
    data.sourceUrl = sourceUrl;
    loadDocument(data);
}

void Editor::loadDocument(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        const QString localPath = QFileInfo(f).absoluteFilePath();
        const QString suffix = QFileInfo(f).suffix();
        const QUrl url = QUrl::fromLocalFile(localPath);
        QString error;
        try {
            loadDocument(&f, suffix, QString(), url);
        }
        catch (const QString & e) {
            error = e;
        }
        f.close();
        if (error.length() > 0) {
            throw error;
        }
    }
    else {
        throw tr("Can't open file %1 for reading").arg(fileName);
    }
}

void Editor::loadDocument(const KumFile::Data &data)
{
    Shared::AnalizerInterface * analizerPlugin = nullptr;
    Shared::Analizer::InstanceInterface * analizerInstance = nullptr;

    QList<Shared::AnalizerInterface*> analizers =
            ExtensionSystem::PluginManager::instance()
            ->findPlugins<Shared::AnalizerInterface>();

    for (int i=0; i<analizers.size(); i++) {
        if (analizers[i]->defaultDocumentFileNameSuffix() == data.canonicalSourceLanguageName) {
            analizerPlugin = analizers[i];
            analizerInstance = analizerPlugin->createInstance();
            if (data.sourceUrl.isLocalFile()) {
                const QString localPath = data.sourceUrl.toLocalFile();
                const QString dirName = QFileInfo(localPath).absoluteDir().path();
                analizerInstance->setSourceDirName(dirName);
            }
            break;
        }
    }

    analizerPlugin_ = analizerPlugin;
    analizerInstance_ = analizerInstance;
    documentUrl_ = data.sourceUrl;
    setKumFile(data);
}

void Editor::setMarginText(int lineNo, const QString &text, const QColor & fgColor)
{
    if (lineNo>=0 && lineNo<doc_->linesCount()) {
        TextLine::Margin & margin = doc_->marginAt(lineNo);
        margin.text = text;
        margin.color = fgColor;
    }
    update();
}

void Editor::clearMarginText()
{
    for (uint i=0; i<doc_->linesCount(); i++) {
        TextLine::Margin & margin = doc_->marginAt(i);
        margin.text.clear();
    }
    update();
}

void Editor::clearMarginText(int fromLine, int toLine)
{
    int start = qMin(qMax(0, fromLine), int(doc_->linesCount())-1);
    int end = qMin(qMax(0, toLine), int(doc_->linesCount())-1);
    if (toLine == -1)
        end = doc_->linesCount() - 1;
    for (int i=start; i<=end; i++) {
        TextLine::Margin & margin = doc_->marginAt(i);
        margin.text.clear();
    }
    update();
}

void Editor::setLineHighlighted(int lineNo, const QColor &color, quint32 colStart, quint32 colEnd)
{
    plane_->setLineHighlighted(lineNo, color, colStart, colEnd);
}


void Editor::disableInsertActions()
{
    foreach (Macro m , userMacros_) {
        m.action->setEnabled(false);
    }
    foreach (Macro m , systemMacros_) {
        m.action->setEnabled(false);
    }
}

void Editor::enableInsertActions()
{
    foreach (Macro m , userMacros_) {
        m.action->setEnabled(true);
    }
    foreach (Macro m , systemMacros_) {
        m.action->setEnabled(true);
    }
}

void Editor::timerEvent(QTimerEvent *e)
{
    if (e->timerId()==timerId_) {
        e->accept();
        emit keyboardLayoutChanged(
                    Utils::isRussianLayout()? QLocale::Russian : QLocale::English,
                    Utils::isCapsLock(),
                    Utils::shiftKeyPressed,
                    Utils::altKeyPressed
                    );
    }
    else if (e->timerId()==autoScrollTimerId_) {
        e->accept();
        if (autoScrollStateY_==-1) {
            if (verticalScrollBar_->value()>0) {
                verticalScrollBar_->setValue(verticalScrollBar_->value()-verticalScrollBar_->singleStep());
            }
        }
        else if (autoScrollStateY_==1) {
            if (verticalScrollBar_->value()<verticalScrollBar_->maximum()) {
                verticalScrollBar_->setValue(verticalScrollBar_->value()+verticalScrollBar_->singleStep());
            }
        }
        if (autoScrollStateX_==-1) {
            if (horizontalScrollBar_->value()>0) {
                horizontalScrollBar_->setValue(horizontalScrollBar_->value()-horizontalScrollBar_->singleStep());
            }
        }
        else if (autoScrollStateX_==1) {
            if (horizontalScrollBar_->value()<horizontalScrollBar_->maximum()) {
                horizontalScrollBar_->setValue(horizontalScrollBar_->value()+horizontalScrollBar_->singleStep());
            }
        }
    }
}

void Editor::handleAutoScrollChange(char a)
{
    autoScrollStateY_ = a;
}

void Editor::handleAutoScrollChangeX(char a)
{
    autoScrollStateX_ = a;
}

void Editor::updatePosition(int row, int col)
{
    emit cursorPositionChanged(row, col);
}

void Editor::loadMacros()
{
    if (analizerPlugin_ == nullptr) {
        return;
    }
    using namespace Shared;
    using namespace ExtensionSystem;
    const QString analizerName = analizerPlugin_->defaultDocumentFileNameSuffix();

    // System macros
    const QString systemMacrosPath = plugin_->myResourcesDir().absoluteFilePath("macros-"+analizerName+".xml");
    systemMacros_ = loadFromFile(systemMacrosPath);

    // Actor-specific macros
    if (analizerName == "kum") {
        const QList<const KPlugin*> actorPlugins =
                PluginManager::instance()->loadedConstPlugins("Actor*");

        foreach (const KPlugin* plugin, actorPlugins) {
            const QString actorMacrosFileName = plugin_->myResourcesDir().absoluteFilePath(
                        "editor/macros-" + plugin->pluginSpec().name + ".xml");
            if (QFile::exists(actorMacrosFileName)) {
                systemMacros_.push_back(Macro());
                systemMacros_ += loadFromFile(actorMacrosFileName);
            }
        }
    }

    // Import macros
    if (analizerName == "kum") {
        const QList<const KPlugin*> actorPlugins =
                PluginManager::instance()->loadedConstPlugins("Actor*");

        std::deque<QString> availableActorNames;

        foreach (const KPlugin* plugin, actorPlugins) {
            ActorInterface * actor = qobject_cast<ActorInterface*>(plugin);
            if (actor && !actor->localizedModuleName(QLocale::Russian).startsWith("_")) {
                availableActorNames.push_back(actor->localizedModuleName(QLocale::Russian));
            }
        }

        if (!availableActorNames.empty()) {
            systemMacros_.push_back(Macro());
            for (size_t i=0; i<qMin(size_t(9), availableActorNames.size()); i++) {
                const QString & actorName = availableActorNames.at(i);
                const QString insertText = tr("import %1").arg(actorName);
                Macro macro;
                macro.title = insertText;
                macro.key = QString::number(i+1).at(0);
                macro.commands.push_back(
                            KeyCommand(KeyCommand::InsertImport, actorName)
                            );
                systemMacros_.push_back(macro);
            }
        }
    }


    // User macros
    QString fileName = analizerName.length() > 0
            ? QString::fromAscii(".user-macros-%1.xml").arg(analizerName)
            : QString::fromAscii(".user-macros.xml");

    const QString dirName = mySettings()->locationDirectory();
    if (dirName.startsWith(QDir::homePath() + "/."))
        fileName.remove(0, 1);
    QDir dir(dirName);
    const QString filePath = dir.absoluteFilePath(fileName);
    userMacros_ = loadFromFile(filePath);
}

ExtensionSystem::SettingsPtr Editor::mySettings() const
{
    return plugin_->mySettings();
}

void Editor::updateInsertMenu()
{
    loadMacros();
    insertMenu_->clear();
    const QString escComa = "Esc, ";
    for (int i=0; i<systemMacros_.size(); i++) {
        Macro m = systemMacros_[i];
        if (m.title.isEmpty()) {
            // Separator
            systemMacros_[i].action = insertMenu_->addSeparator();
        }
        else {
            m.action = new QAction(m.title, insertMenu_);
            systemMacros_[i].action = m.action;
            insertMenu_->addAction(m.action);
            connect(m.action, SIGNAL(triggered()), this, SLOT(playMacro()));
            if (!m.key.isNull()) {
                const QKeySequence ks(escComa+QString(Utils::latinKey(m.key)));
                const QKeySequence ks2(escComa + QString(m.key));
                if (ks == ks2) {
                    m.action->setShortcut(ks);
                }
                else {
                    const QList<QKeySequence> shortcuts = QList<QKeySequence>() << ks << ks2;
                    m.action->setShortcuts(shortcuts);
                }
            }
            else if (uint(m.extKey) != 0u) {
                QString repr;
                if (m.extKey == Qt::Key_Left)
                    repr = "Left";
                else if (m.extKey == Qt::Key_Right)
                    repr = "Right";
                else if (m.extKey == Qt::Key_Up)
                    repr = "Up";
                else if (m.extKey == Qt::Key_Down)
                    repr = "Down";
                else if (m.extKey == Qt::Key_Space)
                    repr = "Space";
                const QKeySequence ks(escComa + repr);
                m.action->setShortcut(ks);
            }

        }
    }
    if (!userMacros_.isEmpty())
        insertMenu_->addSeparator();
    for (int i=0; i<userMacros_.size(); i++) {
        Macro m = userMacros_[i];
        m.action = new QAction(m.title, insertMenu_);
        if (!m.key.isNull()) {
            const QKeySequence ks(escComa+QString(Utils::latinKey(m.key)));
            const QKeySequence ks2(escComa + QString(m.key));
            const QList<QKeySequence> shortcuts = QList<QKeySequence>() << ks << ks2;
            m.action->setShortcuts(shortcuts);
        }
        userMacros_[i].action = m.action;
        insertMenu_->addAction(m.action);
        connect(m.action, SIGNAL(triggered()), this, SLOT(playMacro()));
    }
    editMacros_->setEnabled(userMacros_.size() > 0);
}

void Editor::playMacro()
{
    QAction * a = qobject_cast<QAction*>(sender());
    Q_CHECK_PTR(a);
    Macro m;
    bool found = false;
    foreach (Macro mm, systemMacros_) {
        if (mm.action==a) {
            found = true;
            m = mm;
            break;
        }
    }
    if (!found) {
        foreach (Macro mm, userMacros_) {
            if (mm.action==a) {
                found = true;
                m = mm;
                break;
            }
        }
    }
    if (found) {
        document()->undoStack()->beginMacro(m.title);
        for (int i=0; i<m.commands.size(); i++) {
            cursor_->evaluateCommand(m.commands[i]);
        }
        document()->undoStack()->endMacro();
        plane_->updateScrollBars();
        plane_->ensureCursorVisible();
    }
}


void Editor::focusInEvent(QFocusEvent *e)
{
    QWidget::focusInEvent(e);
    plane_->setFocus();
}



void Editor::handleCompleteCompilationRequiest(
    const QStringList & visibleText,
    const QStringList & hiddenText,
    int hiddenBaseLine
    )
{
    if (!analizerPlugin_) {
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
    analizerInstance_->setSourceText(vt);
    updateFromAnalizer();
}



void Editor::updateFromAnalizer()
{
    QList<Shared::Analizer::LineProp> props = analizerInstance_->lineProperties();
    QList<QPoint> ranks = analizerInstance_->lineRanks();
    QList<Shared::Analizer::Error> errors = analizerInstance_->errors();
    std::vector<int> oldIndents(doc_->linesCount(), 0);
    for (int i=0; i<doc_->linesCount(); i++) {
        oldIndents[i] = doc_->indentAt(i);
    }
    for (int i=0; i<doc_->linesCount(); i++) {
        int oldIndent = oldIndents[i];
        if (i<ranks.size()) {
            doc_->setIndentRankAt(i, ranks[i]);
        }
        if (i<props.size()) {
            doc_->setHighlightAt(i, props[i].toList());
        }
        doc_->marginAt(i).errors.clear();
        int newIndent = doc_->indentAt(i);
        int diffIndent = newIndent - oldIndent;
        if (cursor_->row()==i) {
            cursor_->setColumn(qMax(cursor_->column()+2*diffIndent, 0u));
        }
    }
    for (int i=0; i<errors.size(); i++) {
        Shared::Analizer::Error err = errors[i];
        int lineNo = err.line;
        if (lineNo>=0) {
            doc_->marginAt(lineNo).errors.append(err.code);
        }
    }
    plane_->update();
}

Editor::Editor(
        EditorPlugin * plugin,
        bool initiallyNotSaved,
        AnalizerInterface * analizerPlugin,
        Analizer::InstanceInterface * analizerInstance)
    : QWidget()
    , plugin_(plugin)
    , analizerPlugin_(analizerPlugin)
    , analizerInstance_(analizerInstance)
    , doc_(new TextDocument(this))
    , cursor_(new TextCursor(this))
    , plane_(new EditorPlane(this))
    , notSaved_(initiallyNotSaved)
    , findReplace_(new FindReplace(plugin->myResourcesDir(), this))
    , autocompleteWidget_(new SuggestionsWindow(this))
    , autoScrollStateX_(0)
    , autoScrollStateY_(0)
{
    setupUi();
    setupStyleSheets();
    createActions();
    updateInsertMenu();

    plane_->addContextMenuAction(cut_);
    plane_->addContextMenuAction(copy_);
    plane_->addContextMenuAction(paste_);

    createConnections();

    timerId_ = startTimer(50);
    autoScrollTimerId_ = startTimer(100);   

    updateSettings(QStringList());
    updatePosition(cursor_->row(), cursor_->column());

    plane_->updateScrollBars();
}

void Editor::setupUi()
{
    horizontalScrollBar_ = new QScrollBar(Qt::Horizontal, this);
    verticalScrollBar_ = new QScrollBar(Qt::Vertical, this);
    QGridLayout * l = new QGridLayout();
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(plane_, 0, 0);
    l->addWidget(verticalScrollBar_, 0, 1);
    l->addWidget(horizontalScrollBar_, 1, 0);
    autocompleteWidget_->setVisible(false);
}

void Editor::setupStyleSheets()
{
    static const char * ScrollBarCSS = ""
            "QScrollBar {"
            "   width: 12px;"
            "   background-color: transparent;"
            "   padding-right: 4px;"
            "   border: 0;"
            "}"
            "QScrollBar:handle {"
            "   background-color: gray;"
            "   border-radius: 4px;"
            "}"
            "QScrollBar:add-line {"
            "   height: 0;"
            "}"
            "QScrollBar:sub-line {"
            "   height: 0;"
            "}"
            ;
    static const char * HorizontalScrollBarCSS = ""
            "QScrollBar {"
            "   height: 12px;"
            "   background-color: transparent;"
            "   padding-right: 4px;"
            "   border: 0;"
            "   border-right: 4px solid #FF8080;"
            "}"
            "QScrollBar:handle {"
            "   background-color: gray;"
            "   border-radius: 4px;"
            "}"
            "QScrollBar:add-line {"
            "   height: 0;"
            "}"
            "QScrollBar:sub-line {"
            "   height: 0;"
            "}"
            ;
    verticalScrollBar_->setStyleSheet(ScrollBarCSS);
    horizontalScrollBar_->setStyleSheet(HorizontalScrollBarCSS);
}

void Editor::createConnections()
{
    installEventFilter(this);
    plane_->installEventFilter(this);
    verticalScrollBar_->installEventFilter(this);
    qApp->installEventFilter(plane_);

    connect(plane_, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
    connect(cursor_, SIGNAL(positionChanged(int,int)),
            this, SLOT(updatePosition(int,int)));

    connect(plane_, SIGNAL(requestAutoScroll(char)),
            this, SLOT(handleAutoScrollChange(char)));

    connect(plane_, SIGNAL(requestAutoScrollX(char)),
            this, SLOT(handleAutoScrollChangeX(char)));

    connect(doc_, SIGNAL(completeCompilationRequest(QStringList,QStringList,int)),
            this, SLOT(handleCompleteCompilationRequiest(QStringList,QStringList,int)), Qt::DirectConnection);

    connect(doc_->undoStack(), SIGNAL(cleanChanged(bool)), this, SIGNAL(documentCleanChanged(bool)));

    connect(plane_, SIGNAL(urlsDragAndDropped(QList<QUrl>)), this, SIGNAL(urlsDragAndDropped(QList<QUrl>)));

    connect(doc_->undoStack(), SIGNAL(canRedoChanged(bool)), cursor_, SLOT(handleRedoChanged(bool)));
    connect(doc_->undoStack(), SIGNAL(canUndoChanged(bool)), cursor_, SLOT(handleUndoChanged(bool)));


    connect(plane_, SIGNAL(enableInsertActions()), this, SLOT(enableInsertActions()));
    connect(plane_, SIGNAL(disableInsertActions()), this, SLOT(disableInsertActions()));

    connect(horizontalScrollBar_, SIGNAL(valueChanged(int)), plane_, SLOT(update()));
    connect(verticalScrollBar_, SIGNAL(valueChanged(int)), plane_, SLOT(update()));

    connect(cursor_, SIGNAL(updateRequest()), plane_, SLOT(updateCursor()));
    connect(cursor_, SIGNAL(updateRequest(int,int)), plane_, SLOT(updateText(int,int)));

    connect(autocompleteWidget_.data(), SIGNAL(requestHelpForAlgorithm(QString)),
            this, SIGNAL(requestHelpForAlgorithm(QString)));
    connect(autocompleteWidget_.data(), SIGNAL(hidden()), plane_, SIGNAL(enableInsertActions()));
    connect(autocompleteWidget_.data(), SIGNAL(acceptedSuggestion(QString)),
            plane_, SLOT(finishAutoCompletion(QString)));

}

QScrollBar * Editor::scrollBar(Qt::Orientation orientation)
{
    if (orientation == Qt::Horizontal) {
        return horizontalScrollBar_;
    }
    else {
        return verticalScrollBar_;
    }
}

void Editor::paintEvent(QPaintEvent * e)
{
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(palette().brush(QPalette::Base));
    p.drawRect(0, 0, width(), height());
    QWidget::paintEvent(e);
    p.end();
    p.begin(this);
    const QBrush br = plane_->hasFocus()
            ? palette().brush(QPalette::Highlight)
            : palette().brush(QPalette::Window);
    p.setPen(QPen(br, 3));
    p.drawLine(width()-1, 0, width()-1, height()-1);
    p.end();
    e->accept();
}

bool Editor::eventFilter(QObject *obj, QEvent *evt)
{
    if (obj == verticalScrollBar_ && evt->type() == QEvent::Paint) {
        QPainter p(verticalScrollBar_);
        const QBrush br = plane_->hasFocus()
                ? palette().brush(QPalette::Highlight)
                : palette().brush(QPalette::Window);
        p.setPen(QPen(br, 3));
        p.drawLine(0, 0,
                   verticalScrollBar_->width()-1, 0);
        p.drawLine(0, verticalScrollBar_->height()-1,
                   verticalScrollBar_->width()-1, verticalScrollBar_->height()-1);
        p.end();
    }
    else if (obj == plane_) {
        if (evt->type() == QEvent::FocusIn || evt->type() == QEvent::FocusOut) {
            verticalScrollBar_->repaint();
        }
    }
    return false;
}

void Editor::createActions()
{
    const QString qtcreatorIconsPath =
            ExtensionSystem::PluginManager::instance()->sharePath()
            + "/icons/from_qtcreator/";

    selectAll_ = new QAction(plane_);
    selectAll_->setText(QObject::tr("Select all text in editor"));
    selectAll_->setShortcut(QKeySequence(QKeySequence::SelectAll));
    selectAll_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(selectAll_, SIGNAL(triggered()), plane_, SLOT(selectAll()));

    copy_ = new QAction(plane_);
    copy_->setText(QObject::tr("Copy selection to clipboard"));
    copy_->setIcon(QIcon(qtcreatorIconsPath+"editcopy.png"));
    copy_->setShortcut(QKeySequence(QKeySequence::Copy));
    copy_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(copy_, SIGNAL(triggered()), plane_, SLOT(copy()));

    cut_ = new QAction(plane_);
    cut_->setText(QObject::tr("Cut selection to clipboard"));
    cut_->setIcon(QIcon(qtcreatorIconsPath+"editcut.png"));
    cut_->setShortcut(QKeySequence(QKeySequence::Cut));
    cut_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(cut_, SIGNAL(triggered()), plane_, SLOT(cut()));

    paste_ = new QAction(plane_);
    paste_->setText(QObject::tr("Paste from clipboard"));
    paste_->setIcon(QIcon(qtcreatorIconsPath+"editpaste.png"));
    paste_->setShortcut(QKeySequence(QKeySequence::Paste));
    paste_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(paste_, SIGNAL(triggered()), plane_, SLOT(paste()));

    find_ = new QAction(plane_);
    find_->setText(QObject::tr("Find..."));
    find_->setShortcut(QKeySequence(QKeySequence::Find));
    find_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(find_, SIGNAL(triggered()),
                     findReplace_, SLOT(showFind()));

    replace_ = new QAction(plane_);
    replace_->setText(QObject::tr("Replace..."));
    replace_->setShortcut(QKeySequence(QKeySequence::Replace));
    replace_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(replace_, SIGNAL(triggered()),
                     findReplace_, SLOT(showReplace()));



    deleteLine_ = new QAction(plane_);
    deleteLine_->setText(QObject::tr("Delete line under cursor"));
    deleteLine_->setShortcut(QKeySequence("Ctrl+Y"));
    deleteLine_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(deleteLine_, SIGNAL(triggered()), plane_, SLOT(removeLine()));

    deleteTail_ = new QAction(plane_);
    deleteTail_->setText(QObject::tr("Delete text from cursor to end of line"));
    deleteTail_->setShortcut(QKeySequence("Ctrl+K"));
    deleteTail_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(deleteTail_, SIGNAL(triggered()), plane_, SLOT(removeLineTail()));

    undo_ = new QAction(plane_);
    undo_->setEnabled(false);
    undo_->setText(QObject::tr("Undo last action"));
    undo_->setIcon(QIcon(qtcreatorIconsPath+"undo.png"));
    undo_->setShortcut(QKeySequence::Undo);
    undo_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(cursor_, SIGNAL(undoAvailable(bool)), undo_, SLOT(setEnabled(bool)));
    QObject::connect(undo_, SIGNAL(triggered()), this, SLOT(undo()));

    redo_ = new QAction(plane_);
    redo_->setEnabled(false);
    redo_->setText(QObject::tr("Redo last undoed action"));
    redo_->setIcon(QIcon(qtcreatorIconsPath+"redo.png"));
    redo_->setShortcut(QKeySequence::Redo);
    redo_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(cursor_, SIGNAL(redoAvailable(bool)), redo_, SLOT(setEnabled(bool)));
    QObject::connect(redo_, SIGNAL(triggered()), this, SLOT(redo()));

    toggleComment_ = new QAction(plane_);
    toggleComment_->setText(QObject::tr("(Un)Comment lines"));
    toggleComment_->setShortcut(QKeySequence("Ctrl+/"));
    toggleComment_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(toggleComment_, SIGNAL(triggered()),
                     cursor_, SLOT(toggleComment()));

    recordMacro_ = new QAction(plane_);
    recordMacro_->setEnabled(true);
    recordMacro_->setCheckable(true);
    recordMacro_->setText(tr("Record keyboard sequence"));
    recordMacro_->setShortcut(QKeySequence("Ctrl+M"));
    recordMacro_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(recordMacro_, SIGNAL(triggered(bool)), this, SLOT(toggleRecordMacro(bool)));

    editMacros_ = new QAction(plane_);
    editMacros_->setText(tr("Edit saved keyboard sequences..."));
    connect(editMacros_, SIGNAL(triggered()), this, SLOT(editMacros()));


    separatorAction_ = new QAction(this);
    separatorAction_->setSeparator(true);

    editMenu_ = new QMenu(tr("Edit"), 0);
    editMenu_->addAction(undo_);
    editMenu_->addAction(redo_);
    editMenu_->addSeparator();
    editMenu_->addAction(selectAll_);
    editMenu_->addSeparator();
    editMenu_->addAction(cut_);
    editMenu_->addAction(copy_);
    editMenu_->addAction(paste_);
    editMenu_->addSeparator();
    editMenu_->addAction(find_);
    editMenu_->addAction(replace_);
    editMenu_->addSeparator();
    editMenu_->addAction(deleteLine_);
    editMenu_->addAction(deleteTail_);


    editMenu_->addSeparator();
    editMenu_->addAction(toggleComment_);


    editMenu_->addSeparator();
    editMenu_->addAction(recordMacro_);
    editMenu_->addAction(editMacros_);

    insertMenu_ = new Widgets::CyrillicMenu(tr("Insert"), 0);
}

const TextCursor * Editor::cursor() const
{
    return cursor_;
}

const TextDocument * Editor::document() const
{
    return doc_;
}

TextCursor * Editor::cursor()
{
    return cursor_;
}

TextDocument * Editor::document()
{
    return doc_;
}

Shared::Analizer::InstanceInterface * Editor::analizer()
{
    return analizerInstance_;
}


Editor::~Editor()
{
    delete doc_;
    plane_->deleteLater();
    killTimer(timerId_);
}


QList<QAction*> Editor::toolBarActions() const
{
    QList<QAction*> result;
    result << cut_;
    result << copy_;
    result << paste_;
    result << separatorAction_;
    result << undo_;
    result << redo_;

    return result;
}

void Editor::undo()
{
    cursor_->undo();
}

void Editor::redo()
{
    cursor_->redo();
}

void Editor::toggleRecordMacro(bool on)
{
    if (on) {
        cursor_->startRecordMacro();
    }
    else {
        Macro * macro = cursor_->endRecordMacro();
        if (macro->commands.size() > 0) {
            MacroEditor * editor = new MacroEditor(this);
            editor->setWindowTitle(tr("New keyboard sequence..."));
            QList<Macro> allMacros = systemMacros_ + userMacros_;
            QString usedLetters;
            QStringList usedNames;
            foreach(Macro m, allMacros) {
                if (!m.key.isNull()) {
                    usedLetters.push_back(m.key);
                    usedNames.push_back(m.title);
                }
            }
            editor->setUsedSymbols(usedLetters, usedNames);
            editor->setMacro(macro);
            if (editor->exec() == QDialog::Accepted) {
                Macro newMacro = *macro;
                userMacros_.push_back(newMacro);
                plugin_->updateUserMacros(analizerPlugin_ ? analizerPlugin_->defaultDocumentFileNameSuffix() : QString(), userMacros_, true);
            }
            editor->deleteLater();
        }
        delete macro;
    }
    emit recordMacroChanged(on);
}

void Editor::editMacros()
{
    MacroListEditor * editor = new MacroListEditor(plugin_->myResourcesDir(), this);
    editor->initialize(userMacros_,
                       systemMacros_);
    editor->exec();
    userMacros_ = editor->result();
    plugin_->updateUserMacros(analizerPlugin_ ? analizerPlugin_->defaultDocumentFileNameSuffix() : QString(), userMacros_, true);
    editor->deleteLater();
}

QList<QMenu*> Editor::menus() const
{
    QList<QMenu*> result;
    result << editMenu_;
    bool nonEmptyInsertMenu = false;
    for (int i=0; i<insertMenu_->children().size(); i++) {
        QObject * child = insertMenu_->children().at(i);
        const QString clazz = child->metaObject()->className();
        if (clazz=="QAction" && child != insertMenu_->menuAction()) {
            nonEmptyInsertMenu = true;
            break;
        }
    }
    if (nonEmptyInsertMenu)
        result << insertMenu_;
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
    notSaved_ = true;
    doc_->setKumFile(data, plugin_->teacherMode_);
    if (analizerInstance_) {
        toggleComment_->setVisible(true);
        toggleComment_->setEnabled(true);
        analizerInstance_->setSourceText(data.visibleText + "\n" + data.hiddenText);
        updateFromAnalizer();
    }
    else {
        toggleComment_->setVisible(false);
        toggleComment_->setEnabled(false);
    }
    updateInsertMenu();
    plane_->setLineHighlighted(-1, QColor(), 0, 0);
    plane_->updateScrollBars();
    plane_->update();
    checkForClean();
    setNotModified();
}

void  Editor::setPlainText(const QString & data)
{
    doc_->setPlainText(data);
    if (analizerInstance_) {
        analizerInstance_->setSourceText(data);
        updateFromAnalizer();
    }
    plane_->setLineHighlighted(-1, QColor(), 0, 0);
    plane_->update();
    checkForClean();
}

KumFile::Data Editor::documentContents() const
{
    KumFile::Data data = doc_->toKumFile();
    data.sourceUrl = documentUrl_;
    return data;
}

void Editor::saveDocument(const QString &fileName)
{
    QFile f(fileName);
    if (f.open(QIODevice::WriteOnly|QIODevice::Text)) {
        saveDocument(&f);
        f.close();
        documentUrl_ = QUrl::fromLocalFile(fileName);
    }
    else {
        throw tr("Can't open file %1 for writing").arg(fileName);
    }
}

void Editor::saveDocument(QIODevice *device)
{
    QDataStream ds(device);
    ds << documentContents();    
    notSaved_ = false;
    checkForClean();
    doc_->undoStack()->setClean();
}

quint32 Editor::errorLinesCount() const
{
    QSet<int> lines;
    if (analizerInstance_) {
        QList<Shared::Analizer::Error> errors = analizerInstance_->errors();
        foreach (const Shared::Analizer::Error & e, errors) {
            if (e.line >= 0) {
                lines.insert(e.line);
            }
        }
    }
    return lines.size();
}

void Editor::highlightLineGreen(int lineNo, quint32 colStart, quint32 colEnd)
{
    const QColor bgColor = palette().color(QPalette::Base);
    int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
    QColor color;
    if (darkness / 3 <= 127) {
        color = QColor("palegreen");
    }
    else {
        color = QColor(Qt::darkGreen);
    }
    setLineHighlighted(lineNo, color, colStart, colEnd);
}

void Editor::highlightLineRed(int lineNo, quint32 colStart, quint32 colEnd)
{
    const QColor bgColor = palette().color(QPalette::Base);
    int darkness = bgColor.red() + bgColor.green() + bgColor.blue();
    QColor color;
    if (darkness / 3 <= 127) {
        color = QColor("lightcoral");
    }
    else {
        color = QColor(Qt::red);
    }
    setLineHighlighted(lineNo, color, colStart, colEnd);
}

void Editor::unhighlightLine()
{
    setLineHighlighted(-1, QColor::Invalid, 0u, 0u);
}

void Editor::ensureAnalized()
{
    doc_->forceCompleteRecompilation(QPoint(cursor_->column(), cursor_->row()));
}

bool Editor::isTeacherMode() const
{
    return plugin_->teacherMode_;
}

void Editor::updateSettings(const QStringList & keys)
{
    plane_->updateSettings(keys);
    plane_->update();
}

void Editor::changeGlobalState(quint32 prevv, quint32 currentt)
{
    ExtensionSystem::GlobalState prev = ExtensionSystem::GlobalState(prevv);
    ExtensionSystem::GlobalState current = ExtensionSystem::GlobalState(currentt);

    using Shared::PluginInterface;

    if (current==PluginInterface::GS_Unlocked || current==PluginInterface::GS_Running) {
        unhighlightLine();
    }
    if (prev==PluginInterface::GS_Observe && current!=PluginInterface::GS_Observe) {
        clearMarginText();
    }
    if (current==PluginInterface::GS_Unlocked || current==PluginInterface::GS_Observe) {
        unlock();
    }
    else {
        lock();
    }
}

void Editor::setNotModified()
{
    doc_->undoStack()->setClean();
    notSaved_ = false;
    emit documentCleanChanged(true);
}

bool Editor::isModified() const
{
    return notSaved_ || ! doc_->undoStack()->isClean();
}

void Editor::checkForClean()
{
    emit documentCleanChanged(!isModified());
}

bool Editor::forceNotSavedFlag() const
{
    return notSaved_;
}

void Editor::setForceNotSavedFlag(bool v)
{
    notSaved_ = v;
    checkForClean();
}

QDataStream & operator<< (QDataStream & stream, const Editor & editor)
{
    stream << KumFile::toString(editor.documentContents());
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


