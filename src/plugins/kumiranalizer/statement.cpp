#include "statement.h"

namespace KumirAnalizer {

Statement::Statement()
{
    realLineNumber = -1;
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

}
