#ifndef EDITOR_H
#define EDITOR_H

#include <QtGui>
#include "interfaces/visualcomponent.h"
#include "interfaces/analizerinterface.h"

namespace Editor {

class Editor
        : public Shared::VisualComponent
{
    Q_OBJECT
public:
    explicit Editor(QSettings * settings = 0, Shared::AnalizerInterface * analizer = 0, int documentId = 0, QWidget *parent = 0);
    ~Editor();
    QList<QAction*> toolbarActions();
    QList<Shared::MenuActionsGroup> menuActions();
    void setText(const QString &text);
    QString text() const;
    bool isModified() const;
    void setNotModified();

private:
    class EditorPrivate * d;

};

} // namespace Editor

#endif // EDITOR_H
