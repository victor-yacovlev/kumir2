#ifndef TERMINAL_TERMINAL_ONESESSION_H
#define TERMINAL_TERMINAL_ONESESSION_H

#include <QtCore>
#include <QtGui>

namespace Terminal {

enum CharSpec {
    CS_Output       = 0x00,
    CS_Input        = 0x01,
    CS_InputError   = 0x11,
    CS_Error        = 0x10
};

typedef QList<CharSpec> LineProp;

class OneSession
        : public QObject
{
    Q_OBJECT
public:
    OneSession(int fixedWidth, const QString & fileName, QWidget * parent);
    QSize visibleSize(int realWidth) const;
    QString plainText(bool footer_header) const;
    inline QString fileName() const { return s_fileName; }
    inline QDateTime startTime() const { return m_startTime; }
    inline QDateTime endTime() const { return m_endTime; }
    inline int fixedWidth() const { return i_fixedWidth; }
    void draw(QPainter * p, int realWidth);
    inline void setFont(const QFont &font) { m_font = font; }
    inline QFont font() const { return m_font; }
    int widthInChars(int realWidth) const;
public slots:
    void output(const QString & text);
    void input(const QString & format);
    void error(const QString & message);
    void finish();
    void terminate();
    void tryFinishInput();

    void changeCursorPosition(quint16 pos);
    void changeInputText(const QString & text);

signals:
    void updateRequest();
    void message(const QString & txt);
    void inputDone(const QVariantList &);
private:

    void timerEvent(QTimerEvent * e);
    QSize charSize() const;
    QStringList m_lines;
    QList<LineProp> m_props;
    QString s_fileName;
    QDateTime m_startTime;
    QDateTime m_endTime;
    int i_fixedWidth;
    QFont m_font;
    int i_inputLineStart;
    int i_inputPosStart;
    int i_inputCursorPosition;
    bool b_inputCursorVisible;
    int i_timerId;

};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_ONESESSION_H
