#include "statement.h"

namespace KumirAnalizer {

TextStatement::TextStatement()
    : type(Shared::LxTypeEmpty)
    , conditionalIndex(0)
{
}

bool TextStatement::hasError() const
{
    for (int i=0; i<data.size(); i++)
        if (!data[i]->error.isEmpty())
            return true;
    return false;
}

void TextStatement::setError(const QString &err,
                         const Lexem::ErrorStage stage,
                         const Lexem::ErrorRaisePosition raise)
{
    for (int i=0; i<data.size(); i++) {
        data[i]->error = err;
        data[i]->errorStage = stage;
        data[i]->errorRaisePosition = raise;
    }
}

}
