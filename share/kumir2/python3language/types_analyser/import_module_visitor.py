__author__ = 'velkerr'

from ast import NodeVisitor


class ImportModuleVisitor(NodeVisitor):
    def __init__(self):
        self.module_names = set()

    def visit_Import(self, node):
        self.module_names.add(node.names[0].name)
        NodeVisitor.generic_visit(self, node)

    def visit_ImportFrom(self, node):
        self.module_names.add(node.module)
        NodeVisitor.generic_visit(self, node)