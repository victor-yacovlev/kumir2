import os
import inspect
import string

from .typedefs import TypeDef
from types_analyzer2.special_pseudo_types import SelfType, KeyType, ValueType
from . import functable
# from . import parsers
from .typedefs import *
from .funcdefs import *

try:
    from _kumir import debug
except ImportError:
    debug = print


class TypesTable:
    def __init__(self):
        self.types = []
        self.marks = []

    def save_position(self):
        self.marks.append(self.types.count())

    def restore_position(self):
        position = self.marks.pop(-1)
        self.types = self.types[0:position]

    def append(self, item: TypeDef):
        item.parent_table = self
        self.types.append(item)

    def lookup_by_name(self, name: str):
        for typedef in self.types:
            assert isinstance(typedef, TypeDef)
            if typedef.name == name:
                return typedef

    def lookup_parametrized_list_by_value_type(self, item_type):
        for typedef in self.types:
            assert isinstance(typedef, TypeDef)
            if typedef.name == "list":
                if typedef.valuetype and typedef.valuetype == item_type:
                    return typedef

    def lookup_parametrized_dict_by_key_type_and_value_type(self, key_type, value_type):
        for typedef in self.types:
            assert isinstance(typedef, TypeDef)
            if typedef.name == "dict":
                if typedef.keytype and typedef.keytype == key_type:
                    if typedef.valuetype and typedef.valuetype == value_type:
                        return typedef

    def lookup_or_create_parametrized_list(self, item_type: TypeDef, base_type_name: str):
        result = self.lookup_parametrized_list_by_value_type(item_type)
        if result:
            return result
        list_type = self.lookup_by_name(base_type_name)
        key_type = self.lookup_by_name("int")
        value_type = item_type
        assert isinstance(list_type, TypeDef)
        assert isinstance(key_type, TypeDef)
        assert isinstance(value_type, TypeDef)
        result = self.parametrized_copy(list_type, list_type, key_type, value_type, [], None, None)
        self.types.append(result)
        return result

    def lookup_or_create_parametrized_dict(self, key_type: TypeDef, value_type: TypeDef):
        result = self.lookup_parametrized_dict_by_key_type_and_value_type(key_type, value_type)
        if result:
            return result
        dict_type = self.lookup_by_name("dict")
        assert isinstance(dict_type, TypeDef)
        assert isinstance(key_type, TypeDef)
        assert isinstance(value_type, TypeDef)
        result = self.parametrized_copy(dict_type, dict_type, key_type, value_type, [], None, None)
        self.types.append(result)
        return result

    def __str__(self):
        return "[" + ", ".join([str(x) for x in self.types]) + "]"

    def __repr__(self):
        return "\n".join([str(x) for x in self.types])

    def parametrized_copy(self, typee, selftype, keytype, valuetype, tupleitems, callable_args, callable_rtype):
        assert isinstance(keytype, TypeDef) or keytype is None
        assert isinstance(valuetype, TypeDef) or valuetype is None
        if isinstance(typee, SelfType):
            return selftype
        elif isinstance(typee, TypedCallable):
            return TypedCallable(callable_rtype, callable_args)
        elif isinstance(typee, KeyType):
            if keytype:
                return keytype
            else:
                return self.lookup_by_name("object")
        elif isinstance(typee, ValueType):
            if valuetype:
                return valuetype
            else:
                return self.lookup_by_name("object")
        elif isinstance(typee, SpecialPseudoType):
            return typee  # do not change
        if typee.is_parametrizable():
            kt = keytype
            vt = valuetype
        else:
            kt = None
            vt = None
        new_type = TypeDef(typee.name, typee.supertypes, kt, vt, tupleitems)
        new_type.parent_table = self
        new_type.parametrizable = True
        return self.store_parametrized_type(new_type)

    def store_parametrized_type(self, entry):  ## TODO implement compare operator
        assert isinstance(entry, TypeDef)
        for t in self.types:
            assert isinstance(t, TypeDef)
            if t.name == entry.name and t.keytype == entry.keytype and t.valuetype == entry.valuetype:
                tuple_match = len(t.tupleitems) == len(entry.tupleitems)
                if tuple_match:
                    for i in range(0, len(t.tupleitems)):
                        t_t = t.tupleitems[i]
                        entry_t = entry.tupleitems[i]
                        if t_t != entry_t:
                            tuple_match = False
                            break
                if tuple_match:
                    return t  ## The type already in table, so return it not to make extra copy
        self.types.append(entry)
        return entry

    def lookup_or_create_parametrized_tuple(self, item_types):
        tuple_type = self.lookup_by_name("tuple")
        key_type = self.lookup_by_name("int")
        value_type = None
        assert isinstance(tuple_type, TypeDef)
        assert isinstance(key_type, TypeDef)
        if item_types:
            result = self.parametrized_copy(tuple_type, tuple_type, key_type, value_type, item_types, None, None)
        else:
            result = tuple_type
        return result

    def find_common_type_parent(self, item_types):
        assert isinstance(item_types, list)
        assert len(item_types) >= 1
        return_as_string = False
        if isinstance(item_types[0], TypeDef):
            common_types = [item_types[0]] + item_types[0].supertypes
        elif isinstance(item_types[0], str):
            item_type_0 = self.lookup_by_name(item_types[0])
            common_types = [item_type_0] + item_type_0.supertypes
            return_as_string = True
        obj = self.lookup_by_name("object")
        call = self.lookup_by_name("_callable")
        for x in common_types:
            if isinstance(x, TypedCallable):
                if call not in common_types:
                    common_types.append(call)
        if obj not in common_types:
            common_types.append(obj)
        for entry in item_types[1:]:
            if isinstance(entry, str):
                entry = self.lookup_by_name(entry)
                return_as_string = True
            assert isinstance(entry, TypeDef)
            entry_types = [entry] + entry.supertypes
            if isinstance(entry, TypedCallable):
                if call not in entry_types:
                    entry_types.append(call)
            if obj not in entry_types:
                entry_types.append(obj)
            new_common_types = []
            for test in common_types:
                if test in entry_types:
                    new_common_types.append(test)
            common_types = new_common_types
            # for test in entry_types:
            #     if test not in common_types:
            #         common_types.remove(test)
        assert len(common_types) >= 1  # at least 'object' is common successor for all types
        if return_as_string:
            return common_types[0].name
        else:
            return common_types[0]

    def remove(self, entry: TypeDef):
        index_to_remove = None
        for index, item in enumerate(self.types):
            if item == entry:
                index_to_remove = index
                break
        if index_to_remove is not None:
            self.types = self.types[:index_to_remove] + self.types[index_to_remove + 1:]
