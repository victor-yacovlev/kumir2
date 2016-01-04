from _ast import BinOp, UnaryOp, Name

__author__ = 'velkerr'

import inspect
import csv
from ast import *

from types_analyser.exceptions import *
from types_analyser.node_analysis import types_table, \
    Type, Method, Field, Variable, vars_table

try:
    from _kumir import debug
except ImportError:
    debug = print

def _find_element(elem_name, collection):
    low_el_name = elem_name.lower()
    types = [current for current in collection if current.simple_name.lower() == elem_name]
    if not types:
        return None
    return types[0]


def find_type(name):
    return _find_element(name, types_table)


def _parse(string):

    def is_terminal(name):
        return True not in (s in name for s in '[]{},')

    def parse_inner(name):
        if is_terminal(name):
            return find_type(name)
        delimiter_index = name.index('[')
        outer_type = find_type(name[:delimiter_index])
        name = name[delimiter_index + 1:-1]
        splitted_elems = parse_elements(name)
        outer_type.keys = parse_inner(splitted_elems[0])
        if len(splitted_elems) > 1:
            outer_type.values = parse_inner(splitted_elems[1])
        return outer_type

    def parse_elements(name):
        elements = []
        balance = 0
        previous_mark = 0
        for index, symb in enumerate(name):
            if symb is '[':
                balance += 1
            elif symb is ']':
                balance -= 1
            if symb is ',' and balance == 0:
                elements.append(name[previous_mark:index].strip())
                previous_mark = index + 1
                if previous_mark in (len(name), 1):  # 1 because previous_mark = index + 1
                    err_mess_mode = 0 if previous_mark == 1 else 1
                    raise UnexpectedEndOfDefinitionException(err_mess_mode)
            if index == len(name) - 1:
                elements.append(name[previous_mark:])
        return elements

    def parse_final(top_level_elems):
        lower_level_elems = [parse_elements(tle.strip()) for tle in top_level_elems] if len(top_level_elems) != 1 else parse_elements(top_level_elems[0].strip())
        return [parse_inner(lle) for lle in lower_level_elems] if len(lower_level_elems) != 1 else parse_inner(lower_level_elems[0])

    return parse_final(string.split('|'))


class AnalysisNodeVisitor(NodeVisitor):
    _collections = ('list', 'tuple', 'set', 'frozenset')

    @staticmethod
    def _parse_args_from_doc(string):
        if '@type ' in string or ':type ' in string:
            argument_parts = string.split(":")
            if len(string.split()) < 3 or True in [':' in ar for ar in argument_parts]:
                raise InvalidDocStringException('Class \' docstring')
            return argument_parts[1].split()[1], _parse(argument_parts[2].strip())

    @staticmethod
    def _analyse_ClassDef(node):
        parents = []
        methods = []
        fields = []
        for base in node.bases:  # searching parents
            base_type = find_type(base.id)
            if base_type is not None:
                parents.append(base_type)
        for body_element in node.body:
            if isinstance(body_element, FunctionDef):  # searching methods
                methods.append(AnalysisNodeVisitor._analyse_FunctionDef(body_element))
            elif isinstance(body_element, ClassDef):  # searching nested classes
                AnalysisNodeVisitor._analyse_ClassDef(body_element)
            elif isinstance(body_element, AugAssign) or isinstance(body_element, Assign):
                fields.append(Field(AnalysisNodeVisitor._analyse_Assign(body_element), is_static=True))
            # todo fields: 1. DocString (DONE), 2. Assigns in body, 3. Attributes
        if get_docstring(node) is not None:  # searching fields and its types in docstring
            for string in get_docstring(node).split('\n'):
                AnalysisNodeVisitor._parse_args_from_doc(string)
                # todo: list keys!
        result_type = Type(full_name=node.name, bases=parents, fields=fields, methods=methods, lineno=node.lineno)
        types_table.append(result_type)
        return result_type

    @staticmethod
    def _analyse_FunctionDef(node):
        """
        Analysing function if it has some annotations, asserts or docstrings.
        Detection nested functions.
        """
        args_from_doc = []
        return_types = []
        _arg_names = []
        for arg in node.args.args:
            _arg_names.append(arg.arg)
            if arg.annotation is not None:  # if the method has an annotation
                args_from_doc.append((arg.arg, _parse(arg.annotation.id)))
                if node.returns is not None:
                    return_types = _parse(node.returns.id)
        if get_docstring(node) is not None:  # if it has a PyCharm-specialized documentation
            for string in get_docstring(node).split('\n'):
                if '@type ' in string or ':type ' in string:
                    argument_descibers = string.split()
                    if len(argument_descibers) < 3:
                        raise InvalidDocStringException(node.name)
                    args_from_doc.append((argument_descibers[1][:-1], _parse(argument_descibers[2])))
                elif True in (s in string for s in ('@return ', '@returns ', ':rtype')):
                    return_describers = string.split()
                    if len(return_describers) < 2:
                        raise InvalidDocStringException(node.name)
                    return_types = _parse(return_describers[1])
        elif True in (isinstance(code_elem, Assert) for code_elem in node.body):  # if it has 'assert isinstance'
            assert_elems = (code_elem for code_elem in node.body if isinstance(code_elem, Assert))
            for assert_el in assert_elems:
                if assert_el.test.func.id == 'isinstance':
                    if assert_el.test.args[0].id in _arg_names:
                        args_from_doc.append((assert_el.test.args[0].id, _parse(assert_el.test.args[1].id)))
                    else:
                        return_types.append(_parse(assert_el.test.args[1].id))
        inners = []
        for body_element in node.body:  # searching inner methods and classes
            if isinstance(body_element, FunctionDef):
                inners.append(AnalysisNodeVisitor._analyse_FunctionDef(body_element))
            elif isinstance(body_element, ClassDef):
                inners.append(AnalysisNodeVisitor._analyse_ClassDef(body_element))
        result_method = Method(node.name, args_from_doc, return_types, inners=inners, lineno=node.lineno)
        vars_table.append(result_method)
        return result_method

    def visit_ClassDef(self, node):
        # result_type = AnalysisNodeVisitor._analyse_ClassDef(node)
        self.generic_visit(self, node)
        # return result_type

    def visit_FunctionDef(self, node):
        result_func = AnalysisNodeVisitor._analyse_FunctionDef(node)
        # find methods, used by variables
        funcs_for_arguments = list(filter(lambda el: el is not None, [analyse_operator(curr_node) for curr_node in walk(node) if type(curr_node) in (BinOp, UnaryOp)]))
        # stubs for arguments
        for argum in node.args.args:
            current_type = Type(marker=argum.arg)  # default type stub
            types_table.append(current_type)
            current_argument_var = Variable(argum.arg, current_type, lineno=argum.lineno)
            for func in funcs_for_arguments:
                for num, formal in enumerate(func.formals):
                    if isinstance(formal, str) and formal == current_argument_var.name:
                        func.formals[num] = current_argument_var
        # add methods to stub types
        for stub in filter(lambda t: t.full_name == Type.default_name and t.type_id != 0, types_table):
            stub.methods.extend(filter(lambda func: stub.marker in [formal.name for formal in func.formals], funcs_for_arguments))
        return result_func

    @staticmethod
    def _analyse_Assign(node):
        content = node.targets[0]
        # getting name
        if isinstance(content, Name):
            name = content.id
        elif isinstance(content, Attribute):
            name = content.attr
            class_name = content.value.id
        # getting type
        if isinstance(node.value, Num):
            type_name = node.value.n.__class__.__name__
        elif isinstance(node.value, Str):
            type_name = node.value.s.__class__.__name__
        else:
            type_name = node.value.__class__.__name__
        return Variable(name, find_type(type_name))


def analyse_operator(node):
    if type(node) not in (BinOp, UnaryOp):
        return None
    op_name = type(node.op).__name__.lower()
    if op_name == 'mult':
        op_name = op_name[:-1]
    elif op_name.startswith('bit'):
        op_name = op_name[3:]
    elif op_name == 'uadd':
        op_name = 'pos'
    elif op_name == 'usub':
        op_name = 'neg'
    # I'll take formals to the list only if it's linear (not BinOp, UnaryOp etc.)
    formals = [oper_argument.id for oper_argument in (node.left, node.right) if isinstance(oper_argument, Name)]
    return Method('__' + op_name + '__', formals, lineno=node.left.lineno) if formals else None