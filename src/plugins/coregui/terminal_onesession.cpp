#include "terminal_onesession.h"
#include "terminal.h"
#include "stdlib/kumirstdlib.hpp"
#include "extensionsystem/pluginmanager.h"
#include "interfaces/actorinterface.h"


namespace Terminal {

static const int BodyPadding = 4;
static const int HeaderPadding = 4;
static const int LineWidth = 1;
static const int ShadowOffset = 4;

static const unsigned int SelectionMask = 0xFF00;

OneSession::OneSession(int fixedWidth, const QString & fileName, QWidget * parent)
    : QObject(parent)
    , parent_(parent)
    , fileName_(fileName)
    , fixedWidth_(fixedWidth)
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

QSize OneSession::visibleSize(int realWidth) const
{
    QSize atom = charSize();
    QSize body;
    if (fixedWidth_==-1) {
        int maxLineLength = 0;
        int linesCount = 0;
        int maxChars = widthInChars(realWidth);
        foreach (const QString & s, lines_) {
            maxLineLength = qMax(s.length(), maxLineLength);
            linesCount += 1+(s.length())/maxChars;
        }
        body = QSize(maxLineLength*atom.width(), linesCount*atom.height());
    }
    else {
        int maxLineLength = fixedWidth_;
        int linesCount = lines_.size();
        body = QSize(maxLineLength*atom.width(), linesCount*atom.height());
    }

    body.setWidth(body.width() + 2 * BodyPadding);
    body.setHeight(body.height() + 2 * BodyPadding);
    const int headerHeight = 2 * HeaderPadding + atom.height();
    const int footerHeight = endTime_.isValid()? headerHeight : 0;
    return QSize(body.width()+LineWidth+ShadowOffset,
                 body.height()+headerHeight+footerHeight+LineWidth+ShadowOffset);
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

void OneSession::draw(QPainter *p, int realWidth)
{
    const QSize atom = charSize();
    QSize sz = visibleSize(realWidth);
    p->save();
    if (fixedWidth_==-1)
        sz.setWidth(realWidth);
    else {
        p->translate((realWidth-sz.width())/2, 0);
    }


    const QString header = headerText();
    const QString footer = footerText();
    p->setFont(utilityFont());
    p->setPen(QColor(Qt::darkGray));
    p->drawText(HeaderPadding, HeaderPadding+QFontMetrics(utilityFont()).height(), header);

    int x = BodyPadding;
    int y = 2 * HeaderPadding + 2 * atom.height() + BodyPadding;

    if (inputCursorPosition_!=-1 && inputCursorVisible_ && (qobject_cast<QWidget*>(parent()))->hasFocus()) {
        int cursorRow = inputLineStart_;
        int cursorCol = inputCursorPosition_+inputPosStart_;
        QRect cursorRect(x + cursorCol * atom.width(),
                         y + cursorRow * atom.height(),
                         atom.width(),
                         2
                         );
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(Qt::black));
        p->drawRect(cursorRect);
    }

    Q_ASSERT(lines_.size()==props_.size());
    p->setFont(font_);

    const QBrush selectionBackroundBrush = parent_->palette().brush(
                parent_->hasFocus() ? QPalette::Active : QPalette::Inactive,
                QPalette::Highlight
                );
    const QColor selectedTextColor = parent_->palette().brush(
                parent_->hasFocus() ? QPalette::Active : QPalette::Inactive,
                QPalette::HighlightedText
                ).color();

    for (int i=0; i<lines_.size(); i++) {
        const QString text = lines_[i];
        const LineProp prop = props_[i];
        Q_ASSERT(text.length()==prop.size());
        for (int j=0; j<text.length(); j++) {
            if (prop[j] & SelectionMask) {
                p->setPen(Qt::NoPen);
                p->setBrush(selectionBackroundBrush);
                p->drawRect(x, y-atom.height(), atom.width(), atom.height());
            }
            if (prop[j] & SelectionMask)
                p->setPen(selectedTextColor);
            else if (prop[j] & CS_Input)
                p->setPen(QColor(Qt::blue));
            else if (prop[j] & CS_Error || prop[j] & CS_InputError)
                p->setPen(QColor(Qt::red));
            else
                p->setPen(QColor(Qt::black));
            p->drawText(x,y,QString(text[j]));
            x += atom.width();
            if (fixedWidth_==-1) {
                int lw = widthInChars(realWidth);
                if (j % lw==0 && j>0) {
                    x = BodyPadding;
                    y += atom.height();
                }
            }
        }
        y += atom.height();
        x = BodyPadding;
    }

    if (!footer.isEmpty()) {
        p->setFont(utilityFont());
        p->setPen(QColor(Qt::darkGray));
        p->drawText(HeaderPadding,
                    sz.height()-LineWidth-ShadowOffset-HeaderPadding,
                    footer);
        p->setPen(QColor(Qt::darkGray));
        p->drawLine(BodyPadding, sz.height()-HeaderPadding, sz.width()-BodyPadding, sz.height()-HeaderPadding);
    }
    p->restore();
}

void OneSession::clearSelection()
{
    for (int y=0; y<props_.size(); y++) {
        LineProp & lineProp = props_[y];
        for (int x=0; x<lineProp.length(); x++) {
            lineProp[x] = Terminal::CharSpec(lineProp[x] & 0xFF);
        }
    }
}

bool OneSession::hasSelectedText() const
{
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
    for (int y=0; y<lines_.size(); y++) {
        QString thisLineText;
        const QString & thisLine = lines_[y];
        const LineProp & thisProp = props_[y];
        for (int x=0; x<thisLine.length(); x++) {
            if (thisProp[x] & SelectionMask)
                thisLineText += thisLine[x];
        }
        if (result.length()>0 && thisLineText.length()>0) {
            result += "\n";
        }
        result += thisLineText;
    }
    return result;
}

void OneSession::triggerTextSelection(const QPoint &fromPos, const QPoint &toPos)
{
    QPoint fromCursorPos = cursorPositionByVisiblePosition(fromPos);
    QPoint toCursorPos = cursorPositionByVisiblePosition(toPos);
    int y0 = qMin(fromCursorPos.y(), toCursorPos.y());
    int y1 = qMax(fromCursorPos.y(), toCursorPos.y());
    int x0 = qMin(fromCursorPos.x(), toCursorPos.x());
    int x1 = qMax(fromCursorPos.x(), toCursorPos.x());
//    qDebug() << __FUNCTION__ << fromCursorPos << toCursorPos;
    for (int y=y0; y<=y1; y++) {
        LineProp & lineProp = props_[y];
        int xx0 = y==y0? x0 : 0;
        int xx1 = y==y1? x1 : lineProp.length();
        for (int x=xx0; x<xx1; x++) {
            lineProp[x] = Terminal::CharSpec(lineProp[x] | SelectionMask);
        }
    }
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
    cursor.setX(qMin(lines_.at(cursor.y()).length(),
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

void OneSession::output(const QString &text)
{
    int curLine = lines_.size()-1;
    int curCol = lines_.isEmpty()? 0 : lines_[curLine].length();
    for (int i=0; i<text.length(); i++) {
        bool newLine = curLine<0 || text[i]=='\n' || ( fixedWidth_!=-1 && curCol>=fixedWidth_ );
        if (newLine) {
            lines_.append("");
            props_.append(LineProp());
            curLine ++;
            curCol = 0;
        }
        if (text[i].unicode()>=32) {
            lines_[curLine] += text[i];
            props_[curLine].append(CS_Output);
            Q_ASSERT(props_[curLine].last()==CS_Output);
        }
    }
    emit updateRequest();
}

void OneSession::input(const QString &format)
{
    inputFormat_ = format;
    if (lines_.isEmpty()) {
        lines_ << "";
        props_ << LineProp();
    }
    inputLineStart_ = lines_.size()-1;
    inputPosStart_ = 0;
    if (!lines_.isEmpty()) {
        inputPosStart_ = lines_.last().length();
    }
    inputCursorPosition_ = 0;
    inputCursorVisible_ = true;
//    StdLib::GenericInputOutput::instance()->doInput(format);
    QString msg = tr("INPUT ");
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
    props_ = props_.mid(0, inputLineStart_+1);
    if (!lines_.isEmpty()) {
        lines_[lines_.size()-1] = lines_[lines_.size()-1].mid(0,inputPosStart_);
        props_[props_.size()-1] = props_[props_.size()-1].mid(0,inputPosStart_);
    }
    int curCol = inputPosStart_;
    int curLine = lines_.size()-1;
    for (int i=0; i<text.length(); i++) {
        bool newLine = curLine<0 || ( fixedWidth_!=-1 && curCol>=fixedWidth_ );
        if (newLine) {
            lines_.append("");
            props_.append(LineProp());
            curLine ++;
            curCol = 0;
        }
        if (text[i].unicode()>=32) {
            lines_[curLine] += text[i];
            props_[curLine].append(CS_Input);
            Q_ASSERT(props_[curLine].last()==CS_Input);
        }
    }
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
            Kumir::String value = Kumir::IO::readString(stream);
            result << QString::fromStdWString(value);
        }
        else if (format[0]=='n') {
            Kumir::IO::readLine(stream);
            result << QChar('\n');
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
        if (stream.hasError())
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
        errorMessage = tr("Not a '%1' value").arg(conversionErrorType);
    }

    if (hasError) {
        emit message(errorMessage);
        for (int i=0; i<errorLength; i++) {
            errmask[errorStart+i] = true;
        }
        int curLine = inputLineStart_;
        int curCol = inputPosStart_;
        for (int i=0; i<text.length(); i++) {
            bool newLine = curLine<0 || ( fixedWidth_!=-1 && curCol>=fixedWidth_ );
            if (newLine) {
                curLine ++;
                curCol = 0;
            }
            else {
                Q_ASSERT(props_[curLine][curCol]==CS_Input || props_[curLine][curCol]==CS_InputError);
                props_[curLine][curCol] = errmask[i]? CS_InputError : CS_Input;
                Q_ASSERT(props_[curLine][curCol]==CS_Input || props_[curLine][curCol]==CS_InputError);
                curCol ++;
            }
        }
        emit updateRequest();
    }
    else {
        inputLineStart_ = inputPosStart_ = inputCursorPosition_ = -1;
        output("\n");
        emit message("");
        emit inputDone(result);
        emit updateRequest();
    }
}

void OneSession::error(const QString &message)
{
    lines_.append(tr("RUNTIME ERROR: %1").arg(message));
    props_.append(LineProp());
    for (int i=0; i<lines_.last().size(); i++) {
        props_[props_.size()-1] << CS_Error;
    }
    endTime_ = QDateTime::currentDateTime();
    emit updateRequest();
}

void OneSession::terminate()
{
    endTime_ = QDateTime::currentDateTime();
    if (timerId_!=-1)
        killTimer(timerId_);
    inputLineStart_ = inputPosStart_ = inputCursorPosition_ = -1;
    emit updateRequest();
}

void OneSession::finish()
{
    endTime_ = QDateTime::currentDateTime();
    emit updateRequest();
}

} // namespace Terminal
