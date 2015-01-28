#include "suggestionswindow.h"
#include "ui_suggestionswindow.h"
#include "settingspage.h"

namespace Editor {

void desaturate(QImage & img) {
    for (int y=0; y<img.height(); y++) {
        for (int x=0; x<img.width(); x++) {
            const QColor source = QColor(img.pixel(x,y));
            QColor target;
            target.setHsv(source.hsvHue(), source.hsvSaturation()/2, source.value()/4*3);
            target.setAlpha(qAlpha(img.pixel(x,y)));
            img.setPixel(x,y,target.rgba());
        }
    }
}

void SuggestionItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
    if (index.isValid()) {
        const QStandardItemModel * model =
                qobject_cast<const QStandardItemModel*>(index.model());
        SuggestionItem * item =
                static_cast<SuggestionItem*>(model->itemFromIndex(index));
        if (item && item->hasHelpEntry()) {
            const QPalette & pal = option.palette;
            const QColor & color = option.state == 98561
                    ? pal.color(QPalette::Active, QPalette::HighlightedText)
                    : pal.color(QPalette::Disabled, QPalette::Text);
            painter->save();
            const QRect rect = option.rect;
            const QSize buttonSize =
                    option.decorationSize - QSize(2, 4);
            const QRect buttonRect = QRect(
                        rect.right() - 4 - buttonSize.width(),
                        rect.top() + 2,
                        buttonSize.width() + 2,
                        buttonSize.height()
                        );
            painter->setPen(QPen(color));
            painter->drawRect(buttonRect);
            QFont fnt = QApplication::font();
            fnt.setPixelSize(buttonSize.height() - 2);
            QTextOption textOpt;
            textOpt.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            painter->setFont(fnt);
            painter->drawText(buttonRect, "F1", textOpt);
            painter->restore();
        }
    }
}

SuggestionsWindow::SuggestionsWindow(QWidget *editorWidget)
    : QWidget(0, Qt::Popup)
    , ui(new Ui::SuggestionsWindow)
    , itemModel_(new QStandardItemModel)
    , keyPressedFlag_(false)
    , editorWidget_(editorWidget)
    , editorPlugin_(0)
{
    ui->setupUi(this);
    ui->alist->setModel(itemModel_);
    ui->alist->setItemDelegate(new SuggestionItemDelegate);
    ui->alist->installEventFilter(this);
    connect(ui->alist, SIGNAL(activated(QModelIndex)),
            this, SLOT(handleItemActivated(QModelIndex)));

    setCursor(Qt::ArrowCursor);
    ui->alist->setCursor(Qt::PointingHandCursor);
}


void SuggestionsWindow::updateSettings(const ExtensionSystem::SettingsPtr settings)
{
    createIcons(settings);
//    QFont f;
//    f.setFamily(settings->value(
//                    SettingsPage::KeyFontName,
//                    SettingsPage::defaultFontFamily()
//                    ).toString());
//    f.setPointSize(settings->value(
//                       SettingsPage::KeyFontSize,
//                       SettingsPage::defaultFontSize
//                       ).toInt());
//    ui->alist->setFont(f);
    ui->alist->setFont(editorPlugin_->defaultEditorFont());
}

void SuggestionsWindow::createIcons(const ExtensionSystem::SettingsPtr settings)
{
    QPoint points[4] = { QPoint(4,0), QPoint(0,4), QPoint(4,8), QPoint(8,4) };
    QImage local(8,8,QImage::Format_ARGB32);
    local.fill(0);
    QPainter pl(&local);
    pl.setRenderHint(QPainter::Antialiasing, true);
    pl.setPen(QPen(QColor("black")));
    pl.setBrush(QColor(settings->value(
                           SettingsPage::KeyColorType,
                           SettingsPage::DefaultColorType
                           ).toString()));
    pl.drawEllipse(QRect(0,0,8,8));

    icon_local_ = QIcon(QPixmap::fromImage(local));

    QImage global(8,8,QImage::Format_ARGB32);
    global.fill(0);
    QPainter pg(&global);
    pg.setRenderHint(QPainter::Antialiasing, true);
    pg.setPen(QPen(QColor("black")));
    pg.setBrush(QColor(settings->value(
                           SettingsPage::KeyColorType,
                           SettingsPage::DefaultColorType
                           ).toString()));
    pg.drawPolygon(points, 4);

    icon_global_ = QIcon(QPixmap::fromImage(global));

    QImage algorithm(8,8,QImage::Format_ARGB32);
    algorithm.fill(0);
    QPainter pa(&algorithm);
    pa.setRenderHint(QPainter::Antialiasing, true);
    pa.setPen(QPen(QColor("black")));
    pa.setBrush(QColor(settings->value(
                           SettingsPage::KeyColorAlg,
                           SettingsPage::DefaultColorAlg
                           ).toString()));
    pa.drawPolygon(points, 4);

    icon_algorithm_ = QIcon(QPixmap::fromImage(algorithm));

    QImage module(8,8,QImage::Format_ARGB32);
    module.fill(0);
    QPainter pm(&module);
    pm.setRenderHint(QPainter::Antialiasing, true);
    pm.setPen(QPen(QColor("black")));
    pm.setBrush(QColor(settings->value(
                           SettingsPage::KeyColorMod,
                           SettingsPage::DefaultColorMod
                           ).toString()));
    pm.drawRect(QRect(1,1,6,6));

    icon_module_ = QIcon(QPixmap::fromImage(module));

    QImage kumfile(8,8,QImage::Format_ARGB32);
    kumfile.fill(0);
    QPainter pf(&kumfile);
    pf.setRenderHint(QPainter::Antialiasing, true);
    pf.setPen(QPen(QColor("black")));
    pf.setBrush(QColor(settings->value(
                           SettingsPage::KeyColorLiteral,
                           SettingsPage::DefaultColorLiteral
                           ).toString()));
    pf.drawRect(QRect(1,1,6,6));

    icon_kumfile_ = QIcon(QPixmap::fromImage(kumfile));

    QImage keyword(8,8,QImage::Format_ARGB32);
    keyword.fill(0);
    QPainter pk(&keyword);
    pk.setRenderHint(QPainter::Antialiasing, true);
    pk.setPen(QPen(QColor("black")));
    pk.setBrush(QColor(settings->value(
                           SettingsPage::KeyColorKw,
                           SettingsPage::DefaultColorKw
                           ).toString()));
    pk.drawRect(QRect(1,1,6,6));

    icon_keyword_ = QIcon(QPixmap::fromImage(keyword));

    QImage other(8,8,QImage::Format_ARGB32);
    other.fill(0);
    icon_other_ = QIcon(QPixmap::fromImage(other));

}

void SuggestionsWindow::handleItemActivated(const QModelIndex & index)
{
    acceptItem();
}

void SuggestionsWindow::acceptItem()
{
    const QModelIndex & index = ui->alist->currentIndex();
    if (index.isValid()) {
        const QString text = itemModel_->data(index, Qt::DisplayRole).toString();
        emit acceptedSuggestion(text);
    }
    hide();
}

SuggestionItem::SuggestionItem(const Shared::Analizer::Suggestion &suggestion,
                               SuggestionsWindow *factory,
                               Shared::EditorInterface * editorPlugin,
                               DocBookViewer::DocBookView * helpViewer
                               )
    : QStandardItem()    
{
    setText(suggestion.value);
    setToolTip(suggestion.description);
    if (suggestion.kind==Shared::Analizer::Suggestion::LocalVariable) {
        setIcon(factory->icon_local_);
    }
    else if (suggestion.kind==Shared::Analizer::Suggestion::GlobalVariable) {
        setIcon(factory->icon_global_);
    }
    else if (suggestion.kind==Shared::Analizer::Suggestion::Function) {
        setIcon(factory->icon_algorithm_);
    }
    else if (suggestion.kind==Shared::Analizer::Suggestion::BuiltinModule) {
        setIcon(factory->icon_module_);
    }
    else if (suggestion.kind==Shared::Analizer::Suggestion::Module) {
        setIcon(factory->icon_kumfile_);
    }
    else if (suggestion.kind==Shared::Analizer::Suggestion::SecondaryKeyword) {
        setIcon(factory->icon_keyword_);
    }
    else {
        setIcon(factory->icon_other_);
    }
    hasHelpEntry_ = helpViewer && helpViewer->hasAlgorithm(suggestion.value.trimmed());

}


void SuggestionsWindow::init(
        const QString &,
        const QList<Shared::Analizer::Suggestion> &suggestions,
        Shared::EditorInterface * editorPlugin,
        DocBookViewer::DocBookView * helpViewer
        )
{
    editorPlugin_ = editorPlugin;
    keyPressedFlag_ = false;
    itemModel_->clear();
    int prefWidth = 100;
    const QFontMetrics fm (ui->alist->font());
    int prefHeight = fm.height()*(5+suggestions.size());
    for (int index = 0; index<suggestions.size(); index++) {
        const Shared::Analizer::Suggestion & s = suggestions.at(index);
        SuggestionItem * item = new SuggestionItem(s, this, editorPlugin, helpViewer);
        itemModel_->appendRow(item);
        prefWidth = qMax(prefWidth, 100+fm.width(s.value));
    }
    int width = qMax(qMin(400, prefWidth), 150);
    int height = qMin(prefHeight, 400);
    setFixedWidth(width);
    setFixedHeight(height);
}

void SuggestionsWindow::focusInEvent(QFocusEvent *event)
{
    ui->alist->setFocus(event->reason());
    event->accept();
    if (itemModel_->rowCount() > 0) {
        const QModelIndex first = itemModel_->index(0, 0);
        ui->alist->setCurrentIndex(first);
    }
    QList<QAction*> globalShortcuts = QApplication::activeWindow()->
            findChildren<QAction*>();
    for (int i=0; i<globalShortcuts.size(); i++) {
        if (globalShortcuts[i]->shortcut().toString()=="F1") {
            globalShortcuts[i]->setEnabled(false);
        }
    }
}

bool SuggestionsWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj==ui->alist && event->type()==QEvent::KeyPress) {
        QKeyEvent * e = static_cast<QKeyEvent*>(event);
        if (e->key()==Qt::Key_Tab || e->key()==Qt::Key_Escape) {
            hide();
            return true;
        }
        if (e->key()==Qt::Key_F1) {
            if (ui->alist->currentIndex().isValid()) {
                SuggestionItem * item =
                        static_cast<SuggestionItem*>(
                            itemModel_->itemFromIndex(ui->alist->currentIndex())
                            );
                if (item->hasHelpEntry()) {
                    const QString text = item->text().trimmed();
                    qDebug() << "Request help for " << text;
                    emit requestHelpForAlgorithm("", text); // TODO package text
                }
            }
            return true;
        }
    }
    return false;
}

void SuggestionsWindow::keyPressEvent(QKeyEvent *event)
{
    keyPressedFlag_ = true;
    if (event->key()==Qt::Key_Tab || event->key()==Qt::Key_Escape) {
        hide();
        event->accept();
    }
    else if (event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return) {
        hide();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void SuggestionsWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (keyPressedFlag_ || event->key()==Qt::Key_Escape) {
        keyPressedFlag_ = false;
        if (event->key()==Qt::Key_Tab || event->key()==Qt::Key_Escape) {
            hide();
            event->accept();
        }
        else if (event->key()==Qt::Key_Enter || event->key()==Qt::Key_Return) {
            hide();
            event->accept();
        }
    }
    else {
        event->ignore();
    }
}

void SuggestionsWindow::hideEvent(QHideEvent *event)
{
    Q_CHECK_PTR(editorWidget_);
    keyPressedFlag_ = false;
    QWidget::hideEvent(event);
    editorWidget_->setFocus();
    QList<QAction*> globalShortcuts = QApplication::activeWindow()->
            findChildren<QAction*>();
    for (int i=0; i<globalShortcuts.size(); i++) {
        if (globalShortcuts[i]->shortcut().toString()=="F1") {
            globalShortcuts[i]->setEnabled(true);
        }
    }
    emit hidden();
}

SuggestionsWindow::~SuggestionsWindow()
{
    delete ui;
}

} // namespace Editor
