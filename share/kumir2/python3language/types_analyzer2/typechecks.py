from .typedefs import TypeDef
from .special_pseudo_types import *
from .special_callable_types import *


def type_can_implicit_cast(provided, required) -> bool:
    if provided.name == "int":
        if required.name == "float" or required.name == "complex":
            return True
    else:
        return False


def type_can_be_matched(provided, required):
    assert isinstance(required, TypeDef)
    if required.name == "object":
        return True  # can be matched by anything
    if isinstance(required, AnyType):
        return True
    if type_can_implicit_cast(provided, required):
        return True
    if getattr(required, "from_placeholder", False):
        return True  # value was constructed to math this type
    if isinstance(required, AnyOf):
        return any([type_can_be_matched(provided, x) for x in required.variants])

    matched_by_name = provided.name == required.name

    if provided == required:
        return True  # exact match

    for super in provided.supertypes:
        if super == required:
            return True  # supertype match
        if super.name == required.name:
            matched_by_name = True

    if matched_by_name and provided.is_parametrizable() and required.is_parametrizable():
        if provided.keytype is None and required.keytype is None:
            key_match = True  # key value has no meaning
        elif provided.keytype is None and required.keytype is not None:
            key_match = True  # parametrized type is a 'subclass' of not parametrized
        elif provided.keytype is not None and required.keytype is not None:
            key_match = type_can_be_matched(provided.keytype, required.keytype)
        else:
            key_match = False

        # The same for value type
        if provided.valuetype is None and required.valuetype is None:
            value_match = True
        elif provided.valuetype is None and required.valuetype is not None:
            value_match = True
        elif provided.valuetype is not None and required.valuetype is not None:
            value_match = type_can_be_matched(provided.valuetype, required.valuetype)
        else:
            value_match = False

        if key_match and value_match:
            return True

    if isinstance(provided, TypedCallable) and isinstance(required, TypedCallable):
        if len(provided.arg_types) == len(required.arg_types):
            rtype_match = type_can_be_matched(provided.return_type, required.return_type)
            args_match = all(
                [type_can_be_matched(x.typedef, y.typedef) for x, y in zip(provided.arg_types, required.arg_types)])
            if rtype_match and args_match:
                return True

    return False
