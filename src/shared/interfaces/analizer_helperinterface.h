#ifndef ANALIZER_HELPERINTERFACE_H
#define ANALIZER_HELPERINTERFACE_H

#include "dataformats/ast.h"
#include "error.h"

#include <QString>
#include <QList>

#include <string>

namespace Shared { namespace Analizer {

typedef QPair<QString,quint32> TextAppend;

struct Suggestion {
    enum Kind { Local, Global, Algorithm, BuiltinModule, KumirModule, Statement, SecondaryKeyword, Other };
    inline Suggestion() { kind = Other; showOnlyInFullList = false; }
    QString value;
    QString description;
    bool showOnlyInFullList;
    Kind kind;
};

class HelperInterface {
public:
    virtual QList<Suggestion> suggestAutoComplete(int lineNo, const QString & before, const QString & after) const = 0;
    virtual TextAppend closingBracketSuggestion(int lineNo) const = 0;
    virtual QStringList importModuleSuggestion(int lineNo) const = 0;
    virtual QStringList imports() const = 0;
    virtual QString createImportStatementLine(const QString &importName) const = 0;
    virtual QString suggestFileName() const = 0;
    virtual void connectSignalImportsChanged(
            QObject * receiver,
            const char * slot /* void (QStringList localizedNames) */
            ) { Q_UNUSED(receiver); Q_UNUSED(slot); }
};


}}


Q_DECLARE_INTERFACE(Shared::Analizer::HelperInterface,
                    "kumir2.Analizer.HelperInterface")


#endif // ANALIZER_HELPERINTERFACE_H
