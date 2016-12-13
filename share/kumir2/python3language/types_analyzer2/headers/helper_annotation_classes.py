from . import base_types_0
from ..typedefs import TypeDef
class _round_return_type(base_types_0._scriptable_match):
    def evaluate(self, kwlist_args, kwdict_args):
        if len(kwlist_args) < 2 and not "ndigits" in kwdict_args:
            return self.typestable.lookup_by_name("int")
        elif "number" in kwdict_args:
            return kwdict_args["number"]
        else:
            return kwlist_args[0]

class _min_max_return_type(base_types_0._scriptable_match):
    def evaluate(self, kwlist_args, kwdict_args):
        if len(kwlist_args)==1 and isinstance(kwlist_args, list):
            alist = kwlist_args[0]
            return self.typestable.find_common_type_parent(alist)
        else:
            return self.typestable.find_common_type_parent(kwlist_args)

class _zip_return_type(base_types_0._scriptable_match):
    def evaluate(self, kwlist_args, kwdict_args):
        if kwlist_args:
            value_types = []
            for typee in kwlist_args:
                value_type = typee.valuetype
                value_types.append(value_type)
            tuple_type = self.typestable.lookup_or_create_parametrized_tuple(value_types)
            return self.typestable.lookup_or_create_parametrized_list(tuple_type, "_sequence")
        else:
            return "list"

class _dict_from_iterable_constructor_type(base_types_0._scriptable_match):
    def evaluate(self, kwlist_args, kwdict_args):
        first_arg = kwlist_args[0]
        obj_type = self.typestable.lookup_by_name("object")
        if first_arg.valuetype is None:
            return self.typestable.lookup_or_create_parametrized_dict(obj_type, obj_type)
        tuple_type = first_arg.valuetype
        assert isinstance(tuple_type, TypeDef)
        if len(tuple_type.tupleitems) == 2:
            kt = tuple_type.tupleitems[0]
            vt = tuple_type.tupleitems[1]
            return self.typestable.lookup_or_create_parametrized_dict(kt, vt)
        else:
            return self.typestable.lookup_or_create_parametrized_dict(obj_type, obj_type)

class _list_from_iterable_constructor_type(base_types_0._scriptable_match):
    def evaluate(self, kwlist_args, kwdict_args):
        if len(kwlist_args)==0 or kwlist_args[0] is None or kwlist_args[0].valuetype is None:
            return self.typestable.lookup_by_name("list")
        else:
            vt = kwlist_args[0].valuetype
            return self.typestable.lookup_or_create_parametrized_list(vt, "list")

class _tuple_from_iterable_constructor_type(base_types_0._scriptable_match):
    def evaluate(self, kwlist_args, kwdict_args):
        if len(kwlist_args)==0 or kwlist_args[0].valuetype is None:
            return self.typestable.lookup_by_name("tuple")
        else:
            vt = kwlist_args[0].valuetype
            return self.typestable.lookup_or_create_parametrized_tuple([vt])