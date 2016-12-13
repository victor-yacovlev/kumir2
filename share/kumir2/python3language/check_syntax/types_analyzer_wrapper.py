# coding=utf-8
import inspect
import symtable
from check_syntax.error import Error
# from types_analyser import initial_checking
import ast

from types_analyzer2 import basic_type_check, typetable, functable, parsers, type_analizer_errors
from types_analyzer2 import special_internal_types

name = "TYPE-ANALYZER"

description = {
    "generic": "Type usage analyser",
    "russian": "Анализатор типизации"
}

priority = 4.0

try:
    from _kumir import debug
except ImportError:
    debug = print


class Reporter:
    def __init__(self):
        self.errors = []

    def reset(self):
        self.errors.clear()

_reporter = Reporter()

def __parse_base_types(types_table, methods_table):
    from types_analyzer2.headers import base_types_0 as module_0
    parsers.parse_module_classes(types_table, methods_table, module_0, "")
    types_table.append(special_internal_types.ModuleTypeDef(types_table))


def __parse_builtins(types_table, methods_table):
    from types_analyzer2.headers import builtins_0 as module_0
    from types_analyzer2.headers import builtins_1 as module_1
    parsers.parse_module_functions(types_table, methods_table, module_0, "")
    parsers.parse_module_functions(types_table, methods_table, module_1, "")


BASE_TYPES = typetable.TypesTable()
BASE_METHODS = functable.MethodsTable()
__parse_base_types(BASE_TYPES, BASE_METHODS)
__parse_builtins(BASE_TYPES, BASE_METHODS)
_built_symboltable = None
_errors = []

def set_source_text(text):
    global _reporter
    assert isinstance(_reporter, Reporter)
    _reporter.reset()
    assert isinstance(text, str)
    global _built_symboltable
    global _errors
    path = sys.path
    _built_symboltable, _errors = basic_type_check.build_symbol_table_for_text(text, path, "", BASE_TYPES, BASE_METHODS)


def get_errors():
    global _reporter
    return _reporter.errors

if __name__ == "__main__":
    import sys
    with open(sys.argv[1], "r") as input_file:
        input_data = input_file.read()
        set_source_text(input_data)
        assert isinstance(_built_symboltable, symtable.SymbolTable)
        annotations = dict()
        for entry in _built_symboltable.get_symbols():
            assert isinstance(entry, symtable.Symbol)
            name = entry.get_name()
            try:
                line = entry.lineno
                typedef = entry.typedef
                if typedef:
                    if not line in annotations:
                        annotations[line] = []
                    line_list = annotations[line]
                    line_list.append((name, typedef))
            except:
                pass
        lines = input_data.split("\n")
        for line_index, line in enumerate(lines):
            line_no = line_index + 1
            if line_no in annotations:
                line += "  ### "
                anno_list = annotations[line_no]
                type_annos = [ name + ": " + repr(typedef) for name, typedef in anno_list ]
                line += ", ".join(type_annos)
            for e in _errors:
                assert isinstance(e, type_analizer_errors.CompileError)
                if e.node.lineno==line_no:
                    line += " #!!!! " + repr(e)
            print(line)
