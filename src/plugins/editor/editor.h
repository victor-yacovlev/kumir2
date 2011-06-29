#ifndef EDITOR_H
#define EDITOR_H

#include <QtGui>
#include "interfaces/analizerinterface.h"

namespace Editor {

class Editor
        : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QSettings * settings = 0, Shared::AnalizerInterface * analizer = 0, int documentId = 0, QWidget *parent = 0);
    ~Editor();
    QList<QAction*> toolbarActions();
    QList<QMenu*> menuActions();
    QList<QWidget*> statusbarWidgets();
    void setText(const QString &text);
    void setDocumentId(int id);
    QString text() const;
    bool isModified() const;
    void setNotModified();
    void lock();
    void unlock();
    void setLineHighlighted(int lineNo, const QColor & color);
signals:
    void urlsDragAndDropped(const QList<QUrl> &);
private:
    void focusInEvent(QFocusEvent *e);
    class EditorPrivate * d;


};

} // namespace Editor

#endif // EDITOR_H
