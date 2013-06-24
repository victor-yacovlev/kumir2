#ifndef TERMINAL_TERMINAL_PLANE_H
#define TERMINAL_TERMINAL_PLANE_H

#include <QtCore>
#include <QtGui>

namespace Terminal {

class Term;
class OneSession;

class Plane : public QWidget
{
    Q_OBJECT
public:
    explicit Plane(Term * parent);
    QSize minimumSizeHint() const;
    inline void setInputMode(bool v) {
        inputMode_ = v; inputText_ = ""; inputPosition_ = 0;
    }

signals:
    void inputTextChanged(const QString & txt);
    void inputCursorPositionChanged(quint16 pos);
    void inputFinishRequest();

public slots:
    void updateScrollBars();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void contextMenuEvent(QContextMenuEvent *);

    QPoint offset() const;

private slots:
    void selectAll();
    void copyToClipboard();
    void pasteFromClipboard();

private:
    OneSession* sessionByPos(const QPoint & pos) const;
    QRect sessionRect(const OneSession * session) const;
    Term * terminal_;
    bool inputMode_;
    quint16 inputPosition_;
    QString inputText_;
    OneSession* mousePressSession_;
    QPoint mousePressPosition_;
    QAction * actionCopyToClipboard_;
    QAction * actionPasteFromClipboard_;
};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_PLANE_H
