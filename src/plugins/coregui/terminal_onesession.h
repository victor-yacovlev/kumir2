#ifndef TERMINAL_TERMINAL_ONESESSION_H
#define TERMINAL_TERMINAL_ONESESSION_H

#include <QtCore>
#include <QtGui>

namespace Terminal {

enum CharSpec {
    CS_Output,
    CS_Input,
    CS_Error
};

typedef QList<CharSpec> LineProp;

class OneSession
        : public QObject
{
    Q_OBJECT
public:
    OneSession(int fixedWidth, const QString & fileName, QObject * parent);
    QSize visibleSize() const;
    QString plainText() const;
    inline QString fileName() const { return s_fileName; }
    inline QDateTime startTime() const { return m_startTime; }
    inline QDateTime endTime() const { return m_endTime; }
    inline int fixedWidth() const { return i_fixedWidth; }
    void draw(QPainter * p, int realWidth);
    inline void setFont(const QFont &font) { m_font = font; }
    inline QFont font() const { return m_font; }
public slots:
    void output(const QString & text);
    void input(const QString & format);
    void error(const QString & message);
    void finish();
    void terminate();
signals:
    void updateRequest();
private:
    QSize charSize() const;
    QStringList m_lines;
    QList<LineProp> m_props;
    QString s_fileName;
    QDateTime m_startTime;
    QDateTime m_endTime;
    int i_fixedWidth;
    QFont m_font;

};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_ONESESSION_H
