import copy

from types_analyzer2.typechecks import type_can_be_matched
# from . import basic_type_check
# from . import typetable
from .type_analizer_errors import *
from .typedefs import *
from .funcdefs import *

from .special_pseudo_types import *
from .special_match_types import *
from .special_callable_types import *

try:
    from _kumir import debug
except ImportError:
    debug = print


class MethodsTable:
    def __init__(self):
        self.methods = dict()
        self.marks = []

    def save_position(self):
        self.marks.append(self.types.count())

    def restore_position(self):
        position = self.marks.pop(-1)
        self.types = self.methods[0:position]

    def append(self, namespace, item: MethodDef):
        item.parent_table = self
        if namespace:
            key = namespace + "." + item.name
        else:
            key = item.name
        if not key in self.methods:
            self.methods[key] = []
        self.methods[key].append(item)
        return item

    def lookup_methods_by_fully_qualified_name(self, class_fq_name: str, method_name: str):
        ## TODO build and use tree-based index
        key = class_fq_name + "." + method_name
        if key in self.methods:
            return self.methods[key]
        else:
            return []

    def lookup_functions_by_name(self, function_name: str):
        key = function_name
        if key in self.methods:
            return self.methods[key]
        else:
            return []

    def __build_effective_argument_list(self, method, args, kwdict):
        assert isinstance(method, MethodDef)
        result = dict()
        provided_positional_args_count = len(args)
        args = copy.copy(args)
        kwdict = copy.copy(kwdict)
        assert isinstance(args, list)
        assert isinstance(kwdict, dict)
        allow_kwlist = False
        allow_kwdict = False
        kwlist_arg = None
        for index, arg in enumerate(method.arguments):
            arg.arg_number = index + 1
            assert isinstance(arg, ArgumentDef)
            sig = arg.signature
            assert isinstance(sig, inspect.Parameter)
            kind = sig.kind
            assert isinstance(kind, inspect._ParameterKind)
            provided_arg = None
            arg_name = arg.signature.name
            if kind == inspect._ParameterKind.KEYWORD_ONLY:
                allow_kwdict = True
                if arg_name in kwdict:
                    provided_arg = kwdict[arg_name]
                    del kwdict[arg_name]
            if kind == inspect._ParameterKind.POSITIONAL_OR_KEYWORD:
                allow_kwdict = True
                if arg_name in kwdict:
                    provided_arg = kwdict[arg_name]
                    del kwdict[arg_name]
                elif len(args) > 0:
                    provided_arg = args.pop(0)
                elif arg.signature.default != inspect.Signature.empty:
                    pass  # do bind this parameter to actual list
                else:
                    raise NoEnoughtArgumentsMatchError(method, provided_positional_args_count)
            if kind == inspect._ParameterKind.VAR_POSITIONAL:
                allow_kwlist = True
                kwlist_arg = arg
            result[arg] = copy.copy(provided_arg) if provided_arg else None
        if kwdict and not allow_kwdict:
            raise MethodDoesNotAllowKwdictMatchError(method, kwdict.keys())
        if args and not allow_kwlist:
            raise ExtraArgumentsMatchError(method, len(args))
        if kwlist_arg:
            result[arg] = [copy.copy(x) for x in args]
        for x in result.keys():
            self.bind_object_to_placeholder(method, x.typedef)
        return result

    def __update_formal_arguments_by_name(self, method, used_args):
        for arg in used_args:
            for marg in method.arguments:
                if marg.signature.name == arg.signature.name:
                    yield marg

    ## TODO move to separate .py file
    def match(self, method: MethodDef, args: list, types_table, kwdict, owner_class):
        assert isinstance(types_table, typetable.TypesTable)

        initial_method = method  # for control it's not changed (shown in debugger)
        method = method.copy()

        ## Build actual arguments list and check if arguments enoungt
        bound_args = self.__build_effective_argument_list(method, args, kwdict)
        for prov_arg in bound_args.values():
            if isinstance(prov_arg, TypedCallable):
                self.bind_object_to_placeholder(prov_arg, prov_arg)

        self.__match_arguments(method, bound_args, types_table, owner_class, method)

        ## Check for individual arguments match
        args_to_check = list(bound_args.keys())
        args_to_check.sort(key=lambda x: x.arg_number)
        for method_arg in args_to_check:
            assert isinstance(method_arg, ArgumentDef)
            provided_type = bound_args[method_arg]
            if method_arg.signature.kind != inspect._ParameterKind.VAR_POSITIONAL:
                assert isinstance(provided_type, TypeDef) or provided_type is None
                required_type = method_arg.typedef
                reverse_match = getattr(method_arg, "reverse_match", False)
                if reverse_match:  # in case if required type constructed after provided
                    required_type, provided_type = provided_type, required_type
                    required_type = self.__match_typedef(method, provided_type, required_type, bound_args, types_table,
                                                         owner_class, method)
                if provided_type and not type_can_be_matched(provided_type, required_type):
                    raise TypeMismatchMatchError(method, required_type, provided_type)
            else:
                assert isinstance(provided_type, list)
                required_type = method_arg.typedef
                if provided_type and required_type.valuetype:
                    common_type = types_table.find_common_type_parent(provided_type)
                    assert isinstance(common_type, TypeDef)
                    if not type_can_be_matched(common_type, required_type.valuetype):
                        raise TypeMismatchMatchError(method, required_type, provided_type)
                        pass

        if method.return_type is None:
            ## If there is not any information on function return type, try to deduce it from
            ## function AST body
            visitor = method.ast_visitor;
            assert isinstance(visitor, basic_type_check.Visitor)
            node = method.ast_node
            visitor.begin_function_match(method.name, bound_args)
            visitor.visit_function_body(node)
            return_type = visitor.end_function_match()

        else:
            ## If matched then here were no errors raised - return function return type
            return_type = self.__match_typedef(method, method.return_type, method.return_type, bound_args, types_table,
                                               owner_class, method)
        if isinstance(return_type, AnyOf):
            return_type = self.__simplify_anyof(return_type)
        return return_type

    def bind_method_to_placeholders(self, method):
        assert isinstance(method, MethodDef)
        for arg in method.arguments:
            assert isinstance(arg, ArgumentDef)
            self.bind_object_to_placeholder(method, arg.typedef)
        self.bind_object_to_placeholder(method, method.return_type)

    def bind_object_to_placeholder(self, callable, typedef: TypeDef):
        if typedef is None:
            return
        if any([isinstance(typedef, x)
                for x in [
                    NthArgType, NthArgKeyType, NthArgValueType, NthArgMatch, NthArgCallableReturnType, TypedCallable,
                    ScriptableMatch, AnyType, MatchAfter
                ]]):
            typedef.scope = callable
        if typedef.keytype != typedef: self.bind_object_to_placeholder(callable, typedef.keytype)
        if typedef.valuetype != typedef: self.bind_object_to_placeholder(callable, typedef.valuetype)
        if isinstance(typedef, MatchAfter):
            self.bind_object_to_placeholder(callable, typedef.predicate)
        for ti in typedef.tupleitems:
            self.bind_object_to_placeholder(callable, ti)
        if isinstance(typedef, TypedCallable):
            self.bind_object_to_placeholder(callable, typedef.return_type)
            for at in typedef.arg_types:
                self.bind_object_to_placeholder(callable, at.typedef)

    def __nth_arg(self, method, bound_args: dict, arg_no: int, types_table):
        for arg, val in bound_args.items():
            if arg.arg_number == arg_no:
                assert isinstance(arg, ArgumentDef)
                assert isinstance(val, TypeDef) or val is None
                return arg, val
        assert isinstance(method, MethodDef)
        arg = method.arguments[arg_no - 1]
        assert isinstance(arg, ArgumentDef)
        assert arg.signature.default != inspect.Signature.empty
        assert isinstance(types_table, typetable.TypesTable)
        return copy.copy(arg), arg.typedef

    def __nth_arg_vt(self, method, bound_args: dict, arg_no: int, types_table):
        a, nth_arg = self.__nth_arg(method, bound_args, arg_no, types_table)
        assert isinstance(nth_arg, TypeDef)
        if nth_arg.valuetype is None:
            return a, types_table.lookup_by_name("object")
        return a, nth_arg.valuetype


    def __nth_arg_kt(self, method, bound_args: dict, arg_no: int, types_table):
        a, nth_arg = self.__nth_arg(method, bound_args, arg_no, types_table)
        assert isinstance(nth_arg, TypeDef)
        if nth_arg.keytype is None:
            return a, types_table.lookup_by_name("object")
        return a, nth_arg.keytype


    def __match_values(self, method, bound_args: dict, types_table, owner_class):
        assert isinstance(types_table, typetable.TypesTable)
        for arg, val in bound_args.items():
            if isinstance(val, TypedCallable):
                new_val = self.__match_typedef(method, val, val, bound_args, types_table, owner_class, val)
                bound_args[arg] = copy.copy(new_val)

    def __match_arguments(self, method, bound_args: dict, types_table, owner_class, scope):
        assert isinstance(types_table, typetable.TypesTable)
        args_list = list(bound_args.keys())
        args_list.sort(key=lambda x: x.arg_number)
        for arg in args_list:
            req_type = arg.typedef
            prov_type = bound_args[arg]
            reverse_match = False
            if isinstance(req_type, MatchAfter):
                for req_arg_no in req_type.arg_numbers:
                    req_arg, req_val = self.__nth_arg(method, bound_args, req_arg_no, types_table)
                    if not req_arg.resolved:
                        req_arg.typedef = self.__match_typedef(method, req_arg.typedef, req_val, bound_args,
                                                               types_table, owner_class, scope)
                        req_arg.resolved = True
                req_type = req_type.predicate
                reverse_match = True
            req_type = self.__match_typedef(method, req_type, prov_type, bound_args, types_table, owner_class, scope)
            arg.typedef = req_type
            setattr(arg, "reverse_match", reverse_match)

    def __match_typedef(self, method, pattern: TypeDef, value: TypeDef, bound_args, types_table, owner_class, scope):
        if pattern is None and value is None:
            return None
        assert isinstance(types_table, typetable.TypesTable)
        if isinstance(pattern, SelfType):
            result = owner_class
        elif isinstance(pattern, NthArgType):
            if isinstance(pattern.scope, TypedCallable):
                result = scope.arg_types[pattern.arg_number - 1].typedef
            else:
                _, result = self.__nth_arg(method, bound_args, pattern.arg_number, types_table)
        elif isinstance(pattern, NthArgValueType):
            _, result = self.__nth_arg_vt(method, bound_args, pattern.arg_number, types_table)
        elif isinstance(pattern, NthArgKeyType):
            _, result = self.__nth_arg_kt(method, bound_args, pattern.arg_number, types_table)
        elif isinstance(pattern, NthArgCallableReturnType):
            arg, result = self.__nth_arg(method, bound_args, pattern.arg_number, types_table)
            assert isinstance(result, TypedCallable) or result is None
            reverse_match = getattr(arg, "reverse_match", False)
            if not result: return None
            if reverse_match and isinstance(arg.typedef, TypedCallable):
                result = arg.typedef.return_type
            else:
                result = result.return_type
        elif isinstance(pattern, AnyType):
            result = value
        elif isinstance(pattern, ScriptableMatch):
            check_args = list(bound_args.keys())
            check_args.sort(key=lambda x: x.arg_number)
            kwlist = []
            kwdict = {}
            for arg in check_args:
                assert isinstance(arg, ArgumentDef)
                sig = arg.signature;
                assert isinstance(sig, inspect.Parameter)
                kind = sig.kind;
                assert isinstance(kind, inspect._ParameterKind)
                if kind in [inspect._ParameterKind.POSITIONAL_OR_KEYWORD]:
                    kwlist.append(bound_args[arg])
                elif kind in [inspect._ParameterKind.KEYWORD_ONLY]:
                    kwdict[arg.signature.name] = bound_args[arg]
                elif kind in [inspect._ParameterKind.VAR_POSITIONAL]:
                    kwlist += bound_args[arg]
            clazz = pattern.clazz
            ev = pattern.evaluate
            # assert inspect.ismethod(ev)
            assert inspect.isclass(clazz)
            instance = clazz()
            instance.typestable = types_table
            result = instance.evaluate(kwlist, kwdict)
        elif pattern is not None:
            result = pattern
        else:
            result = value


        # Do recursively
        result = copy.copy(result)
        value = copy.copy(value)
        assert isinstance(result, TypeDef)

        if result.is_parametrizable() and (result.keytype is None or isinstance(result.keytype, SpecialPseudoType)) and value:
            result.keytype = self.__match_typedef(method, result.keytype, value.keytype, bound_args, types_table,
                                                  owner_class, scope)
        if result.is_parametrizable() and result.valuetype != result and not isinstance(value, SpecialPseudoType):
            if isinstance(value, TypeDef):
                vt = value.valuetype
            elif isinstance(value, list) and len(value) > 0:  # kwlist
                vt = types_table.find_common_type_parent(value)
            else:
                vt = None
            if vt:
                result.valuetype = self.__match_typedef(method, result.valuetype, vt, bound_args, types_table,
                                                        owner_class, scope)
            else:
                result.valuetype = None
        if isinstance(result, TypedCallable) and isinstance(value, TypedCallable):
            if len(result.arg_types) == len(value.arg_types):
                result.arg_types = copy.copy(result.arg_types)
                value.arg_types = copy.copy(value.arg_types)
                for index, (p_arg, v_arg) in enumerate(zip(result.arg_types, value.arg_types)):
                    assert isinstance(p_arg, ArgumentDef)
                    assert isinstance(v_arg, ArgumentDef)
                    if isinstance(p_arg, AnyType):
                        p_arg = v_arg
                    new_arg_type = self.__match_typedef(method, p_arg.typedef, v_arg.typedef, bound_args, types_table,
                                                        owner_class, scope)
                    if new_arg_type in v_arg.typedef.supertypes:
                        new_arg_type = v_arg.typedef
                    result.arg_types[index] = copy.copy(result.arg_types[index])
                    value.arg_types[index] = copy.copy(value.arg_types[index])
                    result.arg_types[index].typedef = new_arg_type
                    value.arg_types[index].typedef = new_arg_type
            if isinstance(value, Lambda):
                value.return_type = self.resolve_lambda_rtype(value)
            if isinstance(result.return_type, AnyType):
                new_rtype = self.__match_typedef(method, value.return_type, value.return_type, None, types_table, None,
                                                 value)
                value.return_type = new_rtype
                result.return_type = new_rtype
            elif isinstance(result.return_type, SpecialPseudoType):
                new_rtype = self.__match_typedef(method, result.return_type, value.return_type, None, types_table, None,
                                                 value)
                value.return_type = new_rtype
                result.return_type = new_rtype
            else:
                if not type_can_be_matched(result.return_type, value.return_type):
                    if isinstance(value, Lambda):
                        raise LambdaArgumentReturnTypeMatchError(method, result.return_type, value.return_type)
        if "tuple" == result.name and isinstance(value, TypeDef) and "tuple" == value.name and len(
                result.tupleitems) == len(value.tupleitems):
            result.tupleitems = copy.copy(result.tupleitems)
            value.tupleitems = copy.copy(result.tupleitems)
            for index, (p_arg, v_arg) in enumerate(zip(result.tupleitems, value.tupleitems)):
                if isinstance(p_arg, AnyType):
                    p_arg = v_arg
                new_arg = self.__match_typedef(method, p_arg, v_arg, bound_args, types_table, owner_class, scope)
                result.tupleitems[index] = new_arg
                value.tupleitems[index] = new_arg
        return result

    def __simplify_anyof(self, typedef):
        assert isinstance(typedef, AnyOf)
        types_varies = False
        first_type = typedef.variants[0]
        for typee in typedef.variants[1:]:
            if typee != first_type:
                types_varies = True
                break
        if types_varies:
            return typedef
        else:
            return first_type

    def resolve_lambda_rtype(self, value):
        assert isinstance(value, Lambda)
        visitor = value.ast_visitor;
        assert isinstance(visitor, basic_type_check.Visitor)
        node = value.ast_node
        bound_args = {}
        for arg in value.arg_types:
            bound_args[arg] = arg.typedef
        return_type = visitor.visit_function_lambda(bound_args, node)
        return return_type


