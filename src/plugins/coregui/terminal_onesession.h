#ifndef TERMINAL_TERMINAL_ONESESSION_H
#define TERMINAL_TERMINAL_ONESESSION_H

#include <QtCore>
#include <QtGui>

#include <deque>

namespace Terminal {

enum CharSpec {
    CS_Output       = 0x00,
    CS_Input        = 0x01,
    CS_InputError   = 0x11,
    CS_Error        = 0x10
};

typedef QVector<CharSpec> LineProp;

struct VisibleLine {
    LineProp prop;
    QString text;
    bool * endSelected;
    size_t from;
    size_t to;

    inline explicit VisibleLine(const QString &tx, const LineProp &lp, bool * es, size_t f, size_t t)
        : text(tx), prop(lp), endSelected(es), from(f), to(t) {}

    inline VisibleLine& operator=(const VisibleLine & other)
    {
        *this = other;
        return *this;
    }
};

class OneSession
        : public QObject
{
    Q_OBJECT
public:
    OneSession(int fixedWidth, const QString & fileName, QWidget * parent);
    QSize minimumSizeHint() const;
    QSize visibleSize() const;
    QString plainText(bool footer_header) const;
    QString fileName() const;
    inline QDateTime startTime() const { return startTime_; }
    inline QDateTime endTime() const { return endTime_; }
    inline int fixedWidth() const { return fixedWidth_; }
    void draw(QPainter &p, const QRect & dirtyRect) const;
    void drawInputRect(QPainter &p, const uint mainTextY) const;
    uint drawUtilityText(QPainter &p,
                         const QString & text,
                         const LineProp & prop,
                         const QPoint & topLeft
                         ) const;
    uint drawMainText(QPainter &p, const QPoint & topLeft, const QRect & dirtyRect) const;
    void drawCursor(QPainter &p) const;
    void triggerTextSelection(const QPoint & fromPos, const QPoint & toPos);
    void clearSelection();
    inline void setFont(const QFont &font) { font_ = font; }
    inline QFont font() const { return font_; }
    int widthInChars(int realWidth) const;
    bool hasSelectedText() const;
    QString selectedText() const;
    QString selectedRtf() const;
    void selectAll();
    bool isEditable() const;
    void relayout(uint realWidth, size_t fromLine, bool headerAndFooter);
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
    std::deque<LineProp> props_;
    std::deque<VisibleLine> visibleLines_;
    mutable uint maxLineLength_; // cached to faster "relayout" method
    QList<bool> selectedLineEnds_;
    QRect mainTextRegion_;
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
    QString visibleHeader_;
    QString visibleFooter_;
    LineProp headerProp_;
    LineProp footerProp_;
    QRect headerRect_;
    QRect footerRect_;
    QScopedPointer<QMutex> relayoutMutex_;
};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_ONESESSION_H
