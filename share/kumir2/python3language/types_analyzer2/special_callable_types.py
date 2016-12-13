from .typedefs import TypeDef
from .special_pseudo_types import SpecialPseudoType, AnyType
from .argumentdef import ArgumentDef

import inspect


class TypedCallable(SpecialPseudoType):
    name = "_typed_callable"

    def __init__(self, return_type, arg_types):
        super().__init__(TypedCallable.name)
        assert isinstance(return_type, TypeDef)
        assert isinstance(arg_types, list)
        self.return_type = return_type
        self.arg_types = []
        for index, arg_type in enumerate(arg_types):
            if isinstance(arg_type, ArgumentDef):
                self.arg_types.append(arg_type)
            elif isinstance(arg_type, TypeDef):
                arg_name = "a_" + str(index + 1)
                param = inspect.Parameter(arg_name, inspect.Parameter.POSITIONAL_OR_KEYWORD)
                arg = ArgumentDef(param, arg_type)
                self.arg_types.append(arg)
        self.method = None

    def __repr__(self):
        return "_typed_callable<(" + \
               ", ".join([repr(x) for x in self.arg_types]) + \
               ") -> " + repr(self.return_type) + ">"


class Lambda(TypedCallable):
    name = "_lambda"

    def __init__(self, arg_names, node):
        args = []
        for name in arg_names:
            param = inspect.Parameter(name, inspect.Parameter.POSITIONAL_OR_KEYWORD)
            typee = AnyType()
            arg = ArgumentDef(param, typee)
            args.append(arg)
        super().__init__(AnyType(), args)
        self.ast_node = node
        self.resolved = False

    def __repr__(self):
        return "_lambda<(" + \
               ", ".join([repr(x) for x in self.arg_types]) + \
               ") -> " + repr(self.return_type) + ">"
