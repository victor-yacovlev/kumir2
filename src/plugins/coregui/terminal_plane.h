#ifndef TERMINAL_TERMINAL_PLANE_H
#define TERMINAL_TERMINAL_PLANE_H

#include <QtCore>
#include <QtGui>

namespace Terminal {

class Terminal;
class OneSession;

class Plane : public QWidget
{
    Q_OBJECT
public:
    explicit Plane(Terminal * parent);
    inline void setInputMode(bool v) {
        b_inputMode = v; s_inputText = ""; i_inputPosition = 0;
        if (!v) {
            disconnect(this, SIGNAL(inputTextChanged(QString)), 0, 0);
            disconnect(this, SIGNAL(inputCursorPositionChanged(quint16)), 0, 0);
            disconnect(this, SIGNAL(inputFinishRequest()), 0, 0);
        }
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
    QPoint offset() const;

private:
    Terminal * m_terminal;
    bool b_inputMode;
    quint16 i_inputPosition;
    QString s_inputText;
};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_PLANE_H
