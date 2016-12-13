from .typedefs import TypeDef
from .special_pseudo_types import SpecialPseudoType


class NthArgType(SpecialPseudoType):
    name = "_nth_arg_type"

    def __init__(self, arg_number):
        super().__init__(NthArgType.name)
        assert isinstance(arg_number, int)
        self.arg_number = arg_number
        self.scope = None

    def __repr__(self):
        return "_nth_arg_type(" + str(self.arg_number) + ")"


class NthArgKeyType(SpecialPseudoType):
    name = "_nth_arg_key_type"

    def __init__(self, arg_number):
        super().__init__(NthArgKeyType.name)
        assert isinstance(arg_number, int)
        self.arg_number = arg_number
        self.scope = None

    def __repr__(self):
        return "_nth_arg_key_type(" + str(self.arg_number) + ")"


class NthArgValueType(SpecialPseudoType):
    name = "_nth_arg_value_type"

    def __init__(self, arg_number):
        super().__init__(NthArgValueType.name)
        assert isinstance(arg_number, int)
        self.arg_number = arg_number
        self.scope = None

    def __repr__(self):
        return "_nth_arg_value_type(" + str(self.arg_number) + ")"


class NthArgCallableReturnType(SpecialPseudoType):
    name = "_nth_arg_callable_rtype"

    def __init__(self, arg_number):
        super().__init__(NthArgCallableReturnType.name)
        assert isinstance(arg_number, int)
        self.arg_number = arg_number
        self.scope = None


class NthArgMatch(SpecialPseudoType):
    name = "_nth_arg_match"

    def __init__(self, arg_number, match_table):
        super().__init__(NthArgMatch.name)
        assert isinstance(arg_number, int)
        assert isinstance(match_table, dict)
        self.arg_number = arg_number
        self.match_table = match_table
        self.scope = None

    def __repr__(self):
        return "_nth_arg_match(" + str(self.arg_number) + ", {" + \
               ", ".join([repr(key) + ":" + repr(val) for key, val in self.match_table.items()]) + "})"


class MatchAfter(SpecialPseudoType):
    name = "_match_after"

    def __init__(self, arg_numbers, predicate):
        super().__init__(MatchAfter.name)
        self.arg_numbers = arg_numbers
        self.predicate = predicate
        assert isinstance(arg_numbers, list)
        assert isinstance(predicate, TypeDef)

    def __repr__(self):
        return "_match_after(" + repr(self.arg_numbers) + ", " + repr(self.predicate) + ")"


class ScriptableMatch(SpecialPseudoType):
    name = "_scriptable_match"

    def __init__(self, clazz):
        super().__init__(ScriptableMatch.name)
        self.clazz = clazz
        self.evaluate = getattr(clazz, "evaluate")
        self.class_name = clazz.__name__

    def __repr__(self):
        return self.class_name + "(" + ScriptableMatch.name + ")"
