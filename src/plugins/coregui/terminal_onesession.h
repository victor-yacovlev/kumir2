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
    inline QString fileName() const { return fileName_; }
    inline QDateTime startTime() const { return startTime_; }
    inline QDateTime endTime() const { return endTime_; }
    inline int fixedWidth() const { return fixedWidth_; }
    void draw(QPainter * p, int realWidth);
    void triggerTextSelection(const QPoint & fromPos, const QPoint & toPos);
    void clearSelection();
    inline void setFont(const QFont &font) { font_ = font; }
    inline QFont font() const { return font_; }
    int widthInChars(int realWidth) const;
    bool hasSelectedText() const;
    QString selectedText() const;
    bool isEditable() const;
public slots:
    void output(const QString & text, const CharSpec cs);
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
    QPoint cursorPositionByVisiblePosition(const QPoint & pos) const;
    QString headerText() const;
    QString footerText() const;
    QFont utilityFont() const;
    void timerEvent(QTimerEvent * e);
    QSize charSize() const;
    QWidget * parent_;
    QStringList lines_;
    QList<LineProp> props_;
    QString fileName_;
    QDateTime startTime_;
    QDateTime endTime_;
    QString inputFormat_;
    int fixedWidth_;
    QFont font_;
    int inputLineStart_;
    int inputPosStart_;
    int inputCursorPosition_;
    bool inputCursorVisible_;
    int timerId_;

};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_ONESESSION_H
