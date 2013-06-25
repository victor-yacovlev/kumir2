#include "statusbar.h"
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QStyle>
#include <QStyleOption>
#include <QStyleOptionFrameV3>

namespace CoreGUI {

#ifdef Q_OS_MAX
static const int ItemPadding = 16;
#else
static const int ItemPadding = 8;
#endif


StatusBar::StatusBar(QWidget *parent)
    : QStatusBar(parent)
    , state_(ExtensionSystem::GS_Unlocked)
    , errorsCount_(0u)
    , stepsDone_(0u)
    , messageRole_(Normal)
    , editorRow_(0u)
    , editorColumn_(0u)
    , keyboardLayout_(QLocale::C)
    , keyboardCaps_(false)
    , keyboardShift_(false)
    , keyboardAlt_(false)
{
}

void StatusBar::addButtonToLeft(QToolButton *btn)
{
    btn->setParent(this);
    btn->setAutoRaise(true);
    btn->setFixedSize(btn->iconSize() + QSize(8, 8));
    addWidget(btn);
    int x = 0;
    for (int i=0; i<toolButtons_.size(); i++) {
        x += toolButtons_[i]->width();
    }
    btn->move(x, 0);
    toolButtons_ << btn;
}

QSize StatusBar::minimumSizeHint() const
{
    int h = 0;
    int w = 0;
    for (int i=0; i<toolButtons_.size(); i++) {
        w += toolButtons_[i]->width();
        h = qMax(h, toolButtons_[i]->height());
    }
    if (toolButtons_.size() > 0) {
        w += ItemPadding;
    }
    h = qMax(h, modeItemSize().height());
    w += modeItemSize().width();
    h = qMax(h, counterItemSize().height());
    w += counterItemSize().width();
    h = qMax(h, cursorPositionItemSize().height());
    w += cursorPositionItemSize().width();
    h = qMax(h, keyboardLayoutItemSize().height());
    w += keyboardLayoutItemSize().width();
#ifdef Q_OS_MAC
    h += 8;
#endif
    return QSize(w, h);
}

void StatusBar::setState(ExtensionSystem::GlobalState state)
{
    state_ = state;
    update();
}

void StatusBar::setMessage(const QString &message, MessageRole role)
{
    message_ = message;
    messageRole_ = role;
    update();
}

void StatusBar::unsetMessage()
{
    message_.clear();
    messageRole_ = Normal;
    update();
}

void StatusBar::setStepsDoneCounter(uint value)
{
    stepsDone_ = value;
    update();
}

void StatusBar::setErrorsCounter(uint value)
{
    errorsCount_ = value;
    update();
}

void StatusBar::setEditorCursorPosition(uint row, uint column)
{
    editorRow_ = row;
    editorColumn_ = column;
    update();
}

void StatusBar::setEditorKeyboardLayout(QLocale::Language lang, bool capslock, bool shift, bool alt)
{
    keyboardLayout_ = lang;
    keyboardCaps_ = capslock;
    keyboardShift_ = shift;
    keyboardAlt_ = alt;
    update();
}

QSize StatusBar::keyboardLayoutItemSize() const
{
    static const QStringList items = QStringList()
            << tr("rus") << tr("rus").toUpper()
            << tr("lat") << tr("lat").toUpper();
    int maxTextWidth = 0;
    const int textHeight = statusBarFontMetrics().height();
    foreach (const QString &text, items) {
        maxTextWidth = qMax(maxTextWidth, statusBarFontMetrics().width(text));
    }
    const int width = 2*(12 + 1) + maxTextWidth;
    const int height = qMax(14, textHeight);
    return QSize(width + 2*ItemPadding, height);
}

QSize StatusBar::cursorPositionItemSize() const
{
    static const QString text = tr("Row: ww, Col.: ww");
    return QSize(statusBarFontMetrics().width(text) + 2 * ItemPadding,
                 qMax(14, statusBarFontMetrics().height()));
}

QSize StatusBar::modeItemSize() const
{
    static QStringList items = QStringList()
            << tr("Edit") << tr("Analisys") << tr("Run") << tr("Pause");
    int maxTextWidth = 0;
    const int textHeight = statusBarFontMetrics().height();
    foreach (const QString &text, items) {
        maxTextWidth = qMax(maxTextWidth, statusBarFontMetrics().width(text));
    }
    const int height = qMax(14, textHeight);
    return QSize(maxTextWidth + 2*ItemPadding, height);
}

QSize StatusBar::counterItemSize() const
{
    static const QString errorsText = tr("ww errors");
    static const QString noErrorsText = tr("No errors");
    static const QString stepsDoneText = tr("wwwww steps done");
    const int textHeight = statusBarFontMetrics().height();

    int maxTextWidthEdit = 0;
    maxTextWidthEdit = qMax(maxTextWidthEdit, statusBarFontMetrics().width(errorsText));
    maxTextWidthEdit = qMax(maxTextWidthEdit, statusBarFontMetrics().width(noErrorsText));

    int textWidthOther = statusBarFontMetrics().width(stepsDoneText);

    const int textWidth = state_ == ExtensionSystem::GS_Unlocked
            ? maxTextWidthEdit : textWidthOther;

    const int height = qMax(14, textHeight);
#ifdef Q_OS_MAC
    return QSize(textWidth + 2*ItemPadding + 20, height);
#else
    return QSize(textWidth + 2*ItemPadding, height);
#endif
}

QSize StatusBar::messageItemSize() const
{
    const int textHeight = statusBarFontMetrics().height();
    const int messageWidth = statusBarFontMetrics().width(message_);
    const int height = qMax(14, textHeight);
    return QSize(messageWidth + 2*ItemPadding, height);
}

QFont StatusBar::statusBarFont() const
{
    QFont fnt = font();
    fnt.setPointSize(12);
    return fnt;
}

QFontMetrics StatusBar::statusBarFontMetrics() const
{
    QFontMetrics fm(statusBarFont());
    return fm;
}

void StatusBar::paintItemRect(QPainter &p, const QSize &sz, int x)
{
    p.save();
    QStyleOptionFrameV3 opt;
    opt.rect = QRect(x, 0, sz.width(), sz.height());
    opt.palette = palette();
    opt.state = QStyle::State_None;
    opt.frameShape = QFrame::NoFrame;
    style()->drawPrimitive(QStyle::PE_FrameStatusBarItem, &opt,  &p, this);
    p.restore();
}

void StatusBar::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QStyle * st = style();
    QStyleOption opt;
    opt.initFrom(this);
    st->drawPrimitive(QStyle::PE_PanelStatusBar, &opt, &p, this);
    const QPen pen(palette().brush(QPalette::Text), 1);
    p.setFont(statusBarFont());
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    int x = 0;
    for (int i=0; i<toolButtons_.size(); i++) {
        x += toolButtons_[i]->width();
    }
    if (toolButtons_.size() > 0) {
        x += ItemPadding;
    }
#ifdef Q_OS_MAC
    x += 20;
#endif
    paintModeItem(p, x);
    x += modeItemSize().width();
    paintCounterItem(p, x);
    x += counterItemSize().width();
    if (state_ == ExtensionSystem::GS_Unlocked) {
        const QSize remaining =
                cursorPositionItemSize() + keyboardLayoutItemSize();
        const QSize message = messageItemSize();
        if (x + message.width() <= width() - remaining.width()) {
            paintMessageItem(p, x);
            x += message.width();
        }
        x = width() - cursorPositionItemSize().width() - keyboardLayoutItemSize().width();
        paintCursorItem(p, x);
        x += cursorPositionItemSize().width();
        paintKeyboardItem(p, x);
    }
    else {
        paintMessageItem(p, x);
        x += messageItemSize().width();
        const QSize remaining =
                cursorPositionItemSize() + keyboardLayoutItemSize();
        if (x <= width() - remaining.width()) {
            x = width() - cursorPositionItemSize().width() - keyboardLayoutItemSize().width();
            paintCursorItem(p, x);
            x += cursorPositionItemSize().width();
            paintKeyboardItem(p, x);
        }
    }
    p.end();
    event->accept();
}

void StatusBar::paintModeItem(QPainter &p, int x)
{
    paintItemRect(p, modeItemSize(), x);
    using namespace ExtensionSystem;
    p.save();
    QString modeText;
    if (state_ == GS_Input || state_ == GS_Pause) {
        modeText = tr("Pause");
    }
    else if (state_ == GS_Observe) {
        modeText = tr("Analisys");
    }
    else if (state_ == GS_Running) {
        modeText = tr("Running");
    }
    else {
        modeText = tr("Edit");
    }
    const QRect textRect(QPoint(x + ItemPadding, (height() - statusBarFontMetrics().height()) / 2 + 2),
                         modeItemSize() - QSize(2*ItemPadding, 0));
    QTextOption opt;
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    p.drawText(textRect, modeText, opt);
    p.restore();
}

void StatusBar::paintCounterItem(QPainter &p, int x)
{
    paintItemRect(p, counterItemSize(), x);
    using namespace ExtensionSystem;
    p.save();
    QString text;
    if (state_ == GS_Unlocked) {
        p.setPen(QPen(errorsCount_==0? normalColor() : errorColor()));
        if (errorsCount_ == 0)
            text = tr("No errors");
        else if (errorsCount_ == 1)
            text = tr("1 error", NULL, 1);
        else if (errorsCount_ >= 10 && errorsCount_ <= 20)
            text = tr("%1 errors", NULL, 25).arg(errorsCount_);
        else if (errorsCount_ % 10 == 1)
            text = tr("%1 errors", NULL, 21).arg(errorsCount_);
        else if (errorsCount_ % 10 == 2 || errorsCount_ % 10 == 3 || errorsCount_ % 10 == 4)
            text = tr("%1 errors", NULL, 22).arg(errorsCount_);
        else
            text = tr("%1 errors", NULL, 25).arg(errorsCount_);
    }
    else {
        if (stepsDone_ == 0)
            text = tr("0 steps done");
        else if (stepsDone_ == 1)
            text = tr("1 step done", NULL, 1);
        else if (stepsDone_ >= 10 && stepsDone_ <= 20)
            text = tr("%1 steps done", NULL, 25).arg(stepsDone_);
        else if (stepsDone_ % 10 == 1)
            text = tr("%1 steps done", NULL, 21).arg(stepsDone_);
        else if (stepsDone_ % 10 == 2 || stepsDone_ % 10 == 3 || stepsDone_ % 10 == 4)
            text = tr("%1 steps done", NULL, 22).arg(stepsDone_);
        else
            text = tr("%1 steps done", NULL, 25).arg(stepsDone_);
    }
    const QRect textRect(QPoint(x + ItemPadding, (height() - statusBarFontMetrics().height()) / 2 + 2),
                         counterItemSize() - QSize(2*ItemPadding, 0));
    QTextOption opt;
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    p.drawText(textRect, text, opt);
    p.restore();
}

void StatusBar::paintMessageItem(QPainter &p, int x)
{
    paintItemRect(p, messageItemSize(), x);
    p.save();
    p.setPen(QPen(QColor(messageRole_==Normal? normalColor() : errorColor())));
    const QRect textRect(QPoint(x + ItemPadding, (height() - statusBarFontMetrics().height()) / 2 + 2),
                         messageItemSize() - QSize(2*ItemPadding, 0));
    QTextOption opt;
    Qt::Alignment horiz = statusBarFontMetrics().width(message_) < textRect.width()
            ? Qt::AlignHCenter : Qt::AlignLeft;
    opt.setAlignment(Qt::AlignVCenter | horiz);
    p.drawText(textRect, message_, opt);
    p.restore();
}

QColor StatusBar::normalColor() const
{
    return palette().brush(QPalette::Text).color();
}

QColor StatusBar::errorColor() const
{
    const QColor bg = palette().brush(QPalette::Base).color();
    const int value = bg.red() + bg.green() + bg.blue();
    const int Thereshold = 128 * 3;
    if (value < Thereshold) {
        // Assuming dark background palette
        return QColor(Qt::yellow);
    }
    else {
        return QColor(Qt::red);
    }
}

void StatusBar::paintCursorItem(QPainter &p, int x)
{
    paintItemRect(p, cursorPositionItemSize(), x);
    p.save();
    const QRect textRect(QPoint(x + ItemPadding, (height() - statusBarFontMetrics().height()) / 2 + 2),
                         cursorPositionItemSize() - QSize(2*ItemPadding, 0));
    QTextOption opt;
    const QString text = tr("Row: %1, Column: %2").arg(editorRow_ + 1).arg(editorColumn_ + 1);
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    p.drawText(textRect, text, opt);
    p.restore();
}

void StatusBar::paintKeyboardItem(QPainter &p, int x)
{   
    paintItemRect(p, keyboardLayoutItemSize(), x);
    bool active = parentWidget() && parentWidget()->isActiveWindow();
    const QColor activeColor = palette().brush(QPalette::Active, QPalette::Text).color();
    const QColor disabledColor = palette().brush(QPalette::Disabled, QPalette::Text).color();
    QColor altColor, shiftColor;
    if (active) {
        altColor = keyboardAlt_? activeColor : disabledColor;
        shiftColor = keyboardShift_ ? activeColor : disabledColor;
    }
    else {
        altColor = shiftColor = disabledColor;
    }
    QImage shiftImage = makeIndicatorIcon("shift", shiftColor);
    QImage altImage = makeIndicatorIcon("alt", altColor);
    p.save();
    p.drawImage(x + ItemPadding, (height() - 12) / 2 + 2, shiftImage);
    p.drawImage(x + 12 + ItemPadding, (height() - 12) / 2 + 2, altImage);
    const QRect textRect(QPoint(x + 25 + ItemPadding, (height() - statusBarFontMetrics().height()) / 2 + 2),
                         keyboardLayoutItemSize() - QSize(25 + 2*ItemPadding, 0));
    QTextOption opt;
    QString text;
    QLocale::Language lang = keyboardLayout_;
    if (keyboardAlt_) {
        if (lang == QLocale::Russian)
            text = tr("lat");
        else
            text = tr("rus");
    }
    else {
        if (lang == QLocale::Russian)
            text = tr("rus");
        else
            text = tr("lat");
    }
    bool upperCase = (keyboardCaps_ && !keyboardShift_) || keyboardShift_;
    if (upperCase)
        text = text.toUpper();
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    p.drawText(textRect, text, opt);
    p.restore();
}

QImage StatusBar::makeIndicatorIcon(const QString & name, const QColor &color)
{
    QImage img(QString(":/coregui/statusbar_%1_indicator.png").arg(name));
    img.setColor(0, color.rgba());
    img.setColor(1, QColor(Qt::transparent).rgba());
//    QImage result = QImage(img.size(), QImage::Format_ARGB32);
//    result.fill(0);
//    QRgb col = color.rgba();
//    for (int y = 0; y<height; y++) {
//        for (int x = 0; x<width; x++) {
//            if (img.pixel(x, y))
//                result.setPixel(x, y, col);
//        }
//    }
    return img;
}


} // namespace CoreGUI
