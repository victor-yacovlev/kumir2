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

signals:

public slots:
    void updateScrollBars();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void wheelEvent(QWheelEvent *e);
    QPoint offset() const;

private:
    Terminal * m_terminal;
};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_PLANE_H
