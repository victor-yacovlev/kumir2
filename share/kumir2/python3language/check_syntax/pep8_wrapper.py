# coding=utf-8
from check_syntax.error import Error
import pep8
import re

name = "PEP-8"

description = {
    "generic": "PEP-8 checker",
    "russian": "Анализатор PEP-8"
}

priority = 0.0


class Report(pep8.BaseReport):
    def __init__(self, options):
        super().__init__(options)
        self.errors = []

    def stop(self):
        super().stop()

    def print_statistics(self, prefix=''):
        super().print_statistics(prefix)

    def start(self):
        super().start()
        self.errors.clear()

    def print_benchmark(self):
        super().print_benchmark()

    def increment_logical_line(self):
        super().increment_logical_line()

    def get_file_results(self):
        return super().get_file_results()

    def init_file(self, filename, lines, expected, line_offset):
        super().init_file(filename, lines, expected, line_offset)

    def get_count(self, prefix=''):
        return super().get_count(prefix)

    def error(self, line_number, offset, text, check):
        assert isinstance(text, str)
        first_space_pos = text.find(' ')
        msg_id = text[:first_space_pos]
        message = text[first_space_pos + 1:].capitalize()
        self.errors += [Error(line_number - 1, offset, 1, message, msg_id)]
        return super().error(line_number, offset, text, check)

    def get_statistics(self, prefix=''):
        return super().get_statistics(prefix)

    def improve_errors_positions(self, source_text):
        lines = source_text.split('\n')
        for error in self.errors:
            line = lines[error.line_no]
            found_exact_position = False
            if error.start_pos < len(line):
                found_exact_position = "line" not in error.message
            if not found_exact_position:
                # can't determine exact error position, so
                # mark whole line (except leading spaces)
                error.start_pos = 0
                error.length = len(line) - error.start_pos


def set_source_text(text):
    global _reporter
    assert isinstance(text, str)
    text_lines = [line + '\n' for line in text.split('\n')]
    text_lines[-1] = text_lines[-1][:-1]
    style_guide = pep8.StyleGuide(parse_argv=False, config_file=False)
    options = style_guide.options
    _reporter = Report(options)
    options.report = _reporter
    style_guide.input_file(None, text_lines)
    _reporter.improve_errors_positions(text)


def get_errors():
    global _reporter
    return _reporter.errors