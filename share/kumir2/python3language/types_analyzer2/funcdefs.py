from .special_callable_types import *


class MethodDef:
    def __init__(self, name, arguments, return_type):
        assert isinstance(name, str)
        assert isinstance(arguments, list)
        assert isinstance(return_type, TypeDef) or return_type is None
        self.name = name
        self.arguments = arguments
        self.return_type = return_type
        self.static_method = False
        self.parent_table = None
        self.ast_visitor = None
        self.ast_node = None

    def copy(self):
        arguments = [x.copy() for x in self.arguments]
        if self.return_type is not None:
            return_type = self.return_type.copy()
        else:
            return_type = None
        m = MethodDef(self.name, arguments, return_type)
        m.ast_visitor, m.ast_node = self.ast_visitor, self.ast_node
        return m

    @staticmethod
    def from_typed_callable(source: TypedCallable):
        name = repr(source)
        arguments = []
        for i, t in enumerate(source.arg_types):
            ad = ArgumentDef(inspect.Parameter("arg_{}".format(i + 1), inspect.Parameter.POSITIONAL_OR_KEYWORD), t)
            ad.arg_number = i + 1
            arguments.append(ad)
        return_type = source.return_type
        method = MethodDef(name, arguments, return_type)
        method.scope = method
        return method

    def __hash__(self):
        r = repr(self)
        return hash(r)

    def __eq__(self, other):
        if not isinstance(other, MethodDef):
            return False
        return self.name == other.name and len(self.arguments) == len(other.arguments)

    def __str__(self):
        return self.name + "(" + ", ".join([str(x) for x in self.arguments]) + ") -> " + str(self.return_type)

    def to_callable_type(self):
        arg_types = [x.typedef for x in self.arguments]
        rtype = self.return_type
        return TypedCallable(rtype, arg_types)
