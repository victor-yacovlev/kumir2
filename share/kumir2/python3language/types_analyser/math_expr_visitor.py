__author__ = 'velkerr'

from ast import *
from types_analyser.analysis_node_visitor import analyse_operator

oper_table = []


class MathExprVisitor(NodeVisitor):
    def visit_BinOp(self, node):
        all_nodes = [n for n in walk(node) if isinstance(n, BinOp)]
        analyse_operator(node)

    def visit_UnaryOp(self, node):
        analyse_operator(node)

