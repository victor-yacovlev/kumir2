#ifndef EDITOR_INTERFACE
#define EDITOR_INTERFACE

#include "interfaces/analizerinterface.h"

namespace Shared
{

struct EditorComponent {
    int id;
    class QWidget * widget;
    QList<class QMenu*> menus;
    QList<class QAction*> toolbarActions;
    QList<class QWidget*> statusbarWidgets;
};

class EditorInterface
{
public:
    virtual struct EditorComponent newDocument(const QString & analizerName = "Analizer",
                                                     const QString & initialText = "") = 0;
    virtual void closeDocument(int documentId) = 0;
    virtual bool hasUnsavedChanges(int documentId) const = 0;
    virtual void setDocumentChangesSaved(int documentId) = 0;
    virtual QString saveDocument(int documentId, const QString & fileName) = 0;
    virtual AnalizerInterface * analizer(int documentId) = 0;
    virtual quint32 errorsCount(int documentId) const = 0;
};

}

Q_DECLARE_INTERFACE(Shared::EditorInterface, "kumir2.editor")

#endif
