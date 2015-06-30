# coding=utf-8
from pyflakes.api import check
from pyflakes.reporter import Reporter as ReporterBase
from check_syntax.error import Error
from pyflakes.messages import *

name = "PyFlakes"

description = {
    "generic": "PyFlakes error checker",
    "russian": "Анализатор PyFlakes"
}

priority = 5.0


class Reporter(ReporterBase):
    def __init__(self):
        self.errors = []
        self.reset()

    def unexpectedError(self, filename, msg):
        pass

    def syntaxError(self, filename, msg, lineno, offset, text):
        if text[offset] == ' ':
            start = 0
            length = len(text)
            if text.endswith('\n'):
                length -= 1
        else:
            start = offset
            length = 1
        message = str(msg).capitalize()
        self.errors += [Error(lineno - 1, start, length, message)]

    def flake(self, message):
        if isinstance(message, UndefinedName):
            self.errors += [Error(message.lineno - 1, message.col, len(message.message_args[0]), "Undefined name")]

    def reset(self):
        self.errors.clear()

    def improve_errors_positions(self, source_text):
        lines = source_text.split('\n')
        for error in self.errors:
            line = lines[error.line_no]
            assert isinstance(line, str)
            if error.start_pos >= len(line):
                error.start_pos = 0
                for char in line:
                    if ' ' == char:
                        error.start_pos += 1
                    else:
                        break
                error.length = len(line) - error.start_pos


_reporter = Reporter()


def set_source_text(text):
    global _reporter
    assert isinstance(_reporter, Reporter)
    _reporter.reset()
    check(text, "<stdin>", _reporter)
    _reporter.improve_errors_positions(text)


def get_errors():
    global _reporter
    return _reporter.errors