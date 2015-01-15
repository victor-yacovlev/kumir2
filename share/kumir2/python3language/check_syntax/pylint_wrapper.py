# coding=utf-8
from astroid.builder import AstroidBuilder
from logilab.common.interface import implements
from pylint import lint
from pylint.interfaces import *
from pylint.reporters import BaseReporter
from check_syntax.error import Error
import re

name = "PyLint"

description = {
    "generic": "PyLint error checker",
    "russian": "Анализатор PyLint"
}

_not_errors = [
    "C0103",  # Invalid module name
    "C0304",  # Final newline missing
    "C0111",  # Missing module docstring
    "W0622",  # Redefining built-in ...
    "W0401",  # Wildcard import
    "W0614",  # Unused import ... from wildcard import
    "W0613",  # Unused argument
    "W0621",  # Redefining name from outer scope
    "W0612",  # Undefined variable
    "W0704",  # Except doesn't do anything
]

priority = 10.0

_reporter = None
_linter = None
_walker = None
_token_checkers = None
_raw_checkers = None


class Reporter(BaseReporter):
    __implements__ = IReporter

    def __init__(self, output=None):
        super().__init__(output)
        self.errors = []

    def set_output(self, output=None):
        super().set_output(output)

    def _display(self, layout):
        pass

    def writeln(self, string=''):
        super().writeln(string)

    def on_set_current_module(self, module, filepath):
        super().on_set_current_module(module, filepath)

    def display_results(self, layout):
        super().display_results(layout)

    def on_close(self, stats, previous_stats):
        super().on_close(stats, previous_stats)

    def add_message(self, msg_id, location, msg):
        # TODO select only error messages
        if msg_id[0] not in "C" and msg_id not in _not_errors:
            self.errors += [Error(location[3]-1, location[4], 1, msg, msg_id)]

    def reset(self):
        self.errors.clear()

    def improve_errors_positions(self, source_text):
        lines = source_text.split('\n')
        for error in self.errors:
            line = lines[error.line_no]
            name_match = re.search(r"\s'(\w+)'", error.message)
            found_exact_position = False
            search_literal = False
            if not name_match:  # second chance...
                name_match = re.search(r'"(\S+)"\sis\b', error.message)
                search_literal = True
            if name_match is not None:
                a0, b0 = name_match.span(0)
                a1, b1 = name_match.span(1)
                lexem_to_find = error.message[a1:b1]
                if search_literal:
                    lexem_match = re.search(r"([\"']"+lexem_to_find+r"[\"'])", line, error.start_pos)
                else:
                    lexem_match = re.search(r"\b("+lexem_to_find+r")\b", line, error.start_pos)
                if lexem_match is not None:
                    start, end = lexem_match.span(1)
                    error.start_pos = start
                    error.length = end - start
                    error.message = str(error.message[:a0] + error.message[b0:]).strip().capitalize()
                    if error.message.endswith("."):
                        error.message = error.message[:-1]
                    found_exact_position = True
            if not found_exact_position:
                # can't determine exact error position, so
                # mark whole line (except leading spaces)
                error.start_pos = 0
                for char in line:
                    if ' ' == char:
                        error.start_pos += 1
                    else:
                        break
                error.length = len(line) - error.start_pos




_reporter = Reporter()


# noinspection PyBroadException
def set_source_text(text):
    global _reporter
    global _linter
    assert isinstance(_reporter, Reporter)
    assert isinstance(_linter, lint.PyLinter)

    _reporter.reset()
    builder = AstroidBuilder()
    try:
        astroid_module = builder.string_build(text)
    except:
        return

    _linter.set_current_module("<stdin>")
    _linter.check_astroid_module(astroid_module, _walker, _raw_checkers, _token_checkers)
    assert isinstance(text, str)
    _reporter.improve_errors_positions(text)


def get_errors():
    global _reporter
    return _reporter.errors


def _init_linter():
    global _linter
    global _walker
    global _token_checkers
    global _raw_checkers
    global _reporter
    _linter = lint.PyLinter(reporter=_reporter)
    _linter.load_default_plugins()
    _walker = lint.PyLintASTWalker(_linter)
    checkers = _linter.prepare_checkers()
    _token_checkers = [c for c in checkers if implements(c, ITokenChecker) and c is not _linter]
    _raw_checkers = [c for c in checkers if implements(c, IRawChecker)]
    for checker in checkers:
        checker.open()
        if implements(checker, IAstroidChecker):
            _walker.add_checker(checker)


_init_linter()