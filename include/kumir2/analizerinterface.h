#ifndef ANALIZER_INTERFACE_H
#define ANALIZER_INTERFACE_H

#include <kumir2-libs/dataformats/ast.h>

#include "analizer_instanceinterface.h"
#include "analizer_sourcefileinterface.h"

#include <QtCore>

class QWidget;

namespace Shared {

class AnalizerInterface {
public:
    /** How editor should handle indents */
    enum IndentsBehaviour {
        HardIndents,    /** Hard indents, so user can't edit text there */
        PythonIndents,  /** Use ident values as recomendations, but user can backpace on it */
        FreeIndents     /** User allowed to fully contol indent values */
    };

    /** What editor should rehighlight on line edit */
    enum SyntaxHighlightBehaviour {
        IndependentLinesHighlight,  /** Each line rehighlighted independently */
        RestLinesHighlight,         /** Rehighlight edited line and rest lines */
        CompleteTextHighlight       /** Rehighlight complete text on any line change */
    };

    virtual bool primaryAlphabetIsLatin() const = 0;
    virtual bool caseInsensitiveGrammatic() const = 0;
    virtual IndentsBehaviour indentsBehaviour() const = 0;
    virtual SyntaxHighlightBehaviour syntaxHighlightBehaviour() const = 0;
    virtual QString languageName() const = 0;
    virtual QWidget * languageQuickReferenceWidget() { return 0; }
    virtual QByteArray asciiLanguageIdentifier() const { return languageName().toLower().toLatin1(); }
    virtual QString defaultDocumentFileNameSuffix() const = 0;

    virtual Analizer::InstanceInterface * createInstance() = 0;
    virtual Analizer::SourceFileInterface * sourceFileHandler() = 0;

};

}

Q_DECLARE_INTERFACE(Shared::AnalizerInterface, "kumir2.Analizer")

#endif
