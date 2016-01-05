__author__ = 'velkerr'

import parser
import sys
import os
import platform
import inspect
import types
from ast import *

# For debug
import meta.asttools as at
import types_analyser.astpp
###

from types_analyser.analysis_node_visitor import AnalysisNodeVisitor, process_undefined_types, shrink_table
from types_analyser.math_expr_visitor import MathExprVisitor
from types_analyser.node_analysis import fill_initial, Module, parsing_file
from types_analyser.import_module_visitor import ImportModuleVisitor
from types_analyser.node_analysis import types_table, vars_table

try:
    from _kumir import debug
except ImportError:
    debug = print

# root directory for finding lib modules
root_dir = os.environ['PYTHONPATH'].split(os.pathsep)[0] if platform.system() == 'Windows' else '/'
modules = Module()


def parse_tree(fname):
    with open(fname) as f:
        source = f.read()
    f.close()
    return parser.st2tuple(parser.suite(source))


def parse_ast(fname):
    return parse(open(fname).read())


def get_imported_code():
    for name, val in globals().items():
        if isinstance(val, types.ModuleType):
            try:
                at.print_ast(parse_ast(inspect.getsourcefile(val)))
            except:
                pass


def get_source(module_name): # очень долго работает из-за того, что ищет все пути
    for root, dirs, files in os.walk(root_dir, topdown=False):
        for name in files:
            if module_name == name:
                my_module = open(os.path.join(root, name), 'r')
    return my_module.read()


# todo. This code will be pasted into run_static_analisys() in module static_analysis
def main(fname):
    modules = Module(0, Module.get_name(fname))
    try:
        tree = parse_ast(fname)
    except SyntaxError:
        debug("Gross errors was found. Exit")
    else:
        debug("File is correct in general. Let's move further!")
        fill_initial()
        # visitor = AnalysisNodeVisitor()
        # visitor.visit(tree)
        # visitor = ImportModuleVisitor()
        # visitor.visit(tree)
        at.print_ast(tree)
        node_visitor = AnalysisNodeVisitor()
        node_visitor.visit(tree)

        binop_visitor = MathExprVisitor()
        binop_visitor.visit(tree)

        parsing_file(fname)

        # for val in visitor.module_names:
        #     debug(sys.path(val))
            # try:
            #     debug(inspect.getsourcefile(val))
            # except:
            #     debug('sf')
        process_undefined_types()
        shrink_table()
        # [debug(var.name) for var in vars_table]
