#ifndef EDITOR_INTERFACE
#define EDITOR_INTERFACE

#include "extensionsystem/visualcomponent.h"
#include "interfaces/analizerinterface.h"

namespace Shared
{

class EditorInterface
{
public:
    virtual QPair<int, ExtensionSystem::VisualComponent*> newDocument(const QString & analizerName = "Analizer",
                                                     const QString & initialText = "") = 0;
    virtual void closeDocument(int documentId) = 0;
    virtual bool hasUnsavedChanges(int documentId) const = 0;
    virtual void setDocumentChangesSaved(int documentId) = 0;
    virtual QString saveDocument(int documentId, const QString & fileName) = 0;
    virtual AnalizerInterface * analizer(int documentId) = 0;
};

}

Q_DECLARE_INTERFACE(Shared::EditorInterface, "kumir2.editor")

#endif
