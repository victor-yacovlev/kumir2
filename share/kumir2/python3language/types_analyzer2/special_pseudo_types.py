from . import typedefs
from .util import Singleton


class SpecialPseudoType(typedefs.TypeDef):
    name = "_special_pseudo_type"

    def __init__(self, name):
        super().__init__(name, [])

    def copy(self):
        return self


class AnyType(SpecialPseudoType, metaclass=Singleton):
    name = "_any"

    def __init__(self):
        super().__init__(AnyType.name)


class AnyOf(SpecialPseudoType):
    name = "_any_of"

    def __init__(self, variants):
        super().__init__(AnyOf.name)
        assert isinstance(variants, list)
        self.variants = variants

    def __repr__(self):
        return "_any_of([" + ", ".join([repr(x) for x in self.variants]) + "])"


class SelfType(SpecialPseudoType, metaclass=Singleton):
    name = "_self_type"

    def __init__(self):
        super().__init__(SelfType.name)


class KeyType(SpecialPseudoType, metaclass=Singleton):
    name = "_key_type"

    def __init__(self):
        super().__init__(KeyType.name)


class ValueType(SpecialPseudoType, metaclass=Singleton):
    name = "_value_type"

    def __init__(self):
        super().__init__(ValueType.name)
