#ifndef EDITOR_INTERFACE
#define EDITOR_INTERFACE

#include "interfaces/analizerinterface.h"
#include "interfaces/editor_instanceinterface.h"
#include "interfaces/analizer_sourcefileinterface.h"
#include "dataformats/kumfile.h"

namespace Shared
{

class EditorInterface
{
public:
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
