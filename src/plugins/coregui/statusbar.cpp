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
    , messageWidth_(0)
    , state_(Shared::PluginInterface::GS_Unlocked)
    , errorsCount_(0u)
    , stepsDone_(0u)
    , messageRole_(Normal)
    , editorRow_(0u)
    , editorColumn_(0u)
    , keyboardLayout_(QLocale::C)
    , keyboardCaps_(false)
    , keyboardShift_(false)
    , keyboardAlt_(false)
    , editorRecord_(false)
{
}

void StatusBar::addButtonToLeft(QToolButton *btn)
{
    btn->setParent(this);
    btn->setAutoRaise(true);
    const QSize iconSize = btn->iconSize();
    const int w = btn->menu()
            ? iconSize.width() + 4 + 10
            : iconSize.width() + 4 ;
    const int h = iconSize.height() + 4;
    btn->setFixedSize(w, h);
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
//#ifndef Q_WS_X11
    h += 8;
//#endif
    w = 100;
    return QSize(w, h);
}

void StatusBar::setState(ExtensionSystem::GlobalState state)
{
    bool upd = state_ != state;
    state_ = state;
    if (upd) update();
}

void StatusBar::setMessage(const QString &message, MessageRole role)
{
    bool upd = message_ != message || messageRole_ != role;
    message_ = message;
    messageWidth_ = statusBarFontMetrics().width(message);
    messageRole_ = role;
    if (upd) update();
}

void StatusBar::unsetMessage()
{
    bool upd = message_.length() > 0;
    message_.clear();
    messageWidth_ = 0;
    messageRole_ = Normal;
    if (upd) update();
}

void StatusBar::setRecordIndicator(bool on)
{
    bool upd = editorRecord_ != on;
    editorRecord_ = on;
    if (upd) update();
}

void StatusBar::setStepsDoneCounter(uint value)
{
    bool upd = stepsDone_ != value;
    stepsDone_ = value;
    if (upd) repaint();
}

void StatusBar::setErrorsCounter(uint value)
{
    bool upd = errorsCount_ != value;
    errorsCount_ = value;
    if (upd) repaint();
}

void StatusBar::setEditorCursorPosition(uint row, uint column)
{
    bool upd = editorRow_ != row || editorColumn_ != column;
    editorRow_ = row;
    editorColumn_ = column;
    if (upd) update();
}

void StatusBar::setEditorKeyboardLayout(QLocale::Language lang, bool capslock, bool shift, bool alt)
{
    bool upd = keyboardLayout_ != lang || keyboardCaps_ != capslock  || keyboardShift_ != shift || keyboardAlt_ != alt;
    keyboardLayout_ = lang;
    keyboardCaps_ = capslock;
    keyboardShift_ = shift;
    keyboardAlt_ = alt;
    if (upd) update();
}

QSize StatusBar::keyboardLayoutItemSize() const
{
    static const QStringList items = QStringList()
            << tr("rus") << tr("rus").toUpper()
            << tr("lat") << tr("lat").toUpper();
    static int maxTextWidth = 0;
    const int textHeight = fontHeight();
    if (0 == maxTextWidth) {
        foreach (const QString &text, items) {
            maxTextWidth = qMax(maxTextWidth, statusBarFontMetrics().width(text));
        }
    }
    const int width = 2*(12 + 1) + maxTextWidth;
    const int height = qMax(14, textHeight);
    return QSize(width + 2*ItemPadding, height);
}

QSize StatusBar::cursorPositionItemSize() const
{
    const QString text = tr("Row: ww, Col.: ww");
    const int textW = statusBarFontMetrics().width(text);
    return QSize(textW + 2 * ItemPadding,
                 qMax(14, fontHeight()));
}

QSize StatusBar::modeItemSize() const
{
    static QStringList items = QStringList()
            << tr("Edit") << tr("Analisys") << tr("Run") << tr("Pause");
    int maxTextWidth = 0;
    const int textHeight = fontHeight();
    if (0 == maxTextWidth) {
        foreach (const QString &text, items) {
            maxTextWidth = qMax(maxTextWidth, statusBarFontMetrics().width(text));
        }
    }
    const int height = qMax(14, textHeight);
    return QSize(20 + maxTextWidth + 2*ItemPadding, height);
}

QSize StatusBar::counterItemSize() const
{
    using Shared::PluginInterface;
    static const QString errorsText = tr("ww errors");
    static const QString noErrorsText = tr("No errors");
    static const QString stepsDoneText = tr("wwwww steps done") + "wwwwww";
    const int textHeight = fontHeight();

    int maxTextWidthEdit = 0;
    if (0 == maxTextWidthEdit) {
        maxTextWidthEdit = qMax(maxTextWidthEdit, statusBarFontMetrics().width(errorsText));
        maxTextWidthEdit = qMax(maxTextWidthEdit, statusBarFontMetrics().width(noErrorsText));
    }

    const int textWidthOther = statusBarFontMetrics().width(stepsDoneText);

    const int textWidth = state_ == PluginInterface::GS_Unlocked
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
    const int textHeight = fontHeight();
    const int height = qMax(14, textHeight);
    return QSize(messageWidth_ + 2*ItemPadding, height);
}

QFont StatusBar::statusBarFont() const
{
    QFont fnt = font();
#ifndef Q_OS_LINUX
    int MinPointSize = 12;
    int SystemPointSize = fnt.pointSize();
    fnt.setPointSize(qMax(MinPontSize, SystemPointSize));
#endif
    return fnt;
}

QFontMetrics StatusBar::statusBarFontMetrics() const
{
    QFontMetrics fm(statusBarFont());
    return fm;
}

int StatusBar::fontHeight() const
{
    /*
     * Calling QFontMetrics::height() on Win32 causes to memory leaks,
     * so it is required to cache it once
     */

    int result = 0;
    if (0 == result) {
        result = statusBarFontMetrics().height();
    }
    return result;
}

void StatusBar::paintItemRect(QPainter &p, const QSize &sz, int x)
{
//    p.save();
//    QStyleOptionFrameV3 opt;
//    opt.rect = QRect(x, 0, sz.width(), sz.height());
//    opt.palette = palette();
//    opt.state = QStyle::State_None;
//    opt.frameShape = QFrame::NoFrame;
//    style()->drawPrimitive(QStyle::PE_FrameStatusBarItem, &opt,  &p, this);
//    p.restore();
}

static const int HideToolbarThereshold = 400;

void StatusBar::paintEvent(QPaintEvent *event)
{
    static const int HideKeyboardStatusThereshold = 520;
    static const int HideLineNumberThereshold = 460;
    static const int HideModeThereshold = 200;

    using Shared::PluginInterface;
    QPainter p(this);
    QStyle * st = style();
    QStyleOption opt;
    opt.initFrom(this);
    st->drawPrimitive(QStyle::PE_PanelStatusBar, &opt, &p, this);
    const QPen pen(palette().brush(QPalette::WindowText), 1);
    p.setFont(statusBarFont());
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    int x = 0;
    if (width() >= HideToolbarThereshold) {
        for (int i=0; i<toolButtons_.size(); i++) {
            x += toolButtons_[i]->width();
        }
        if (toolButtons_.size() > 0) {
            x += ItemPadding;
        }
    }
#ifdef Q_OS_MAC
    x += 20;
#endif
    if (width() >= HideModeThereshold) {
        paintModeItem(p, x);
        x += modeItemSize().width();
    }
    paintCounterItem(p, x);
    x += counterItemSize().width();

    if (state_ == PluginInterface::GS_Unlocked) {
        const QSize remaining =
                cursorPositionItemSize() + keyboardLayoutItemSize();
        const QSize message = messageItemSize();
        if (x + message.width() <= width() - remaining.width()) {
            paintMessageItem(p, x);
            x += message.width();
        }
        if (width() < HideKeyboardStatusThereshold) {
            x = width() - cursorPositionItemSize().width();
        }
        else {
            x = width() - cursorPositionItemSize().width() - keyboardLayoutItemSize().width();
        }
        if (width() >= HideLineNumberThereshold) {
            paintCursorItem(p, x);
        }
        if (width() >= HideKeyboardStatusThereshold) {
            x += cursorPositionItemSize().width();
            paintKeyboardItem(p, x);
        }
    }
    else {
        paintMessageItem(p, x);
        x += messageItemSize().width();
        const QSize remaining =
                cursorPositionItemSize() + keyboardLayoutItemSize();
        if (x <= width() - remaining.width()) {
            if (width() < HideKeyboardStatusThereshold) {
                x = width() - cursorPositionItemSize().width();
            }
            else {
                x = width() - cursorPositionItemSize().width() - keyboardLayoutItemSize().width();
            }
            if (width() >= HideLineNumberThereshold) {
                paintCursorItem(p, x);
            }
            if (width() >= HideKeyboardStatusThereshold) {
                x += cursorPositionItemSize().width();
                paintKeyboardItem(p, x);
            }
        }
    }
    p.end();
    event->accept();
}

void StatusBar::resizeEvent(QResizeEvent *event)
{
    Q_FOREACH(QToolButton *b, toolButtons_) {
        b->setVisible(width() >= HideToolbarThereshold);
    }

    QStatusBar::resizeEvent(event);
}

void StatusBar::paintModeItem(QPainter &p, int x)
{
    paintItemRect(p, modeItemSize(), x);
    using Shared::PluginInterface;
    p.save();
    uint xoffset = 0;
    QString modeText;
    if (state_ == PluginInterface::GS_Input || state_ == PluginInterface::GS_Pause) {
        modeText = tr("Pause");
    }
    else if (state_ == PluginInterface::GS_Observe) {
        modeText = tr("Analisys");
    }
    else if (state_ == PluginInterface::GS_Running) {
        modeText = tr("Running");
    }
    else {
        xoffset = 10;
        modeText = tr("Edit");
    }
    const QRect textRect(QPoint(x + ItemPadding + xoffset, (height() - fontHeight()) / 2),
                         modeItemSize() - QSize(2*ItemPadding, 0));
    QTextOption opt;
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    p.drawText(textRect, modeText, opt);
    if (state_ == PluginInterface::GS_Unlocked && editorRecord_) {
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(QPen(palette().brush(QPalette::WindowText).color()));
        p.setBrush(alternateColor());
        QRect ellipseRect(x + ItemPadding + 4, ItemPadding + 4,
                           textRect.height() - 8,
                           textRect.height() - 8
                           );
        p.drawEllipse(ellipseRect);
    }
    p.restore();
}

void StatusBar::paintCounterItem(QPainter &p, int x)
{
    paintItemRect(p, counterItemSize(), x);
    using Shared::PluginInterface;
    p.save();
    QString text;
    if (state_ == PluginInterface::GS_Unlocked) {
        p.setPen(QPen(errorsCount_==0? normalColor() : alternateColor()));
        if (errorsCount_ == 0)
            text = tr("No errors");
        else if (errorsCount_ == 1)
            text = tr("1 error");
        else if (errorsCount_ >= 10 && errorsCount_ <= 20)
            text = tr("%1 errors", "10 <= x <= 20").arg(errorsCount_);
        else if (errorsCount_ % 10 == 1)
            text = tr("%1 errors", "1, 21, 31, etc.").arg(errorsCount_);
        else if (errorsCount_ % 10 == 2 || errorsCount_ % 10 == 3 || errorsCount_ % 10 == 4)
            text = tr("%1 errors", "2, 3, 4, 22, 23, 24,  etc.").arg(errorsCount_);
        else
            text = tr("%1 errors", "5, 6, 15, 16, etc").arg(errorsCount_);
    }
    else {
        if (stepsDone_ == 0)
            text = tr("0 steps done");
        else if (stepsDone_ == 1)
            text = tr("1 step done");
        else if (stepsDone_ >= 10 && stepsDone_ <= 20)
            text = tr("%1 steps done", "10 <= x <= 20").arg(stepsDone_);
        else if (stepsDone_ % 10 == 1)
            text = tr("%1 steps done", "1, 21, 31, etc.").arg(stepsDone_);
        else if (stepsDone_ % 10 == 2 || stepsDone_ % 10 == 3 || stepsDone_ % 10 == 4)
            text = tr("%1 steps done", "2, 3, 4, 22, 23, 24,  etc.").arg(stepsDone_);
        else
            text = tr("%1 steps done", "5, 6, 15, 16, etc").arg(stepsDone_);
    }
    const QRect textRect(QPoint(x + ItemPadding, (height() - fontHeight()) / 2),
                         counterItemSize()/* - QSize(2*ItemPadding, 0)*/);
    QTextOption opt;
    opt.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    p.drawText(textRect, text, opt);
    p.restore();
}

void StatusBar::paintMessageItem(QPainter &p, int x)
{
    paintItemRect(p, messageItemSize(), x);
    p.save();
    p.setPen(QPen(QColor(messageRole_==Normal? normalColor() : alternateColor())));
    QRect textRect(QPoint(x + ItemPadding, (height() - fontHeight()) / 2),
                         messageItemSize() - QSize(2*ItemPadding, 0));
    if (textRect.right() > this->width() - ItemPadding) {
        textRect.setRight(this->width() - ItemPadding);
    }
    int width = statusBarFontMetrics().width(message_);

    QString text = message_;

    if (width > textRect.width()) {
        static const QString threeDots = QString("...");
        const int dotsWidth = statusBarFontMetrics().width(threeDots);
        if (dotsWidth >= textRect.width()) {
            text = "";
        }
        else {
            width = statusBarFontMetrics().width(text) + dotsWidth;
            while (text.length() > 0 &&  width > textRect.width()) {
                text.remove(text.length()-1, 1);
                width = statusBarFontMetrics().width(text) + dotsWidth;
            }
            if (!text.isEmpty()) {
                text += threeDots;
            }
        }
    }

    QTextOption opt;
    Qt::Alignment horiz = Qt::AlignHCenter;
    opt.setAlignment(Qt::AlignVCenter | horiz);
    p.drawText(textRect, text, opt);
    p.restore();
}

QColor StatusBar::normalColor() const
{
    return palette().brush(QPalette::WindowText).color();
}

QColor StatusBar::alternateColor() const
{
    const QColor bg = palette().brush(QPalette::Window).color();
    const int value = bg.red() + bg.green() + bg.blue();
    const int Thereshold = 128 * 3;
    if (value < Thereshold) {
        // Assuming dark background palette
        return QColor("orangered");
    }
    else {
        return QColor(Qt::red);
    }
}

void StatusBar::paintCursorItem(QPainter &p, int x)
{
    paintItemRect(p, cursorPositionItemSize(), x);
    p.save();
    const QRect textRect(QPoint(x + ItemPadding, (height() - fontHeight()) / 2),
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
    p.drawImage(x + ItemPadding, (height() - 12) / 2, shiftImage);
    p.drawImage(x + 12 + ItemPadding, (height() - 12) / 2, altImage);
    const QRect textRect(QPoint(x + 25 + ItemPadding, (height() - fontHeight()) / 2),
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
    bool upperCase =
            (keyboardCaps_ && !keyboardShift_) ||
            (!keyboardCaps_ && keyboardShift_);
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
