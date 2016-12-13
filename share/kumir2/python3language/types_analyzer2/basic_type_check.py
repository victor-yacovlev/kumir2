import symtable
import ast
import os
import types

import sys

from types_analyzer2 import typechecks
from types_analyzer2.special_pseudo_types import AnyOf
from .typedefs import *
from . import typetable
from . import functable
from .type_analizer_errors import *
from .funcdefs import *
from . import parsers
from .special_internal_types import ModuleTypeDef


class TemporarySymbolTable:
    def __init__(self, parent):
        self.parent_table = parent
        self.values = {}
        self.return_type = None

    def lookup(self, name):
        if name in self.values:
            return self.values[name]
        else:
            symbol = self.parent_table.lookup(name)
            self.values[name] = copy.copy(symbol)
            return self.lookup(name)


class Visitor(ast.NodeVisitor):

    def __init__(self, path: list, namespace: str, symtable: symtable.SymbolTable, typetable: typetable.TypesTable,
                 functable: functable.MethodsTable):
        super().__init__()
        self.path = path
        self.symtable = [symtable]
        self.typetable = typetable
        self.functable = functable
        self.function_scopes = []
        self.errors = []
        self.temporary_symtables = []
        self.namespace = namespace

    def visit_Assign(self, node):
        try:
            rvalue_type = self.resolve_expression_type(node.value)
            for target in node.targets:
                self.assign_type_to_target(target, rvalue_type)
        except CompileError as error:
            self.errors.append(error)

    def visit_Call(self, node):
        try:
            self.resolve_expression_type(node)
        except CompileError as error:
            self.errors.append(error)

    def visit_If(self, node):
        ## Mark all children as conditional
        for item in node.body:
            self.mark_as_conditional(item)
        for item in node.orelse:
            self.mark_as_conditional(item)
        self.generic_visit(node)

    def mark_as_conditional(self, node):
        setattr(node, "conditional", True)
        for field, value in ast.iter_fields(node):
            if isinstance(value, list):
                for item in value:
                    if isinstance(item, ast.AST):
                        self.mark_as_conditional(item)
            elif isinstance(value, ast.AST):
                self.mark_as_conditional(value)

    def visit_Import(self, node):
        self.generic_visit(node)
        conditional = getattr(node, "conditional", False)
        if conditional:
            return  ## Too exotic case
        for entry in node.names:
            target_name = entry.asname if entry.asname else entry.name
            st = self.current_symbol_table()
            symbol = st.lookup(target_name)
            symbol.typedef = self.typetable.lookup_by_name("module")
            symbol.lineno = node.lineno
            symbol.col_offset = node.col_offset
            ns = target_name if not self.namespace else self.namespace + "." + target_name
            module_symbol_table = build_symbol_table_for_module(self.path, ns, entry.name,  self.typetable, self.functable)
            if module_symbol_table:
                setattr(symbol, "module_symbol_table", module_symbol_table)


    def visit_For(self, node):
        try:
            iterable_type = self.resolve_expression_type(node.iter)
            iterable_value_type = iterable_type.valuetype if isinstance(iterable_type, TypeDef) else None
            self.assign_type_to_target(node.target, iterable_value_type)
        except CompileError as error:
            self.errors.append(error)
        super().generic_visit(node)

    def visit_Return(self, node):
        try:
            rtype = self.resolve_expression_type(node.value)
            if isinstance(self.current_symbol_table(), TemporarySymbolTable):
                self.current_symbol_table().return_type = rtype
            else:
                method = self.current_function_scope()
                if not method:
                    raise ReturnFromWrongScopeCompileError(node.lineno, node.col_offset)
                assert isinstance(method, MethodDef)
                if not method.return_type or isinstance(method.return_type, AnyType):
                    method.return_type = rtype
                elif isinstance(method.return_type, AnyOf):
                    method.return_type.variants.append(rtype)
                else:
                    anyof = AnyOf([method.return_type, rtype])
                    method.return_type = anyof
        except CompileError as error:
            self.errors.append(error)

    def resolve_assert_statements(self, node):
        ## Currently recognize only 'or' operations
        result = []
        if isinstance(node, ast.BoolOp):
            op = node.op
            if isinstance(op, ast.Or):
                for x in node.values:
                    result += self.resolve_assert_statements(x)
        elif isinstance(node, ast.Call) and node.func.id == "isinstance":
            result.append(node)
        return result

    def visit_Assert(self, node):
        statements = self.resolve_assert_statements(node.test)
        known_variables = {}
        for statement in statements:
            first_arg = statement.args[0]
            second_arg = statement.args[1]
            var_name = first_arg.id
            type_name = second_arg.id
            typee = self.typetable.lookup_by_name(type_name)
            if var_name in known_variables:
                old_type = known_variables[var_name]
                if isinstance(old_type, AnyOf):
                    old_type.variants.append(typee)
                else:
                    known_variables[var_name] = AnyOf([old_type, typee])
            else:
                known_variables[var_name] = typee
        for name, value in known_variables.items():
            symbol = self.current_symbol_table().lookup(name)
            symbol.lineno = node.lineno
            symbol.col_offset = node.col_offset
            symbol.typedef = value

    def visit_FunctionDef(self, node):
        self.push_symbol_table(node.name)
        method = parsers.parse_ast_function_def(node, self.functable, self.typetable, self.current_symbol_table())
        method.ast_visitor = self
        method.ast_node = node.body
        self.functable.append(self.namespace, method)
        self.begin_function_scope(method)
        ast.NodeVisitor.generic_visit(self, node)
        self.pop_symbol_table()
        self.end_function_scope()

    def begin_function_match(self, function_name: str, arguments: dict):
        current_table = self.current_symbol_table()
        if function_name:
            func_symtable = self.find_function_symtable(current_table, function_name)
        else:
            func_symtable = current_table  # in case of lambda-functions
        args_table = TemporarySymbolTable(func_symtable)
        for arg_def, arg_type in arguments.items():
            args_table.values[arg_def.signature.name] = arg_type
        self.temporary_symtables.append(args_table)

    def visit_function_body(self, nodes: list):
        for node in nodes:
            self.visit(node)

    def end_function_match(self):
        table = self.temporary_symtables.pop()
        return getattr(table, "return_type", None)

    def visit_function_lambda(self, arguments: dict, node):
        self.begin_function_match(None, arguments)
        rtype = self.resolve_expression_type(node)
        self.end_function_match()
        return rtype

    def resolve_expression_type(self, node):
        if isinstance(node, ast.Tuple):
            item_types = [self.resolve_expression_type(x) for x in node.elts]
            return self.typetable.lookup_or_create_parametrized_tuple(item_types)

        elif isinstance(node, ast.List):
            item_types = [self.resolve_expression_type(x) for x in node.elts]
            if len(item_types) == 0:
                result = self.typetable.lookup_by_name("list")
            else:
                common_parent = self.typetable.find_common_type_parent(item_types)
                assert isinstance(common_parent, TypeDef)
                if common_parent.name != "object":
                    result = self.typetable.lookup_or_create_parametrized_list(common_parent, "list")
                else:
                    result = self.typetable.lookup_by_name("list")
            return result

        elif isinstance(node, ast.Subscript):
            container_type = self.resolve_expression_type(node.value)
            slice = node.slice
            if isinstance(slice, ast.Index):
                provided_key_type = self.resolve_expression_type(slice.value)
                if provided_key_type is not None and container_type is not None and container_type.keytype is not None:
                    if not typechecks.type_can_be_matched(provided_key_type, container_type.keytype):
                        raise WrongIndexTypeCompileError(node, container_type, provided_key_type)
                value_type = container_type.valuetype if container_type else None
            elif isinstance(slice, ast.Slice):
                for key in [slice.lower, slice.upper, slice.step]:
                    if key is not None:
                        provided_key_type = self.resolve_expression_type(slice.lower)
                        if provided_key_type is not None:
                            if not typechecks.type_can_be_matched(provided_key_type, container_type.keytype):
                                raise WrongIndexTypeCompileError(node, container_type, provided_key_type)
                value_type = container_type
            return value_type

        elif isinstance(node, ast.Dict):
            item_key_types = [self.resolve_expression_type(x) for x in node.keys]
            item_val_types = [self.resolve_expression_type(x) for x in node.values]
            assert len(item_key_types) == len(item_val_types)
            if len(item_key_types) == 0:
                result = self.typetable.lookup_by_name("dict")
            else:
                common_key_parent = self.typetable.find_common_type_parent(item_key_types)
                common_val_parent = self.typetable.find_common_type_parent(item_val_types)
                result = self.typetable.lookup_or_create_parametrized_dict(common_key_parent, common_val_parent)
            return result

        elif isinstance(node, ast.Num):
            type_name = type(node.n).__name__
            return self.typetable.lookup_by_name(type_name)

        elif isinstance(node, ast.Str):
            return self.typetable.lookup_by_name("str")

        elif isinstance(node, ast.Bytes):
            return self.typetable.lookup_by_name("bytes")

        elif isinstance(node, ast.UnaryOp):
            operand_type = self.resolve_expression_type(node.operand)
            if operand_type is None:
                return None

            assert isinstance(operand_type, typetable.TypeDef)
            if isinstance(node.op, ast.USub):
                op_name = "__neg__"
            else:
                op_name = "__pos__"
            methods = list(self.find_all_instance_methods(operand_type, op_name))
            if not methods:
                raise UnsupportedOperandTypesCompileError(node.lineno, node.col_offset, node.op, None, operand_type)

            match_error = None
            ## Try to match found methods and select first matching one
            for method in methods:
                assert isinstance(method, MethodDef)
                kwdict = {}
                try:
                    return_type = self.functable.match(method, [operand_type], self.typetable, kwdict, operand_type)
                except MatchError as e:
                    return_type = None
                    if not match_error: match_error = e  ## display first match error
                if return_type is not None:
                    match_error = None
                    break
            if match_error:
                raise transform_match_error_to_compile_error(match_error)
            return return_type

        elif isinstance(node, ast.Compare):
            current_type = self.resolve_expression_type(node.left)
            assert isinstance(current_type, typetable.TypeDef) or current_type is None
            if current_type is None:
                bool_type = self.typetable.lookup_by_name("bool")
                return bool_type
            for op, next_operand in zip(node.ops, node.comparators):
                op_name = type(op).__name__
                op_py_name = "__" + op_name.lower() + "__"
                operand_type = self.resolve_expression_type(next_operand)
                assert isinstance(operand_type, typetable.TypeDef)
                methods = list(self.find_all_instance_methods(current_type, op_py_name))
                if not methods:
                    raise UnsupportedOperandTypesCompileError(node.lineno, node.col_offset,
                                                              op, current_type,
                                                              operand_type)
                match_error = None
                ## Try to match found methods and select first matching one
                for method in methods:
                    assert isinstance(method, MethodDef)
                    args = [current_type, operand_type]
                    kwdict = {}
                    try:
                        return_type = self.functable.match(method, args, self.typetable, kwdict, current_type)
                    except MatchError as e:
                        return_type = None
                        if not match_error: match_error = e  ## display first match error
                    if return_type is not None:
                        match_error = None
                        break
                if match_error:
                    raise transform_match_error_to_compile_error(match_error, node)
                current_type = operand_type
            return return_type  ## is it always "bool" ?

        elif isinstance(node, ast.BinOp):
            left_argument_type = self.resolve_expression_type(node.left)
            right_argument_type = self.resolve_expression_type(node.right)
            if left_argument_type is None or right_argument_type is None:
                # Try to detect type by operator
                op = node.op
                if isinstance(op, ast.BitAnd) or isinstance(op, ast.BitOr) or isinstance(op, ast.BitXor):
                    # Bitwise operations can apply to integer arguments,
                    # so it's possible to specify their types
                    int_type = self.typetable.lookup_by_name("int")
                    args = [(left_argument_type, node.left), (right_argument_type, node.right)]
                    for arg_type, arg_node in args:
                        if arg_type is None and isinstance(arg_node, ast.Name):
                            self.assign_type_to_target(arg_node, int_type)
                    return int_type   # Bitwise operations rtype is always int
                return None  # in case of non-annotated item in expression

            ## If there was an error while resolving subexpressions,
            ## it should be raised before, so all types are known
            assert isinstance(left_argument_type, typetable.TypeDef)
            assert isinstance(right_argument_type, typetable.TypeDef)

            op_name = type(node.op).__name__
            op_py_name = "__" + op_name.lower()[:3] + "__"
            alt_op_py_name = "__r" + op_name.lower()[:3] + "__"
            methods = list(self.find_all_instance_methods(left_argument_type, op_py_name)) + \
                      list(self.find_all_instance_methods(right_argument_type, alt_op_py_name))
            if not methods:
                raise UnsupportedOperandTypesCompileError(node, node.op, left_argument_type,
                                                          right_argument_type)

            match_error = None
            ## Try to match found methods and select first matching one
            for method in methods:
                assert isinstance(method, MethodDef)
                if method.name == op_py_name:
                    instance_object = left_argument_type
                    args = [left_argument_type, right_argument_type]
                else:
                    instance_object = right_argument_type
                    args = [right_argument_type, left_argument_type]
                kwdict = {}
                try:
                    return_type = self.functable.match(method, args, self.typetable, kwdict, instance_object)
                except MatchError as e:
                    return_type = None
                    if not match_error: match_error = e  ## display first match error
                if return_type is not None:
                    match_error = None
                    break
            if match_error:
                raise transform_match_error_to_compile_error(match_error, node)
            return return_type

        elif isinstance(node, ast.Name):
            st = self.current_symbol_table()
            symbol = st.lookup(node.id)
            if isinstance(symbol, TypeDef):
                return symbol
            if hasattr(symbol, "typedef"):
                symbol_type = symbol.typedef
                return symbol_type
            ## Check if symbol is function
            methods = self.functable.lookup_functions_by_name(node.id)
            if methods:
                assert isinstance(methods[0], MethodDef)
                return methods[0].to_callable_type()
            ## standard symtable implementation does not handle typedefs, so check for parent tables
            if len(self.symtable) > 1:
                for i in range(len(self.symtable)-2,-1,-1):
                    table = self.symtable[i]
                    try:
                        symbol = table.lookup(node.id)
                    except KeyError:
                        return None
                    if hasattr(symbol, "typedef"):
                        symbol_type = symbol.typedef
                        return symbol_type

        elif isinstance(node, ast.NameConstant):
            value = node.value
            type_name = type(value).__qualname__
            value_type = self.typetable.lookup_by_name(type_name)
            return value_type

        elif isinstance(node, ast.Call):
            assert isinstance(node.func, ast.Attribute) or isinstance(node.func, ast.Name)
            args = []
            kwdict = {}
            for arg in node.args:
                arg_type = self.resolve_expression_type(arg)
                args.append(arg_type)
            if isinstance(node.func, ast.Attribute):
                instance_type = self.resolve_expression_type(node.func.value)
                if not instance_type:
                    return None
                assert isinstance(instance_type, TypeDef)

                def resolve_fq_name(n):
                    if isinstance(n, ast.Attribute):
                        return resolve_fq_name(n.value) + "." + n.attr
                    elif isinstance(n, ast.Name):
                        return n.id

                if isinstance(instance_type, ModuleTypeDef):
                    ## module function
                    fully_qualified_name = resolve_fq_name(node.func)
                    methods = self.functable.lookup_functions_by_name(fully_qualified_name)
                else:
                    ## class method
                    args = [instance_type] + args  ## prepend 'self' parameter to args list
                    method_name = node.func.attr
                    methods = self.find_all_instance_methods(instance_type, method_name)
            else:
                instance_type = None
                method_name = node.func.id
                methods = self.functable.lookup_functions_by_name(method_name)
                if self.namespace:
                    methods = self.functable.lookup_functions_by_name(self.namespace + "." + method_name) + methods
            if not methods:
                # Might be class constructor?
                instance_type = self.typetable.lookup_by_name(method_name)
                if instance_type is not None:
                    methods = self.find_all_instance_methods(instance_type, "__init__")
                    if methods:
                        args = [instance_type] + args  # prepend 'self' parameter for __init__
            if not methods:
                raise NoSuchMethodCompileError(node, instance_type, method_name)
            ## Try to match found methods and select first matching one
            match_error = None
            for method in methods:
                assert isinstance(method, MethodDef)
                try:
                    return_type = self.functable.match(method, args, self.typetable, kwdict, instance_type)
                except MatchError as e:
                    return_type = None
                    if not match_error: match_error = e  ## display first match error
                # except:
                #     ## something gone wrong, so it can not resolve type
                #     match_error = None
                #     return_type = None
                #     break
                if return_type is not None:
                    match_error = None
                    break
            if match_error:
                compile_error = transform_match_error_to_compile_error(match_error, node)
                assert isinstance(compile_error, CompileError)
                raise compile_error
            return return_type

        elif isinstance(node, ast.Lambda):
            arg_names = []
            for arg in node.args.args:
                assert isinstance(arg, ast.arg)
                arg_name = arg.arg
                arg_names.append(arg_name)
            lambda_type = Lambda(arg_names, node.body)
            lambda_type.ast_visitor = self
            # lambda_type = TypedCallable(self.typetable.lookup_by_name("object"), [AnyType() for _ in node.args.args])
            ## TODO specialuze lambda typed callable from lambda body
            return lambda_type

        elif isinstance(node, ast.IfExp):
            true_type = self.resolve_expression_type(node.body)
            false_type = self.resolve_expression_type(node.orelse)
            if true_type is not None and false_type is None:
                return true_type
            elif true_type is None and false_type is not None:
                return false_type
            elif true_type is None and false_type is None:
                return None
            else:
                 return self.typetable.find_common_type_parent([true_type, false_type])

        elif isinstance(node, ast.ListComp):
            comp_args = {}
            for generator in node.generators:
                iter_type = self.resolve_expression_type(node.generators[0].iter).valuetype
                if isinstance(generator.target, ast.Name):
                    arg = ArgumentDef(generator.target.id, iter_type)
                    comp_args[arg] = iter_type
            self.begin_function_match("", comp_args)
            elt_type = self.resolve_expression_type(node.elt)
            self.end_function_match()
            list_type = self.typetable.lookup_or_create_parametrized_list(elt_type, "list")
            return list_type

        elif isinstance(node, ast.Attribute):
            def update_node_st(n, st):
                if isinstance(n.value, ast.Name):
                    try:
                        symbol = st.lookup(n.value.id)
                        r = getattr(symbol, "module_symbol_table", st)
                    except:
                        r = st
                    if not hasattr(n.value, "symbol_table"):
                        setattr(n.value, "symbol_table", r)
                elif isinstance(n.value, ast.Attribute):
                    update_node_st(n.value, st)

            update_node_st(node, self.current_symbol_table())

            if isinstance(node.value, ast.Name):
                symb_name = node.value.id
            elif isinstance(node.value, ast.Attribute):
                symb_name = node.value.attr
            else:
                symb_name = None
            attribute_name = node.attr

            instance_type = self.resolve_expression_type(node.value)
            if instance_type is None:
                return None

            if attribute_name in instance_type.properties:
                return instance_type.properties[attribute_name]

            if isinstance(instance_type, ModuleTypeDef):
                st = getattr(node.value, "symbol_table")
                symbol = st.lookup(attribute_name)
                module_symbol_table = getattr(symbol, "module_symbol_table", None)
                if module_symbol_table is not None:
                    # symbol = module_symbol_table.lookup(attribute_name)
                    setattr(node, "symbol_table", module_symbol_table)
                if symbol is not None:
                    result = getattr(symbol, "typedef", None)
                    return result


    def assign_type_to_target(self, target, rvalue_type):
        assert isinstance(rvalue_type, TypeDef) or rvalue_type is None
        if isinstance(target, ast.Tuple) and rvalue_type is not None:
            if len(rvalue_type.tupleitems) != len(target.elts):
                raise WrongTupleAssignmentCompileError(target.lineno, target.col_offset, len(target.elts),
                                                       len(rvalue_type.tupleitems))
            for child, rvalue_entry in zip(target.elts, rvalue_type.tupleitems):
                self.assign_type_to_target(child, rvalue_entry)
        elif isinstance(target, ast.Tuple) and rvalue_type is None:
            for child in target.elts:
                self.assign_type_to_target(child, None)
        elif isinstance(target, ast.Name):
            st = self.current_symbol_table()
            symbol = st.lookup(target.id)
            symbol.typedef = rvalue_type
            symbol.lineno = target.lineno
            symbol.col_offset = target.col_offset
            # check for unresolved function argument type
            method = self.current_function_scope()
            if method:
                for arg in method.arguments:
                    if arg.typedef is None and arg.signature.name==target.id:
                        arg.typedef = rvalue_type

    def find_all_instance_methods(self, clazz: TypeDef, name: str):
        result = self.functable.lookup_methods_by_fully_qualified_name(clazz.name, name)
        for super in clazz.supertypes:
            result += self.functable.lookup_methods_by_fully_qualified_name(super.name, name)
        return result

    def current_symbol_table(self):
        if self.temporary_symtables:
            return self.temporary_symtables[-1]
        else:
            return self.symtable[-1]

    def find_function_symtable(self, current_table, children_name):
        assert isinstance(current_table, symtable.SymbolTable)
        child_tables = current_table.get_children()
        for table in child_tables:
            table_name = table.get_name()
            if table_name == children_name:
                return table
        raise AttributeError("Table not found for children name: " + children_name)

    def push_symbol_table(self, children_name: str):
        current_table = self.symtable[-1]
        self.symtable.append(self.find_function_symtable(current_table, children_name))


    def pop_symbol_table(self):
        self.symtable.pop()

    def begin_function_scope(self, method: MethodDef):
        self.functable.append(self.namespace, method)
        self.function_scopes.append(method)

    def end_function_scope(self):
        self.function_scopes.pop()

    def current_function_scope(self):
        if self.function_scopes:
            return self.function_scopes[-1]
        else:
            return None


def find_module_file(path, name):
    assert isinstance(path, list)
    assert isinstance(name, str)
    from .headers import stdlib
    stdlib_dir = os.path.dirname(stdlib.__file__)
    header_file = stdlib_dir + os.path.sep + name + ".py"
    if os.path.exists(header_file):
        return header_file
    for pe in path:
        full_path = pe + os.path.sep + name + ".py"
        if os.path.exists(full_path):
            return full_path


def is_standard_module(path, file_name, module_name):
    assert isinstance(path, list)
    assert isinstance(file_name, str)
    assert isinstance(module_name, str)
    if module_name=="sys": return True
    prefix = sys.prefix
    if file_name.startswith(prefix):
        return True
    return False

__modules_cache = {}

def build_symbol_table_for_module(path, namespace, module_name, types_table, methods_table):
    global __modules_cache
    module_file_name = find_module_file(path, module_name)
    if not module_file_name: return
    if module_name in __modules_cache and is_standard_module(path, module_file_name, module_name):
        return __modules_cache[module_name]
    with open(module_file_name, mode='r') as f:
        contents = f.read()
        table, _ = build_symbol_table_for_text(contents, path, namespace, types_table, methods_table)
        if is_standard_module(path, module_file_name, module_name) and module_name not in __modules_cache:
            __modules_cache[module_name] = table
        return table


def build_symbol_table_for_text(text, path, namespace, types_table, methods_table):
    try:
        tree = ast.parse(text)
    except SyntaxError:
        return None, []
    root_table = symtable.symtable(text, "<string>", "exec")
    visitor = Visitor(path, namespace, root_table, types_table, methods_table)
    visitor.visit(tree)
    result = visitor.symtable[-1] if visitor.symtable else None
    return result, visitor.errors