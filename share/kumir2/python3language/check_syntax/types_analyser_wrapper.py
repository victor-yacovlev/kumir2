# coding=utf-8
from check_syntax.error import Error
from types_analyser import initial_checking
import ast

name = "TYPE-ANALYSER"

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


def set_source_text(text):
    global _reporter
    assert isinstance(_reporter, Reporter)
    _reporter.reset()
    assert isinstance(text, str)

    try:
        tree = ast.parse(text)
    except SyntaxError:
        return  # Do nothing. There are too many errors caught by others

    initial_checking.fill_initial()
    # visitor = initial_checking.ImportModuleVisitor()
    # visitor.visit(tree)
    node_visitor = initial_checking.AnalysisNodeVisitor()
    node_visitor.visit(tree)


def get_errors():
    global _reporter
    return _reporter.errors
