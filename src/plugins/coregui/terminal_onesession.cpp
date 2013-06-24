#include "terminal_onesession.h"
#include "terminal.h"
#include "stdlib/kumirstdlib.hpp"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"


namespace Terminal {

static const uint BodyPadding = 4u;
static const uint HeaderPadding = 4u;
static const uint LineWidth = 1u;
static const uint ShadowOffset = 4u;
static const uint SessionMargin = 4u;
static const uint InputCursorThickness = 2u;

static const unsigned int SelectionMask = 0xFF00;

OneSession::OneSession(int fixedWidth, const QString & fileName, QWidget * parent)
    : QObject(parent)
    , parent_(parent)
    , fileName_(fileName)
    , fixedWidth_(fixedWidth)
    , relayoutMutex_(new QMutex)
{
    inputLineStart_ = inputPosStart_ = -1;
    inputCursorPosition_ = -1;
    timerId_ = -1;
    inputCursorVisible_ = false;
    QString defaultFontFamily = "Courier";
#ifdef Q_WS_X11
    defaultFontFamily = "DejaVu Sans Mono";
#endif
#ifdef Q_OS_WIN32
    if (QSysInfo::WindowsVersion>=0x0080)
        defaultFontFamily = "Consolas";
#endif
#ifdef Q_OS_MAC
    defaultFontFamily = "Courier"; // TODO implement me
#endif
    int defaultFontSize = 12;
    QFont fnt = font();
    fnt.setFamily(defaultFontFamily);
    fnt.setPointSize(defaultFontSize);
    setFont(fnt);
    startTime_ = QDateTime::currentDateTime();
}

QSize OneSession::charSize() const
{

    QFontMetrics fm(font());
    return QSize(fm.width('m'), fm.height());
}

int OneSession::widthInChars(int realWidth) const
{
    QSize atom = charSize();
    return (realWidth-2*BodyPadding-LineWidth-ShadowOffset)/atom.width();
}

QSize OneSession::visibleSize() const
{
    const QRegion region = QRegion() +
            headerRect_ + mainTextRegion_ + footerRect_;
    return region.boundingRect().size();
}

QString OneSession::plainText(bool footer_header) const
{
    const QString body = lines_.join("\n");
    const QString header = headerText();
    const QString footer = footerText();
    if (footer_header)
        return header + "\n" + body + "\n" + footer+"\n";
    else
        return body;

}

QSize OneSession::minimumSizeHint() const
{
    const QFontMetrics headingFM(utilityFont());
    const QFontMetrics mainFM(font());
    int minH = 2 * (headingFM.height() + BodyPadding + HeaderPadding) +
            mainFM.height();
    int minWidthInChars = fixedWidth_ == -1? 10 : fixedWidth_;
    int minW = charSize().width() * minWidthInChars;
    int headerWidth = qMin(400, headingFM.width(visibleHeader_));
    int footerWidth = qMin(400, headingFM.width(visibleFooter_));
    int maxHeadingWidth = qMax(headerWidth, footerWidth);
    return QSize(qMax(minW, maxHeadingWidth), minH);
}

void OneSession::relayout(uint realWidth)
{
    QMutexLocker lock(relayoutMutex_.data());

    // 1. Main text
    Q_ASSERT(lines_.size()==props_.size());
    Q_ASSERT(lines_.size()==selectedLineEnds_.size());
    visibleLines_.clear();
    uint maxLineLength = 0;
    for (int i=0; i<lines_.size(); i++) {
        const QString & text = lines_[i];
        LineProp & prop = props_[i];
        bool & selectedEnd = selectedLineEnds_[i];
        Q_ASSERT(text.length()==prop.size());
        const uint charsInLine = text.length();
        const uint charsInVisibleLine = fixedWidth_ == -1
                ? widthInChars(realWidth)
                : fixedWidth_;
        const uint visibleLinesCount = charsInVisibleLine <= charsInLine
                ? 1u
                : 1u + charsInLine / charsInVisibleLine;

        uint currentOffset = 0u;

        for (uint visibleLineNo = 0;
             visibleLineNo < visibleLinesCount;
             visibleLineNo++)
        {
            VisibleLine vline(text,
                              prop,
                              selectedEnd,
                              currentOffset,
                              qMin(currentOffset + charsInVisibleLine,
                                   charsInLine)
                              );
            visibleLines_.push_back(vline);
            maxLineLength = qMax(maxLineLength, charsInVisibleLine);
            currentOffset += charsInVisibleLine;
        }
    }
    uint top =
            QFontMetrics(utilityFont()).height() + HeaderPadding + BodyPadding;
    uint height = charSize().height() * visibleLines_.size();
    uint left = BodyPadding;
    uint width = charSize().width() * maxLineLength;
    mainTextRegion_ = QRect(left, top, width, height);

    // 2. Header
    visibleHeader_ = headerText();
    headerProp_.clear();
    for (size_t i=0; i<visibleHeader_.length(); i++) {
        headerProp_.push_back(CS_Output);
    }
    headerRect_ = QRect(BodyPadding,
                        0,
                        QFontMetrics(utilityFont()).width(visibleHeader_),
                        QFontMetrics(utilityFont()).height() + HeaderPadding);

    // 3. Footer
    visibleFooter_ = footerText();
    footerProp_.clear();
    for (size_t i=0; i<visibleFooter_.length(); i++) {
        footerProp_.push_back(CS_Output);
    }
    footerRect_ = footerText().isEmpty()
            ? QRect(BodyPadding,
                    mainTextRegion_.bottom() + HeaderPadding + BodyPadding,
                    0,
                    0)
            : QRect(BodyPadding,
                    mainTextRegion_.bottom() + HeaderPadding + BodyPadding,
                    QFontMetrics(utilityFont()).width(visibleFooter_),
                    QFontMetrics(utilityFont()).height());


}

void OneSession::draw(QPainter &p) const
{
    QMutexLocker lock(relayoutMutex_.data());
//    p.save();
//    p.setPen(QColor(Qt::red));
//    p.drawRect(mainTextRegion_);
//    p.setPen(QColor(Qt::blue));
//    p.drawRect(headerRect_);
//    p.setPen(QColor(Qt::green));
//    p.drawRect(footerRect_);
//    p.restore();
    drawUtilityText(p, visibleHeader_, headerProp_, headerRect_.topLeft());
//    drawInputRect(p, 0);
    drawMainText(p, mainTextRegion_.topLeft());
    drawUtilityText(p, visibleFooter_, footerProp_, footerRect_.topLeft());
    drawCursor(p);
}

void OneSession::drawCursor(QPainter &p) const
{
    if (inputCursorVisible_) {
        const QSize atom = charSize();
        p.save();
        uint top = mainTextRegion_.top() +
                inputLineStart_ * atom.height() +
                InputCursorThickness
                ;
        uint left = mainTextRegion_.left() +
                (inputPosStart_ + inputCursorPosition_) * atom.width();
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(Qt::black));
        p.drawRect(left, top, InputCursorThickness, atom.height());
        p.restore();
    }
}

uint OneSession::drawUtilityText(
        QPainter &p,
        const QString &text,
        const LineProp & prop,
        const QPoint & topLeft
        ) const
{
    Q_ASSERT(text.length() == prop.size());
    p.save();
    p.setFont(utilityFont());
    const QBrush selectionBackroundBrush = parent_->palette().brush(
                parent_->hasFocus() ? QPalette::Active : QPalette::Inactive,
                QPalette::Highlight
                );
    const QColor selectedTextColor = parent_->palette().brush(
                parent_->hasFocus() ? QPalette::Active : QPalette::Inactive,
                QPalette::HighlightedText
                ).color();
    const QFontMetrics fm(utilityFont());
    const uint height = fm.height();
    uint xx = topLeft.x();
    for (size_t i=0; i<text.length(); i++) {
        const QChar ch = text.at(i);
        const CharSpec cs = prop[i];
        const uint cw = fm.width(ch);
        if (cs & SelectionMask) {
            p.setPen(Qt::NoPen);
            p.setBrush(selectionBackroundBrush);
            p.drawRect(xx, topLeft.y(), cw, height);
            p.setPen(selectedTextColor);
        }
        else {
            p.setPen(QColor(Qt::darkGray));
        }
        p.drawText(xx, topLeft.y() + height, QString(ch));
        xx += cw;
    }
    p.restore();
    return  height;
}

void OneSession::drawInputRect(QPainter &p, const uint mainTextY) const
{
    if (inputCursorPosition_!=-1) {
        p.save();
        const QSize atom = charSize();
        const QRect highlightRect(
                    BodyPadding + inputPosStart_ * atom.width(),
                    mainTextY + inputLineStart_ * atom.height(),
                    parent_->width() - (inputPosStart_ * atom.width()) - 2 * BodyPadding,
                    atom.height() + 4
                    );

        // Prepare a brush gradient based on current highlight color
        QLinearGradient gr(QPointF(0,0),QPointF(0,1));
        gr.setCoordinateMode(QGradient::ObjectBoundingMode);
        QColor c1 = QColor("green").lighter();
        c1.setAlpha(32);
        QColor c2 = QColor("green").lighter();
        gr.setColorAt(0, c1);
        gr.setColorAt(1, c2);

        // Draw a rect
        p.setBrush(gr);
        p.setPen(Qt::NoPen);
        p.drawRect(highlightRect);

        // Draw borders
        p.setPen(QColor("green"));
        p.drawLine(highlightRect.topLeft(),
                   highlightRect.topRight());
        p.drawLine(highlightRect.bottomLeft(),
                   highlightRect.bottomRight());
        p.drawLine(highlightRect.bottomLeft(),
                   highlightRect.topLeft());
        p.drawLine(highlightRect.bottomRight(),
                   highlightRect.topRight());
        p.restore();
    }
}

uint OneSession::drawMainText(QPainter &p, const QPoint & topLeft) const
{
    const QSize atom = charSize();
    const QBrush selectionBackroundBrush = parent_->palette().brush(
                parent_->hasFocus() ? QPalette::Active : QPalette::Inactive,
                QPalette::Highlight
                );
    const QColor selectedTextColor = parent_->palette().brush(
                parent_->hasFocus() ? QPalette::Active : QPalette::Inactive,
                QPalette::HighlightedText
                ).color();
    p.save();
    p.setFont(font_);
    for (size_t i=0; i<visibleLines_.size(); i++) {
        uint xx = topLeft.x();
        uint yy = topLeft.y() + i * atom.height() + atom.height();
        const VisibleLine & vline = visibleLines_.at(i);
        const QString & text = vline.text;
        const LineProp & prop = vline.prop;
        const size_t from = vline.from;
        const size_t to = vline.to;
        for (size_t j=from; j<to; j++) {
            const QChar symbol = text.at(j);
            const CharSpec spec = prop.at(j);
            if (spec & SelectionMask) {
                p.setPen(Qt::NoPen);
                p.setBrush(selectionBackroundBrush);
                p.drawRect(xx, yy-atom.height(), atom.width(), atom.height());
            }
            if (spec & SelectionMask)
                p.setPen(selectedTextColor);
            else if (spec == CS_Error || spec == CS_InputError)
                p.setPen(QColor(Qt::red));
            else if (spec == CS_Input)
                p.setPen(QColor(Qt::blue));
            else
                p.setPen(QColor(Qt::black));
            p.drawText(xx, yy, QString(symbol));
            xx += atom.width();
        }
        if (vline.text.length() == 0 && vline.endSelected) {
            p.setPen(Qt::NoPen);
            p.setBrush(selectionBackroundBrush);
            p.drawRect(xx, yy-atom.height(), atom.width() / 2, atom.height());
        }
    }
    p.restore();
    return atom.height() * visibleLines_.size();
}


void OneSession::clearSelection()
{
    for (int x = 0; x < headerProp_.size(); x++) {
        headerProp_[x] = Terminal::CharSpec(headerProp_[x] & 0xFF);
    }
    for (int x = 0; x < footerProp_.size(); x++) {
        footerProp_[x] = Terminal::CharSpec(footerProp_[x] & 0xFF);
    }
    for (int y=0; y<props_.size(); y++) {
        LineProp & lineProp = props_[y];
        for (int x=0; x<lineProp.size(); x++) {
            lineProp[x] = Terminal::CharSpec(lineProp[x] & 0xFF);
        }
        selectedLineEnds_[y] = false;
    }
}

bool OneSession::hasSelectedText() const
{
    for (int x=0; x<headerProp_.size(); x++) {
        if (headerProp_[x] & SelectionMask)
            return true;
    }
    for (int x=0; x<footerProp_.size(); x++) {
        if (footerProp_[x] & SelectionMask)
            return true;
    }
    for (int y=0; y<lines_.size(); y++) {
        const LineProp & lineProp = props_[y];
        for (int x=0; x<lineProp.size(); x++) {
            if (lineProp[x] & SelectionMask)
                return true;
        }
    }
    return false;
}

bool OneSession::isEditable() const
{
    return inputCursorPosition_ != -1;
}

QString OneSession::selectedText() const
{
    QString result;
    for (int x = 0; x < headerProp_.size(); x++) {
        if (headerProp_[x] & SelectionMask)
            result += visibleHeader_[x];
    }
    if (result.length() > 0)
        result += "\n";
    for (int y=0; y<lines_.size(); y++) {
        QString thisLineText;
        const QString & thisLine = lines_[y];
        const LineProp & thisProp = props_[y];
        for (int x=0; x<thisLine.length(); x++) {
            if (thisProp[x] & SelectionMask)
                thisLineText += thisLine[x];
        }        
        result += thisLineText;
        if (selectedLineEnds_[y])
            result += "\n";
    }
    if (result.length() > 0 && visibleFooter_.length() > 0 && !result.endsWith("\n"))
        result += "\n";
    for (int x = 0; x < footerProp_.size(); x++) {
        if (footerProp_[x] & SelectionMask)
            result += visibleFooter_[x];
    }
    return result;
}

static QString lineToRtf(const QString & text,
                         const bool utilText,
                         const LineProp & prop,
                         size_t from, size_t to
                         )
{
    struct Chunk {
        QString text;
        bool italic;
        uint color;
        QString data;
        CharSpec spec;
    };

    QList<Chunk> result;
    QString rtf;

    // Split text into chunks of various formats
    for (size_t i=from; i<to; i++) {
        if (result.isEmpty())
        {
            // Create first empty chunk of text
            Chunk chunk;
            chunk.italic = utilText;
            chunk.color = utilText? 4u : 0u;
            if (prop[i] & CS_Input)
                chunk.color = 3u;
            else if (prop[i] & CS_Error || prop[i] & CS_InputError)
                chunk.color = 2u;
            chunk.text += text.at(i);
            chunk.spec = prop[i];
            result.push_back(chunk);
        }
        else if (result.last().spec==prop[i])
        {
            // Continue the same format chunk of text
            Chunk & chunk = result.last();
            chunk.text += text.at(i);
        }
        else
        {
            // Make new format chunk of text
            Chunk chunk;
            chunk.italic = utilText;
            chunk.color = utilText? 4u : 0u;
            if (prop[i] & CS_Input)
                chunk.color = 3u;
            else if (prop[i] & CS_Error || prop[i] & CS_InputError)
                chunk.color = 2u;
            chunk.text += text.at(i);
            chunk.spec = prop[i];
            result.push_back(chunk);
        }
    }

    // Make RTF data for chunks
    for (size_t i=0; i<result.size(); i++) {
        rtf += "{";
        const Chunk & chunk = result[i];
        if (chunk.color)
            rtf += "\\cf"+QString::number(chunk.color).toAscii();
        if (chunk.italic)
            rtf += "\\i";
        if (chunk.italic || chunk.color)
            rtf += " ";
        rtf += chunk.text;
        rtf += "}";
    }

    return rtf;
}

void OneSession::selectAll()
{
    for (int i=0; i<headerProp_.size(); i++) {
        headerProp_[i] = CharSpec(headerProp_[i] | SelectionMask);
    }
    for (int i=0; i<footerProp_.size(); i++) {
        footerProp_[i] = CharSpec(footerProp_[i] | SelectionMask);
    }
    for (int l=0; l<props_.size(); l++) {
        LineProp & lp = props_[l];
        for (int i=0; i<lp.size(); i++) {
            lp[i] = CharSpec(lp[i] | SelectionMask);
        }
        selectedLineEnds_[l] = true;
    }
    emit updateRequest();
}

QString OneSession::selectedRtf() const
{
    QString result;
    {
        // Header
        int from = -1;
        int to = -1;
        for (int i=0; i<headerProp_.size(); i++) {
            if (headerProp_[i] & SelectionMask) {
                if (from == -1)
                    from = i;
                to = i + 1;
            }
        }
        if (from != -1 && to != -1) {
            result += lineToRtf(visibleHeader_, true, headerProp_, from, to);
            result += "\\par\r\n";
        }

    }
    {
        // Main text
        for (int l=0; l<lines_.size(); l++) {
            const QString & text = lines_.at(l);
            const LineProp & prop = props_.at(l);

            int from = -1;
            int to = -1;

            for (int i=0; i<prop.size(); i++) {
                if (prop[i] & SelectionMask) {
                    if (from == -1)
                        from = i;
                    to = i + 1;
                }
            }
            if (from!=-1 && to != -1) {
                result += lineToRtf(text, false, prop, from, to);
            }
            if (selectedLineEnds_[l]) {
                result += "\\par\r\n";
            }
        }
    }
    {
        // Footer
        int from = -1;
        int to = -1;
        for (int i=0; i<footerProp_.size(); i++) {
            if (footerProp_[i] & SelectionMask) {
                if (from == -1)
                    from = i;
                to = i + 1;
            }
        }
        if (from != -1 && to != -1) {
            if (!result.isEmpty() && !result.endsWith("\\par\r\n"))
                result += "\\par\r\n";
            result += lineToRtf(visibleFooter_, true, footerProp_, from, to);
        }

    }
    return result;
}

void OneSession::triggerTextSelection(const QPoint &fromPos, const QPoint &toPos)
{
    clearSelection();
    const QFontMetrics mfm(font());
    const QFontMetrics ufm(utilityFont());
    const int fromY = qMin(fromPos.y(), toPos.y());
    const int toY = qMax(fromPos.y(), toPos.y());
    const int fromX = qMin(fromPos.x(), toPos.x());
    const int toX = qMax(fromPos.x(), toPos.x());

    // Header
    if (fromY <= headerRect_.bottom() && toY >= headerRect_.top()) {
       int fromChar = 0;
       int toChar = visibleHeader_.length();
       if (fromY > headerRect_.top())
           fromChar = (fromX - headerRect_.left()) / ufm.width('m');
       if (toY <= headerRect_.bottom())
           toChar = (toX - headerRect_.left()) / ufm.width('m');
       fromChar = qMax(0, fromChar);
       toChar = qMin(visibleHeader_.length(), toChar);
       for (int i=fromChar; i<toChar; i++) {
           headerProp_[i] = Terminal::CharSpec(headerProp_[i] | SelectionMask);
       }
    }

    // Footer
    if (fromY <= footerRect_.bottom() && toY >= footerRect_.top()) {
       int fromChar = 0;
       int toChar = visibleFooter_.length();
       if (fromY > footerRect_.top())
           fromChar = (fromX - footerRect_.left()) / ufm.width('m');
       if (toY <= footerRect_.bottom())
           toChar = (toX - footerRect_.left()) / ufm.width('m');
       fromChar = qMax(0, fromChar);
       toChar = qMin(visibleFooter_.length(), toChar);
       for (int i=fromChar; i<toChar; i++) {
           footerProp_[i] = Terminal::CharSpec(footerProp_[i] | SelectionMask);
       }
    }

    // Main text
    for (int l = 0; l < visibleLines_.size(); l++) {
        const VisibleLine & line = visibleLines_[l];
        const QString thisLineText = line.text.mid(line.from, line.to - line.from);
        const QRect thisLineRect = QRect(
                    mainTextRegion_.left(),
                    mainTextRegion_.top() + l * mfm.height(),
                    mfm.width(thisLineText),
                    mfm.height()
                    );
        if (fromY <= thisLineRect.bottom() && toY >= thisLineRect.top()) {
            size_t fromChar = line.from;
            size_t toChar = line.to;
            if (fromY > thisLineRect.top())
                fromChar = line.from + (fromX - thisLineRect.left()) / mfm.width('m');
            if (toY <= thisLineRect.bottom())
                toChar = line.from + (toX - thisLineRect.left()) / mfm.width('m');
            if (toY > thisLineRect.bottom() || toX > thisLineRect.right())
                line.endSelected = true;
            fromChar = qMax(line.from, fromChar);
            toChar = qMin(line.to, toChar);
            for (size_t i=fromChar; i<toChar; i++) {
                line.prop[i] = Terminal::CharSpec(line.prop[i] | SelectionMask);
            }
        }
    }
    emit updateRequest();
}

QPoint OneSession::cursorPositionByVisiblePosition(const QPoint &pos) const
{
    const QSize atom = charSize();

    int offsetX = BodyPadding;
    int offsetY = 2 * HeaderPadding + 2 * atom.height() + BodyPadding;

    const QPoint bodyPos = pos - QPoint(offsetX, offsetY);

    QPoint cursor(bodyPos.x()/atom.width(), bodyPos.y()/atom.height());
    cursor.setY(qMin(lines_.size()-1,
                    qMax(0,
                        cursor.y()))
                );
    cursor.setX(qMin(lines_.isEmpty()? 0 : lines_.at(cursor.y()).length(),
                     qMax(0,
                          cursor.x())
                    ));
    return cursor;
}

QString OneSession::headerText() const
{
    return tr(">> %1:%2:%3 - %4 - Process started")
                .arg(startTime_.time().hour(), 2, 10, QChar(' '))
                .arg(startTime_.time().minute(), 2, 10, QChar('0'))
                .arg(startTime_.time().second(), 2, 10, QChar('0'))
                .arg(fileName_);
}

QString OneSession::footerText() const
{
    return endTime_.isValid()
            ? tr(">> %1:%2:%3 - %4 - Process finished")
              .arg(endTime_.time().hour(), 2, 10, QChar(' '))
              .arg(endTime_.time().minute(), 2, 10, QChar('0'))
              .arg(endTime_.time().second(), 2, 10, QChar('0'))
              .arg(fileName_)
            : "";
}

QFont OneSession::utilityFont() const
{
    QFont smallFont = font_;
    smallFont.setPointSize(font_.pointSize()-2);
    smallFont.setItalic(true);
    return smallFont;
}

void OneSession::output(const QString &text, const CharSpec cs)
{
    int curLine = lines_.size()-1;
    int curCol = lines_.isEmpty()? 0 : lines_[curLine].length();
    for (int i=0; i<text.length(); i++) {
        bool newLine = curLine<0 || text[i]=='\n' || ( fixedWidth_!=-1 && curCol>=fixedWidth_ );
        if (newLine) {
            lines_.append("");
            props_.push_back(LineProp());
            selectedLineEnds_.append(false);
            curLine ++;
            curCol = 0;
        }
        if (text[i].unicode()>=32) {
            lines_[curLine] += text[i];
            props_[curLine].push_back(cs);
        }
    }
    relayout(parent_->width() - 2 * SessionMargin);
    emit updateRequest();
}

void OneSession::input(const QString &format)
{
    inputFormat_ = format;
    if (lines_.isEmpty()) {
        lines_ << "";
        props_.push_back(LineProp());
        selectedLineEnds_ << false;
    }
    inputLineStart_ = lines_.size()-1;
    inputPosStart_ = 0;
    if (!lines_.isEmpty()) {
        inputPosStart_ = lines_.last().length();
    }
    inputCursorPosition_ = 0;
    inputCursorVisible_ = true;   
    QString msg;
    if (format.length()==1 && format.at(0)=='w') {
        msg = tr("INPUT raw data to console stream");
    }
    else {
        msg += tr("INPUT ");
        QStringList fmts = format.split(";", QString::SkipEmptyParts);
        for (int i=0; i<fmts.size(); i++) {
            if (i>0) {
                msg += ", ";
            }
            if (fmts[i][0]=='s')
                msg += tr("string");
            else if (fmts[i][0]=='i')
                msg += tr("integer");
            else if (fmts[i][0]=='r')
                msg += tr("real");
            else if (fmts[i][0]=='c')
                msg += tr("charect");
            else if (fmts[i][0]=='b')
                msg += tr("boolean");
            else if (fmts[i].contains("::")) {
                QStringList typeName = fmts[i].split("::", QString::KeepEmptyParts);
                msg += typeName[1];
            }

        }
        msg += ".";
    }
    emit message(msg);
    timerId_ = startTimer(QApplication::cursorFlashTime()/2);
    emit updateRequest();
}

void OneSession::timerEvent(QTimerEvent *e)
{
    inputCursorVisible_ = !inputCursorVisible_;
    emit updateRequest();
    e->accept();
}

void OneSession::changeCursorPosition(quint16 pos)
{
    inputCursorPosition_ = pos;
    inputCursorVisible_ = true;
    emit updateRequest();
}

void OneSession::changeInputText(const QString &text)
{
    lines_ = lines_.mid(0, inputLineStart_+1);
    props_.resize(inputLineStart_+1);
    selectedLineEnds_ = selectedLineEnds_.mid(0, inputLineStart_ + 1);
    if (!lines_.isEmpty()) {
        lines_[lines_.size()-1] = lines_[lines_.size()-1].mid(0,inputPosStart_);
        props_[props_.size()-1].resize(inputPosStart_);
    }
    int curCol = inputPosStart_;
    int curLine = lines_.size()-1;
    for (int i=0; i<text.length(); i++) {
        bool newLine = curLine<0 || ( fixedWidth_!=-1 && curCol>=fixedWidth_ );
        if (newLine) {
            lines_.append("");
            props_.push_back(LineProp());
            selectedLineEnds_.append(false);
            curLine ++;
            curCol = 0;
        }
        if (text[i].unicode()>=32) {
            lines_[curLine] += text[i];
            props_[curLine].push_back(CS_Input);
        }
    }
    relayout(parent_->width() - 2 * SessionMargin);
    emit updateRequest();
}

void OneSession::tryFinishInput()
{
    QString text;
    for (int i=inputLineStart_; i<lines_.size(); i++) {
        if (i==inputLineStart_)
            text += lines_[i].mid(inputPosStart_);
        else
            text += lines_[i];
    }
    QVector<bool> errmask = QVector<bool>(text.length(), false);
    QVariantList result;
    Kumir::IO::InputStream stream(text.toStdWString());
    bool conversionError = false;
    int conversionErrorStart = -1;
    int conversionErrorLength = 0;
    bool ignoreGarbageError = false;
    QString conversionErrorType = "";
    const QStringList & formats = inputFormat_.split(";", QString::SkipEmptyParts);
    for (int i=0; i<formats.size(); i++) {
        char type = formats[i][0].unicode();
        const QString format = formats[i];
        if (format[0]=='i') {
            int value = Kumir::IO::readInteger(stream);
            result << value;
        }
        else if (format[0]=='r') {
            double value = Kumir::IO::readReal(stream);
            result << value;
        }
        else if (format[0]=='b') {
            bool value = Kumir::IO::readBool(stream);
            result << value;
        }
        else if (format[0]=='c') {
            Kumir::Char value = Kumir::IO::readChar(stream);
            result << QChar(value);
        }
        else if (format[0]=='s') {
            Kumir::String value = Kumir::IO::readLine(stream);
            result << QString::fromStdWString(value);
        }
        else if (format[0]=='n') {
            Kumir::IO::readLine(stream);
            result << QChar('\n');
        }
        else if (format[0]=='w') {  // raw data
            result << QString(text + "\n");
            ignoreGarbageError = true;
        }
        else if (format.contains("::")) {
            const QStringList typeName = format.split("::", QString::KeepEmptyParts);
            const QString & moduleName = typeName[0];
            const QString & className  = typeName[1];
            QList<ExtensionSystem::KPlugin*> plugins =
                    ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
            Shared::ActorInterface * actor = 0;
            for (int i=0; i<plugins.size(); i++) {
                actor = qobject_cast<Shared::ActorInterface*>(plugins[i]);
                if (actor) {
                    if (actor->name()==moduleName)
                        break;
                    else
                        actor = 0;
                }
            }
            if (actor) {
                stream.skipDelimiters(Kumir::IO::inputDelimeters);
                conversionErrorStart = stream.currentPosition();
                QString lexem = QString::fromStdWString(Kumir::IO::readString(stream));
                QVariant value;
                if (!stream.hasError()) {
                    Shared::ActorInterface::CustomType ct;
                    ct.first = className;
                    value = actor->customValueFromString(ct, lexem);
                    if (!value.isValid()) {
                        conversionError = true;
                        conversionErrorLength = lexem.length();
                        conversionErrorType   = className;
                        break;
                    }
                }
                result << value;
            }
            else {
                result << QVariant::Invalid;
            }
        }
        if (stream.hasError() || stream.currentPosition() == text.length())
            break;
    }
    bool hasError    = false;
    int  errorStart  = -1;
    int  errorLength = 0;
    QString errorMessage;

    if (stream.hasError()) {
        hasError = true;
        Kumir::String errorText;
        stream.getError(errorText, errorStart, errorLength);
        errorMessage = QString::fromStdWString(errorText);
    }

    if (conversionError) {
        hasError = true;
        errorStart = conversionErrorStart;
        errorLength = conversionErrorLength;
        errorMessage = tr("INPUT ERROR: Not a '%1' value").arg(conversionErrorType);
    }

    if (stream.currentPosition() < text.length()) {
        const QString remainder = text.mid(stream.currentPosition());
        if (!ignoreGarbageError && !remainder.trimmed().isEmpty() && !hasError) {
            hasError = true;
            errorStart = stream.currentPosition();
            errorLength = remainder.length();
            errorMessage = tr("INPUT ERROR: Extra input");
        }
    }

    if (hasError) {
        emit message(errorMessage);
        for (int i=0; i<errorLength; i++) {
            errmask[errorStart+i] = true;
        }
        int curLine = inputLineStart_;
        int curCol = inputPosStart_;
        LineProp & lp = props_[curLine];
        for (int i=0; i<text.length(); i++) {
            bool newLine = curLine<0 || ( fixedWidth_!=-1 && curCol>=fixedWidth_ );
            if (newLine) {
                curLine ++;
                lp = props_[curLine];
                curCol = 0;
            }
            else {
                if (errmask[i]) {
                    lp[curCol] = CS_InputError;
                }
                else {
                    lp[curCol] = CS_Input;
                }
                curCol ++;
            }
        }
        emit updateRequest();
    }
    else {
        inputLineStart_ = inputPosStart_ = inputCursorPosition_ = -1;
        output("\n", CS_Output);
        emit message("");
        emit inputDone(result);
        emit updateRequest();
    }
}

void OneSession::error(const QString &message)
{
    inputLineStart_ = inputPosStart_ = inputCursorPosition_ = -1;
    lines_.append(tr("RUNTIME ERROR: %1").arg(message));
    props_.push_back(LineProp());
    selectedLineEnds_.append(false);
    for (int i=0; i<lines_.last().size(); i++) {
        props_[props_.size()-1].push_back(CS_Error);
    }
    endTime_ = QDateTime::currentDateTime();
    relayout(parent_->width() - 2 * SessionMargin);
    emit updateRequest();
}

void OneSession::terminate()
{
    endTime_ = QDateTime::currentDateTime();
    if (timerId_!=-1)
        killTimer(timerId_);
    inputLineStart_ = inputPosStart_ = inputCursorPosition_ = -1;
    relayout(parent_->width() - 2 * SessionMargin);
    emit updateRequest();
}

void OneSession::finish()
{
    endTime_ = QDateTime::currentDateTime();
    inputLineStart_ = inputPosStart_ = inputCursorPosition_ = -1;
    relayout(parent_->width() - 2 * SessionMargin);
    emit updateRequest();
}

} // namespace Terminal
