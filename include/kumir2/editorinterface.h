#ifndef EDITOR_INTERFACE
#define EDITOR_INTERFACE

#include "analizerinterface.h"
#include "editor_instanceinterface.h"
#include "analizer_sourcefileinterface.h"
#include <kumir2-libs/dataformats/kumfile.h>

namespace Shared
{

class EditorInterface
{
public:
    virtual QFont defaultEditorFont() const = 0;

    virtual Editor::InstanceInterface * loadDocument(
            QIODevice * device,
            const QString & fileNameSuffix,
            const QString & sourceEncoding,
            const QUrl & sourceUrl, QString * error
            ) = 0;

    virtual Editor::InstanceInterface * loadDocument(
            const QString & fileName, QString * error) = 0;

    virtual Editor::InstanceInterface * loadDocument(
            const Analizer::SourceFileInterface::Data &data, QString * error) = 0;

    virtual Editor::InstanceInterface * newDocument(
            const QString & canonicalLanguageName = "",
            const QString & documentDir = "") = 0;
};

}

Q_DECLARE_INTERFACE(Shared::EditorInterface, "kumir2.editor")

#endif
