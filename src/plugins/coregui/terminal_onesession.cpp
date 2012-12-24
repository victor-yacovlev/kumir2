#include "terminal_onesession.h"
#include "terminal.h"
#include "stdlib/kumirstdlib.hpp"
namespace Terminal {

static const int bodyPadding = 4;
static const int headerPadding = 4;
static const int lineWidth = 1;
static const int shadowOffset = 4;

OneSession::OneSession(int fixedWidth, const QString & fileName, QWidget * parent)
    : QObject(parent)
    , s_fileName(fileName)
    , i_fixedWidth(fixedWidth)
{
    i_inputLineStart = i_inputPosStart = -1;
    i_inputCursorPosition = -1;
    i_timerId = -1;
    b_inputCursorVisible = false;
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
    m_startTime = QDateTime::currentDateTime();
}

QSize OneSession::charSize() const
{

    QFontMetrics fm(font());
    return QSize(fm.width('m'), fm.height());
}

int OneSession::widthInChars(int realWidth) const
{
    QSize atom = charSize();
    return (realWidth-2*bodyPadding-lineWidth-shadowOffset)/atom.width();
}

QSize OneSession::visibleSize(int realWidth) const
{
    QSize atom = charSize();
    QSize body;
    if (i_fixedWidth==-1) {
        int maxLineLength = 0;
        int linesCount = 0;
        int maxChars = widthInChars(realWidth);
        foreach (const QString & s, m_lines) {
            maxLineLength = qMax(s.length(), maxLineLength);
            linesCount += 1+(s.length())/maxChars;
        }
        body = QSize(maxLineLength*atom.width(), linesCount*atom.height());
    }
    else {
        int maxLineLength = i_fixedWidth;
        int linesCount = m_lines.size();
        body = QSize(maxLineLength*atom.width(), linesCount*atom.height());
    }

    body.setWidth(body.width() + 2 * bodyPadding);
    body.setHeight(body.height() + 2 * bodyPadding);
    const int headerHeight = 2 * headerPadding + atom.height();
    const int footerHeight = m_endTime.isValid()? headerHeight : 0;
    return QSize(body.width()+lineWidth+shadowOffset,
                 body.height()+headerHeight+footerHeight+lineWidth+shadowOffset);
}

QString OneSession::plainText(bool footer_header) const
{
    const QString body = m_lines.join("\n");
    const QString header = tr(">> %1:%2:%3 - %4 - Process started")
            .arg(m_startTime.time().hour(), 2, 10, QChar(' '))
            .arg(m_startTime.time().minute(), 2, 10, QChar('0'))
            .arg(m_startTime.time().second(), 2, 10, QChar('0'))
            .arg(s_fileName);
    const QString footer = m_endTime.isValid()
            ? tr(">> %1:%2:%3 - %4 - Process finished")
              .arg(m_endTime.time().hour(), 2, 10, QChar(' '))
              .arg(m_endTime.time().minute(), 2, 10, QChar('0'))
              .arg(m_endTime.time().second(), 2, 10, QChar('0'))
              .arg(s_fileName)
            : "";
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
    if (i_fixedWidth==-1)
        sz.setWidth(realWidth);
    else {
        p->translate((realWidth-sz.width())/2, 0);
    }

//    p->setPen(Qt::NoPen);
//    p->setBrush(QColor(Qt::black));
//    p->drawRect(shadowOffset, shadowOffset,
//                sz.width()-shadowOffset-lineWidth,
//                sz.height()-shadowOffset-lineWidth);
//    p->setPen(QPen(QColor(Qt::black),lineWidth));
//    p->setBrush(QColor(Qt::white));
//    p->drawRect(0,0,sz.width()-shadowOffset, sz.height()-shadowOffset);

    QFont smallFont = m_font;
    smallFont.setPointSize(m_font.pointSize()-2);
    smallFont.setItalic(true);
    const QString header = tr(">> %1:%2:%3 - %4 - Process started")
            .arg(m_startTime.time().hour(), 2, 10, QChar(' '))
            .arg(m_startTime.time().minute(), 2, 10, QChar('0'))
            .arg(m_startTime.time().second(), 2, 10, QChar('0'))
            .arg(s_fileName);
    const QString footer = m_endTime.isValid()
            ? tr(">> %1:%2:%3 - %4 - Process finished")
              .arg(m_endTime.time().hour(), 2, 10, QChar(' '))
              .arg(m_endTime.time().minute(), 2, 10, QChar('0'))
              .arg(m_endTime.time().second(), 2, 10, QChar('0'))
              .arg(s_fileName)
            : "";
    p->setFont(smallFont);
    p->setPen(QColor(Qt::darkGray));
    p->drawText(headerPadding, headerPadding+QFontMetrics(smallFont).height(), header);
    int x = bodyPadding;
    int y = 2 * headerPadding + 2 * atom.height() + bodyPadding;

    if (i_inputCursorPosition!=-1 && b_inputCursorVisible && (qobject_cast<QWidget*>(parent()))->hasFocus()) {
//        int cursorRow = m_lines.size()-1-i_inputLineStart;
        int cursorRow = i_inputLineStart;
//        int cursorCol = cursorRow>0? 0 : i_inputCursorPosition+i_inputPosStart;
        int cursorCol = i_inputCursorPosition+i_inputPosStart;
        QRect cursorRect(x + cursorCol * atom.width(),
                         y + cursorRow * atom.height(),
                         atom.width(),
                         2
                         );
        p->setPen(Qt::NoPen);
        p->setBrush(QColor(Qt::black));
        p->drawRect(cursorRect);
    }

    Q_ASSERT(m_lines.size()==m_props.size());
    p->setFont(m_font);
    for (int i=0; i<m_lines.size(); i++) {
        const QString text = m_lines[i];
        const LineProp prop = m_props[i];
        Q_ASSERT(text.length()==prop.size());
        for (int j=0; j<text.length(); j++) {
            if (prop[j]==CS_Input)
                p->setPen(QColor(Qt::blue));
            else if (prop[j]==CS_Error || prop[j]==CS_InputError)
                p->setPen(QColor(Qt::red));
            else
                p->setPen(QColor(Qt::black));
            p->drawText(x,y,QString(text[j]));
            x += atom.width();
            if (i_fixedWidth==-1) {
                int lw = widthInChars(realWidth);
                if (j % lw==0 && j>0) {
                    x = bodyPadding;
                    y += atom.height();
                }
            }
        }
        y += atom.height();
        x = bodyPadding;
    }



    if (!footer.isEmpty()) {
        p->setFont(smallFont);
        p->setPen(QColor(Qt::darkGray));
        p->drawText(headerPadding,
                    sz.height()-lineWidth-shadowOffset-headerPadding,
                    footer);
        p->setPen(QColor(Qt::darkGray));
        p->drawLine(bodyPadding, sz.height()-headerPadding, sz.width()-bodyPadding, sz.height()-headerPadding);
    }
    p->restore();
}

void OneSession::output(const QString &text)
{
    int curLine = m_lines.size()-1;
    int curCol = m_lines.isEmpty()? 0 : m_lines[curLine].length();
    for (int i=0; i<text.length(); i++) {
        bool newLine = curLine<0 || text[i]=='\n' || ( i_fixedWidth!=-1 && curCol>=i_fixedWidth );
        if (newLine) {
            m_lines.append("");
            m_props.append(LineProp());
            curLine ++;
            curCol = 0;
        }
        if (text[i].unicode()>=32) {
            m_lines[curLine] += text[i];
            m_props[curLine].append(CS_Output);
            Q_ASSERT(m_props[curLine].last()==CS_Output);
        }
    }
    emit updateRequest();
}

void OneSession::input(const QString &format)
{
    s_inputFormat = format;
    if (m_lines.isEmpty()) {
        m_lines << "";
        m_props << LineProp();
    }
    i_inputLineStart = m_lines.size()-1;
    i_inputPosStart = 0;
    if (!m_lines.isEmpty()) {
        i_inputPosStart = m_lines.last().length();
    }
    i_inputCursorPosition = 0;
    b_inputCursorVisible = true;
//    StdLib::GenericInputOutput::instance()->doInput(format);
    QString msg = tr("INPUT ");
    QStringList fmts = format.split(";", QString::SkipEmptyParts);
    for (int i=0; i<fmts.size(); i++) {
        if (i>0) {
            msg += ", ";
        }
        if (fmts[i][0]=='s')
            msg += tr("string");
        if (fmts[i][0]=='i')
            msg += tr("integer");
        if (fmts[i][0]=='r')
            msg += tr("real");
        if (fmts[i][0]=='c')
            msg += tr("charect");
        if (fmts[i][0]=='b')
            msg += tr("boolean");
    }
    msg += ".";
    emit message(msg);
    i_timerId = startTimer(QApplication::cursorFlashTime()/2);
    emit updateRequest();
}

void OneSession::timerEvent(QTimerEvent *e)
{
    b_inputCursorVisible = !b_inputCursorVisible;
    emit updateRequest();
    e->accept();
}

void OneSession::changeCursorPosition(quint16 pos)
{
    i_inputCursorPosition = pos;
    b_inputCursorVisible = true;
    emit updateRequest();
}

void OneSession::changeInputText(const QString &text)
{
    m_lines = m_lines.mid(0, i_inputLineStart+1);
    m_props = m_props.mid(0, i_inputLineStart+1);
    if (!m_lines.isEmpty()) {
        m_lines[m_lines.size()-1] = m_lines[m_lines.size()-1].mid(0,i_inputPosStart);
        m_props[m_props.size()-1] = m_props[m_props.size()-1].mid(0,i_inputPosStart);
    }
    int curCol = i_inputPosStart;
    int curLine = m_lines.size()-1;
    for (int i=0; i<text.length(); i++) {
        bool newLine = curLine<0 || ( i_fixedWidth!=-1 && curCol>=i_fixedWidth );
        if (newLine) {
            m_lines.append("");
            m_props.append(LineProp());
            curLine ++;
            curCol = 0;
        }
        if (text[i].unicode()>=32) {
            m_lines[curLine] += text[i];
            m_props[curLine].append(CS_Input);
            Q_ASSERT(m_props[curLine].last()==CS_Input);
        }
    }
    emit updateRequest();
}

void OneSession::tryFinishInput()
{
    QString text;
    for (int i=i_inputLineStart; i<m_lines.size(); i++) {
        if (i==i_inputLineStart)
            text += m_lines[i].mid(i_inputPosStart);
        else
            text += m_lines[i];
    }
    QVector<bool> errmask = QVector<bool>(text.length(), false);
    QVariantList result;
    Kumir::IO::InputStream stream(text.toStdWString());
    const QStringList & formats = s_inputFormat.split(";", QString::SkipEmptyParts);
    for (int i=0; i<formats.size(); i++) {
        char type = formats[i][0].unicode();
        const Kumir::String format = formats[i].mid(1).toStdWString();
        switch (type) {
        case 'i': {
            int value = Kumir::IO::readInteger(stream);
            result << value;
            break;
        }
        case 'r': {
            double value = Kumir::IO::readReal(stream);
            result << value;
            break;
        }
        case 'b': {
            bool value = Kumir::IO::readBool(stream);
            result << value;
            break;
        }
        case 'c': {
            Kumir::Char value = Kumir::IO::readChar(stream);
            result << QChar(value);
            break;
        }
        case 's': {
            Kumir::String value = Kumir::IO::readString(stream);
            result << QString::fromStdWString(value);
            break;
        }
        case 'n': {
            Kumir::IO::readLine(stream);
            result << QChar('\n');
            break;
        }
        default:
            break;
        }
        if (stream.hasError())
            break;
    }
    if (stream.hasError()) {
        Kumir::String errorText;
        int errorStart, errorLength;
        stream.getError(errorText, errorStart, errorLength);
        emit message(QString::fromStdWString(errorText));
        for (int i=0; i<errorLength; i++) {
            errmask[errorStart+i] = true;
        }
        int curLine = i_inputLineStart;
        int curCol = i_inputPosStart;
        for (int i=0; i<text.length(); i++) {
            bool newLine = curLine<0 || ( i_fixedWidth!=-1 && curCol>=i_fixedWidth );
            if (newLine) {
                curLine ++;
                curCol = 0;
            }
            else {
                Q_ASSERT(m_props[curLine][curCol]==CS_Input || m_props[curLine][curCol]==CS_InputError);
                m_props[curLine][curCol] = errmask[i]? CS_InputError : CS_Input;
                Q_ASSERT(m_props[curLine][curCol]==CS_Input || m_props[curLine][curCol]==CS_InputError);
                curCol ++;
            }
        }
        emit updateRequest();
    }
    else {
        i_inputLineStart = i_inputPosStart = i_inputCursorPosition = -1;
        output("\n");
        emit message("");
        emit inputDone(result);
        emit updateRequest();
    }
}

void OneSession::error(const QString &message)
{
    m_lines.append(tr("RUNTIME ERROR: %1").arg(message));
    m_props.append(LineProp());
    for (int i=0; i<m_lines.last().size(); i++) {
        m_props[m_props.size()-1] << CS_Error;
    }
    m_endTime = QDateTime::currentDateTime();
    emit updateRequest();
}

void OneSession::terminate()
{
    m_endTime = QDateTime::currentDateTime();
    if (i_timerId!=-1)
        killTimer(i_timerId);
    i_inputLineStart = i_inputPosStart = i_inputCursorPosition = -1;
    emit updateRequest();
}

void OneSession::finish()
{
    m_endTime = QDateTime::currentDateTime();
    emit updateRequest();
}

} // namespace Terminal
