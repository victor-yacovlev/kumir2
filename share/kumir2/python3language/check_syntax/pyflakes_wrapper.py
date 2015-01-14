# coding=utf-8
from pyflakes.api import check
from pyflakes.reporter import Reporter as ReporterBase
from check_syntax.error import Error
from pyflakes.messages import *

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


_reporter = Reporter()


def set_source_text(text):
    global _reporter
    _reporter.reset()
    check(text, "<stdin>", _reporter)


def get_errors():
    global _reporter
    return _reporter.errors