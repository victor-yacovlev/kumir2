#ifndef EDITOR_INTERFACE
#define EDITOR_INTERFACE

#include "interfaces/analizerinterface.h"
#include "interfaces/editor_instanceinterface.h"
#include "dataformats/kumfile.h"

namespace Shared
{

class EditorInterface
{
public:
    virtual Editor::InstanceInterface * loadDocument(
            QIODevice * device,
            const QString & fileNameSuffix = "",
            const QString & sourceEncoding = "",
            const QUrl & sourceUrl = QUrl()
            ) /* throws QString */ = 0;

    virtual Editor::InstanceInterface * loadDocument(
            const QString & fileName) /* throws QString */ = 0;

    virtual Editor::InstanceInterface * loadDocument(
            const KumFile::Data &data) /* throws QString */ = 0;

    virtual Editor::InstanceInterface * newDocument(
            const QString & canonicalLanguageName = "",
            const QString & documentDir = "") = 0;
};

}

Q_DECLARE_INTERFACE(Shared::EditorInterface, "kumir2.editor")

#endif
