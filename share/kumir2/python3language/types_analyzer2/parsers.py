import os
import inspect
import symtable

from . import typetable
from . import funcdefs
from .functable import MethodsTable
# from . import functable
from .typedefs import *
from .funcdefs import *
from .special_pseudo_types import *
from .special_match_types import *
from .special_callable_types import *


def get_resource_file(name: str):
    my_dirname = os.path.dirname(__file__)
    return open(my_dirname + os.pathsep + name.replace("/", os.pathsep))


def __get_supertype_names(clazz):
    result = []
    classtree = inspect.getclasstree([clazz])

    def get_supertypes_r(node):
        if isinstance(node, list) or isinstance(node, tuple):
            for child in node:
                get_supertypes_r(child)
        elif isinstance(node, type):
            name = node.__name__
            self_name = clazz.__name__
            if name != self_name and name not in result:
                result.append(name)

    get_supertypes_r(classtree)
    return result


def __remove_extra_prefix(type_name: str):
    from .headers import base_types_0
    prefix_to_remove = base_types_0.__name__
    if type_name.startswith(prefix_to_remove):
        return type_name[len(prefix_to_remove) + 1]
    else:
        return type_name


def __type_from_annotation(annotation, table):
    assert isinstance(table, typetable.TypesTable)
    if annotation is not None:
        if isinstance(annotation, type):
            type_name = annotation.__qualname__
        else:
            type_name = annotation.__class__.__qualname__
        type_name = __remove_extra_prefix(type_name)
        result = table.lookup_by_name(type_name)
        if isinstance(annotation, list) and len(annotation) > 0:
            list_item = annotation[0]
            list_item_type = __type_from_annotation(list_item, table)
            if list_item_type:
                result = table.lookup_or_create_parametrized_list(list_item_type, "_sequence")
            else:
                result = table.lookup_by_name("list")
        elif isinstance(annotation, tuple) and len(annotation) > 0:
            tuple_item_types = [__type_from_annotation(x, table) for x in list(annotation)]
            tuple_type = table.lookup_or_create_parametrized_tuple(tuple_item_types)
            result = tuple_type
        elif isinstance(annotation, dict) and len(list(annotation.items())) > 0:
            first_entry = list(annotation.items())[0]
            key_item, value_item = first_entry
            key_type = __type_from_annotation(key_item, table)
            value_type = __type_from_annotation(value_item, table)
            if key_type and value_type:
                result = table.lookup_or_create_parametrized_dict(key_type, value_type)
            else:
                result = table.lookup_by_name("dict")
    else:
        type_name = None
        result = None
    if not result and type_name is not None:
        if type_name == SelfType.name:
            result = SelfType()
        elif type_name == KeyType.name:
            result = KeyType()
        elif type_name == ValueType.name:
            result = ValueType()
        elif type_name == AnyType.name:
            result = AnyType()
        elif type_name == AnyOf.name:
            result = AnyOf([__type_from_annotation(x, table) for x in annotation.variants])
        elif type_name == NthArgType.name:
            result = NthArgType(annotation.arg_number)
        elif type_name == NthArgKeyType.name:
            result = NthArgKeyType(annotation.arg_number)
        elif type_name == NthArgValueType.name:
            result = NthArgValueType(annotation.arg_number)
        elif type_name == NthArgCallableReturnType.name:
            result = NthArgCallableReturnType(annotation.arg_number)
        elif type_name == NthArgMatch.name:
            match_table = dict()
            source_match_table = annotation.match_table
            assert isinstance(source_match_table, dict)
            for key, val in source_match_table.items():
                from_type = __type_from_annotation(key, table)
                to_type = __type_from_annotation(val, table)
                match_table[from_type] = to_type
            result = NthArgMatch(annotation.arg_number, match_table)
        elif type_name == TypedCallable.name:
            return_type = annotation.return_type
            arg_types = annotation.arg_types
            rtype = __type_from_annotation(return_type, table)
            atypes = [__type_from_annotation(x, table) for x in arg_types]
            result = TypedCallable(rtype, atypes)
        elif type_name == MatchAfter.name:
            arg_numbers = annotation.arg_numbers_required
            predicate = __type_from_annotation(annotation.predicate, table)
            result = MatchAfter(arg_numbers, predicate)
        elif isinstance(annotation, type):
            supertypes = __get_supertype_names(annotation)
            if "_scriptable_match" in supertypes:
                assert hasattr(annotation, "evaluate")
                result = ScriptableMatch(annotation)

    if not result:
        result = table.lookup_by_name("object")  ## The most generic case
    return result


def __parse_signature(name, types_table, signature):
    assert isinstance(name, str)
    assert isinstance(types_table, typetable.TypesTable)
    assert isinstance(signature, inspect.Signature)
    arguments = []
    for param_name in signature.parameters:
        param = signature.parameters[param_name]
        if param_name == "self":
            argument_type = SelfType()
        else:
            assert isinstance(param, inspect.Parameter)
            annotation = param.annotation
            argument_type = __type_from_annotation(annotation, types_table)
        arg = ArgumentDef(param, argument_type)
        arguments.append(arg)
    assert signature.return_annotation != inspect.Signature.empty
    return_annotation = signature.return_annotation
    return_type = __type_from_annotation(return_annotation, types_table)
    return MethodDef(name, arguments, return_type)


def __add_supertype_recursively(target_list, supertypes):
    assert isinstance(target_list, list)
    assert isinstance(supertypes, list)
    for supertype in supertypes:
        if supertype not in target_list:
            target_list.append(supertype)
    for supertype in supertypes:
        for parent in supertype.supertypes:
            __add_supertype_recursively(target_list, [parent])


def parse_module_classes(types_table, methods_table, source_module, qn_prefix: str):
    assert isinstance(types_table, typetable.TypesTable)
    assert isinstance(methods_table, functable.MethodsTable)
    source_items = inspect.getmembers(source_module, inspect.isclass)

    # create names in table
    for class_name, _ in source_items:
        qn_name = qn_prefix + class_name
        if not types_table.lookup_by_name(qn_name):
            types_table.append(TypeDef(qn_prefix + class_name, [], None))

    # update supertypes
    for class_name, source_entry in source_items:
        table_entry = types_table.lookup_by_name(qn_prefix + class_name)
        supertype_names = __get_supertype_names(source_entry)
        skip = False
        for super_name in supertype_names:
            if super_name == SpecialPseudoType.name or class_name == SpecialPseudoType.name:
                types_table.remove(table_entry)
                skip = True
        if skip: continue
        for super_name in supertype_names:
            if super_name == SpecialPseudoType.name or class_name == SpecialPseudoType.name:
                break
            supertype = types_table.lookup_by_name(qn_prefix + super_name)
            assert supertype is not None
            __add_supertype_recursively(table_entry.supertypes, [supertype])

    # ensure 'object' is the last entry in supertypes list
    for table_entry in types_table.types:
        assert isinstance(table_entry, TypeDef)
        if table_entry.name == "object":
            continue
        assert len(table_entry.supertypes) >= 1
        obj_type = None
        for supertype in table_entry.supertypes:
            assert isinstance(supertype, TypeDef)
            if supertype.name == "object":
                obj_type = supertype
                table_entry.supertypes.remove(supertype)
                break
        assert obj_type is not None
        table_entry.supertypes.append(obj_type)

    # parse methods annotations
    for class_name, source_entry in source_items:
        table_entry = types_table.lookup_by_name(qn_prefix + class_name)
        if not table_entry:
            continue
        methods = inspect.getmembers(source_entry, inspect.isfunction)
        for source_method in methods:
            _, function = source_method
            qualname = function.__qualname__
            method_name = function.__name__
            assert isinstance(qualname, str)
            if qualname.startswith(class_name + "."):
                signature = inspect.signature(function)
                entry_method = __parse_signature(method_name, types_table, signature)
                methods_table.append(qn_prefix + class_name, entry_method)

    # set exact key types and value types
    for class_name, source_entry in source_items:
        table_entry = types_table.lookup_by_name(class_name)
        if not table_entry:
            continue
        assert isinstance(table_entry, TypeDef)
        type_props = inspect.getmembers(source_entry, inspect.isclass)
        for name, clazz in type_props:
            if name == "value_type":
                table_entry.valuetype = types_table.lookup_by_name(qn_prefix + clazz.__name__)
            elif name == "key_type":
                table_entry.keytype = types_table.lookup_by_name(qn_prefix + clazz.__name__)
        value_props = inspect.getmembers(source_entry)
        for name, value in value_props:
            if name == "parametrizable":
                if isinstance(value, bool):
                    table_entry.parametrizable = value

    # update parametrizable parameters from superclasses
    for entry in types_table.types:
        assert isinstance(entry, TypeDef)
        if not entry.parametrizable:
            for super in entry.supertypes:
                assert isinstance(super, TypeDef)
                if super.parametrizable:
                    entry.parametrizable = True
                    if not entry.keytype and super.keytype:
                        entry.keytype = super.keytype
                    if not entry.valuetype and super.valuetype:
                        entry.valuetype = super.valuetype


def parse_module_functions(types_table, methods_table, source_module, qn_prefix: str):
    assert isinstance(types_table, typetable.TypesTable)
    assert isinstance(methods_table, functable.MethodsTable)
    source_items = inspect.getmembers(source_module, inspect.isfunction)
    for name, source_entry in source_items:
        signature = inspect.signature(source_entry)
        method_def = __parse_signature(name, types_table, signature)
        methods_table.append(qn_prefix, method_def)


def parse_ast_annotation_name(node: ast.Name, functions_table, types_table,
                              symbols_table: symtable.SymbolTable) -> TypeDef:
    type_name = node.id
    return types_table.lookup_by_name(type_name)


def parse_ast_function_def(node: ast.FunctionDef, functions_table, types_table,
                           symbols_table: symtable.SymbolTable) -> MethodDef:
    name = node.name
    node_args = node.args
    arguments = []
    for node_arg in node_args.args:
        assert isinstance(node_arg, ast.arg)
        arg_name = node_arg.arg
        annotation = node_arg.annotation
        symbol = symbols_table.lookup(arg_name)
        typedef = None
        if isinstance(annotation, ast.Name):
            typedef = parse_ast_annotation_name(annotation, functions_table, types_table, symbols_table)
        symbol.typedef = typedef
        symbol.lineno = node_arg.lineno
        symbol.col_offset = node_arg.col_offset
        arg_def = ArgumentDef(inspect.Parameter(name=arg_name, kind=inspect.Parameter.POSITIONAL_OR_KEYWORD), typedef)
        arguments.append(arg_def)
    returns = node.returns
    rtype = None
    if isinstance(returns, ast.Name):
        rtype = parse_ast_annotation_name(returns, functions_table, types_table, symbols_table)
    method = MethodDef(name, arguments, rtype)
    return method
