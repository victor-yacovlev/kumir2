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
    void setSettings(QSettings * s);
    QList<QMenu*> menuActions();
    QList<QWidget*> statusbarWidgets();
    void setText(const QString &text);
    void setDocumentId(int id);
    QString text() const;
    const class TextCursor * cursor() const;
    const class TextDocument * document() const;
    const Shared::AnalizerInterface * analizer() const;
    Shared::AnalizerInterface * analizer();
    class TextCursor * cursor();
    class TextDocument * document();
    bool isModified() const;
    void appendMarginText(int lineNo, const QString & text);
    void setMarginText(int lineNo, const QString & text);
    void clearMarginText();
    void clearMarginText(int fromLine, int toLine);
    void setNotModified();
    void checkForClean();
    void lock();
    void unlock();
    void setLineHighlighted(int lineNo, const QColor & color);
    void ensureAnalized();
    QByteArray saveState() const;
    void restoreState(const QByteArray &data);
    void unsetAnalizer();
public slots:
    void undo();
    void redo();
signals:
    void urlsDragAndDropped(const QList<QUrl> &);
    void documentCleanChanged(bool v);
private:
    void focusInEvent(QFocusEvent *e);
    class EditorPrivate * d;


};

QDataStream & operator<< (QDataStream & stream, const Editor & editor);
QDataStream & operator>> (QDataStream & stream, Editor & editor);

} // namespace Editor

#endif // EDITOR_H
