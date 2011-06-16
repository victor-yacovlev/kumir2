#include "terminal_onesession.h"
#include "terminal.h"

namespace Terminal {

static const int bodyPadding = 4;
static const int headerPadding = 4;
static const int lineWidth = 1;
static const int shadowOffset = 4;

OneSession::OneSession(int fixedWidth, const QString & fileName, QObject * parent)
    : QObject(parent)
    , s_fileName(fileName)
    , i_fixedWidth(fixedWidth)
{
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

QSize OneSession::visibleSize() const
{
    QSize atom = charSize();
    QSize body;
    if (i_fixedWidth==-1) {
        int maxLineLength = 0;
        foreach (const QString & s, m_lines) {
            maxLineLength = qMax(s.length(), maxLineLength);
        }
        int linesCount = m_lines.size();
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

QString OneSession::plainText() const
{
    const QString body = m_lines.join("\n");
    const QString header = tr(">> %1:%2:%3 - %4 - Process started")
            .arg(m_startTime.time().hour())
            .arg(m_startTime.time().minute())
            .arg(m_startTime.time().second())
            .arg(s_fileName);
    const QString footer = m_endTime.isValid()
            ? tr(">> %1:%2:%3 - %4 - Process finished")
              .arg(m_startTime.time().hour())
              .arg(m_startTime.time().minute())
              .arg(m_startTime.time().second())
              .arg(s_fileName)
            : "";
    return header + "\n" + body + "\n" + footer;
}

void OneSession::draw(QPainter *p, int realWidth)
{
    const QSize atom = charSize();
    QSize sz = visibleSize();
    if (i_fixedWidth==-1)
        sz.setWidth(realWidth);
    p->save();
    p->setPen(Qt::NoPen);
    p->setBrush(QColor(Qt::black));
    p->drawRect(shadowOffset, shadowOffset,
                sz.width()-shadowOffset-lineWidth,
                sz.height()-shadowOffset-lineWidth);
    p->setPen(QPen(QColor(Qt::black),lineWidth));
    p->setBrush(QColor(Qt::white));
    p->drawRect(0,0,sz.width()-shadowOffset, sz.height()-shadowOffset);

    QFont smallFont = m_font;
    smallFont.setPointSize(m_font.pointSize()-2);
    smallFont.setItalic(true);
    const QString header = tr(">> %1:%2:%3 - %4 - Process started")
            .arg(m_startTime.time().hour())
            .arg(m_startTime.time().minute())
            .arg(m_startTime.time().second())
            .arg(s_fileName);
    const QString footer = m_endTime.isValid()
            ? tr(">> %1:%2:%3 - %4 - Process finished")
              .arg(m_startTime.time().hour())
              .arg(m_startTime.time().minute())
              .arg(m_startTime.time().second())
              .arg(s_fileName)
            : "";
    p->setFont(smallFont);
    p->setPen(QColor(Qt::gray));
    p->drawText(headerPadding, headerPadding, header);
    int x = bodyPadding;
    int y = 2 * headerPadding + atom.height() + bodyPadding;

    Q_ASSERT(m_lines.size()==m_props.size());
    p->setFont(m_font);
    for (int i=0; i<m_lines.size(); i++) {
        const QString text = m_lines[i];
        const LineProp prop = m_props[i];
        Q_ASSERT(text.length()==prop.size());
        for (int j=0; j<text.length(); j++) {
            if (prop[j]==CS_Input)
                p->setPen(QColor(Qt::blue));
            else if (prop[j]==CS_Error)
                p->setPen(QColor(Qt::red));
            else
                p->setPen(QColor(Qt::black));
            p->drawText(x,y,QString(text[j]));
            x += atom.width();
        }
        y += atom.height();
    }
    if (!footer.isEmpty()) {
        p->setFont(smallFont);
        p->setPen(QColor(Qt::gray));
        p->drawText(headerPadding,
                    sz.height()-lineWidth-shadowOffset-headerPadding,
                    footer);
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
            m_props[curLine] << CS_Output;
        }
    }
    emit updateRequest();
}

void OneSession::input(const QString &format)
{
    // TODO implement me
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
    emit updateRequest();
}

void OneSession::finish()
{
    m_endTime = QDateTime::currentDateTime();
    emit updateRequest();
}

} // namespace Terminal
