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

protected:
    void paintEvent(QPaintEvent *e);

private:
    Terminal * m_terminal;
    QPoint pnt_offset;
};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_PLANE_H
