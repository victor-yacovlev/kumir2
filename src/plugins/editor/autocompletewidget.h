#ifndef EDITOR_AUTOCOMPLETEWIDGET_H
#define EDITOR_AUTOCOMPLETEWIDGET_H

#include <QtCore>
#include <QtGui>

namespace Editor {

struct AutoCompleteItem
{
    QString text;
    enum Type { None, Local, Global, Algorhitm } type;
};

class AutoCompleteWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int listViewOffset READ listViewOffset WRITE setListViewOffset)
public:
    explicit AutoCompleteWidget(QWidget *parent = 0);
    int offsetY() const;

    void init(const QFont & font,
              QWidget * focusOnCloseWidget,
              const QString & source,
              const QStringList & algorhitms,
              const QStringList & locals,
              const QStringList & globals);

    inline void setListViewOffset(int v) { i_listViewOffset = v; update(); }
    inline int listViewOffset() const { return i_listViewOffset; }

signals:
    void accepted(const QString & source, const QString & newtext);
public slots:
    void setCurrentIndex(int index);
    void accept();
    void reject();
private:

    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void keyPressEvent(QKeyEvent *e);
    bool event(QEvent *e);


    QList<AutoCompleteItem> l_items;
    int i_currentIndex;
    int i_listViewOffset;

    QPropertyAnimation * m_animation;
    QString s_source;

    QWidget * w_focusOnCloseWidget;

    int i_dY;
    QPoint pnt_mousePress;
    bool b_doubleClicked;

};

} // namespace Editor

#endif // EDITOR_AUTOCOMPLETEWIDGET_H
