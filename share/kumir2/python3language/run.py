#!/usr/bin/env python3
import bdb
import io
import parser
import symbol
import sys
import pdb
import threading
import token

try:
    import _kumir
    debug = _kumir.debug
    KUMIR_IMPORTED = True
except ImportError:
    KUMIR_IMPORTED = False
    import logging
    logging.root.name = "run"
    logging.root.setLevel(logging.DEBUG)
    debug = logging.debug


class StdOut(io.TextIOBase):
    def __init__(self):
        super().__init__()
        try:
            import _kumir
            self.write = _kumir.write_to_stdout
        except ImportError:
            self.write = sys.__stdout__.write


class StdErr(io.TextIOBase):
    def __init__(self):
        super().__init__()
        try:
            import _kumir
            self.write = _kumir.write_to_stderr
        except ImportError:
            self.write = sys.__stderr__.write


if KUMIR_IMPORTED:
    sys.stdout = StdOut()
    sys.stderr = StdErr()


class Runner(bdb.Bdb):

    def __init__(self):
        super().__init__()

    def user_return(self, frame, retval):
        _kumir.user_return(frame, retval)

    def user_call(self, frame, retval):
        _kumir.user_call(frame, retval)

    def user_line(self, frame):
        if _kumir.user_line(frame):
            self.set_step()
        else:
            self.set_quit()

    def user_exception(self, frame, exc_info):
        _kumir.user_exception(frame, exc_info)

    def main_loop(self, filename, source):
        self.reset()
        self.set_step()
        try:
            self.run(source)
            status = 0
        except bdb.BdbQuit:
            status = 1
        except BaseException:
            status = 2
        return status

__runner = Runner()


def main_loop(filename, source):
    return __runner.main_loop(filename, source)


def __extract_atoms_from_expression(items):
        result = []
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.atom:
                result += [(item[1][1], item[1][2])]
            elif isinstance(item, tuple):
                result += __extract_atoms_from_expression(item)
        return result


def __extract_lvalue_from_statement(items, target):
    left_parts = []
    right_parts = []
    parts = right_parts
    for item in items:
        if isinstance(item, tuple) and item[0] in [symbol.testlist_star_expr, symbol.exprlist]:
            parts += [item[1:]]
        elif isinstance(item, tuple) and item[0] == token.EQUAL:
            left_parts = parts
            right_parts = []
            parts = right_parts
    stmt_atoms = __extract_atoms_from_expression(left_parts)
    for atom_name, atom_line in stmt_atoms:
        line_atoms = target[atom_line - 1]
        line_atoms += [atom_name]


def __extract_lvalue_from_for_statement(items, target):
    for item in items:
        if isinstance(item, tuple) and item[0] == symbol.exprlist:
            for_assign_atoms = __extract_atoms_from_expression(item)
            for atom_name, atom_line in for_assign_atoms:
                line_atoms = target[atom_line - 1]
                line_atoms += [atom_name]
        elif isinstance(item, tuple) and item[0] == symbol.suite:
            __extract_lvalue_atoms(item, target)


def __extract_lvalue_atoms(root, target):
    if isinstance(root, tuple):
        if root[0] == symbol.expr_stmt:
            __extract_lvalue_from_statement(root[1:], target)
        elif root[0] == symbol.for_stmt:
            __extract_lvalue_from_for_statement(root[1:], target)
        else:
            for item in root:
                if isinstance(item, tuple):
                    __extract_lvalue_atoms(item, target)

def extract_lvalue_atoms(source):
    st = parser.suite(source)
    lines_count = len(source.split("\n")) + 1
    items = parser.st2tuple(st, line_info=True, col_info=True)
    result = list()
    for i in range(0, lines_count):
        result += [list()]
    __extract_lvalue_atoms(items, result)
    return result

if __name__ == "__main__":
    a = extract_lvalue_atoms("""
import sys

def main(arguments):
    for index, argument in arguments:
        a, b = index, argument
        print(a)
        print(b)

if __name__ == "__main__":
    ret = main(sys.argv)
    sys.exit(ret)
    """)
    print(a)


