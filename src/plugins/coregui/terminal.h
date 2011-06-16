#ifndef TERMINAL_TERMINAL_H
#define TERMINAL_TERMINAL_H

#include <QtCore>
#include <QtGui>

namespace Terminal {

class Terminal : public QWidget
{
    friend class Plane;
    Q_OBJECT
public:
    explicit Terminal(QWidget *parent = 0);
signals:
    void openTextEditor(const QString & text);
public slots:
    void clear();
    void start(const QString &fileName);
    void finish();
    void terminate();
    void output(const QString & text);
    void error(const QString & message);
    void input(const QString & format);
    void saveLast();
    void saveAll();
    void editLast();

private:
    QList<class OneSession*> l_sessions;
    QScrollBar * sb_vertical;
    QScrollBar * sb_horizontal;
    class Plane * m_plane;
    QAction * a_saveAll;
    QAction * a_saveLast;
    QAction * a_editLast;
    QAction * a_clear;

};

} // namespace Terminal

#endif // TERMINAL_TERMINAL_H
