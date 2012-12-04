#include "ast_type.h"

namespace AST {

bool Type::operator ==(const Type & other) const {
    if (kind!=TypeUser)
        return kind == other.kind;
    else
        return name == other.name;
}

bool Type::operator !=(const Type & other) const {
    if (kind==TypeUser)
        return name!=other.name;
    else
        return kind!=other.kind;
}

bool Type::operator==(const VariableBaseType & otherType) const {
    return kind!=TypeUser && kind==otherType;
}

}
