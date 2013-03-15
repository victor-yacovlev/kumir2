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
    virtual struct EditorComponent newDocument(const QString & analizerName,
            const QString & documentDir,
            bool initiallyNotSaved
            ) = 0;
    virtual void closeDocument(int documentId) = 0;
    virtual int analizerDocumentId(int editorDocumentId) const = 0;
    virtual bool hasUnsavedChanges(int documentId) const = 0;
    virtual void setDocumentChangesSaved(int documentId) = 0;
    virtual QString loadDocument(int documentId, const QString & fileName) = 0;
    virtual QString saveDocument(int documentId, const QString & fileName) = 0;
    virtual AnalizerInterface * analizer(int documentId) = 0;
    virtual quint32 errorsLinesCount(int documentId) const = 0;
    virtual void highlightLineGreen(int documentId, int lineNo) = 0;
    virtual void highlightLineRed(int documentId, int lineNo) = 0;
    virtual void unhighlightLine(int documentId) = 0;
    virtual void appendMarginText(int documentId, int lineNo, const QString & text) = 0;
    virtual void setMarginText(int documentId, int lineNo, const QString & text) = 0;
    virtual void clearMargin(int documentId) = 0;
    virtual void clearMargin(int documentId, int fromLine, int toLine) = 0;
    virtual void ensureAnalized(int documentId) = 0;
    virtual QByteArray saveState(int documentId) = 0;
    virtual void restoreState(int documentId, const QByteArray & data) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::EditorInterface, "kumir2.editor")

#endif
