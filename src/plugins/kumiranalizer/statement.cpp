#include "statement.h"

namespace KumirAnalizer {

Statement::Statement()
{
    indentRank = QPoint(0,0);
    type = Shared::LxTypeEmpty;
    statement = 0;
    alg = 0;
    mod = 0;
    conditionalIndex = 0;
}

bool Statement::hasError() const
{
    for (int i=0; i<data.size(); i++)
        if (!data[i]->error.isEmpty())
            return true;
    return false;
}

void Statement::setError(const QString &err,
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
