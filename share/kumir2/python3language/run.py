#!/usr/bin/env python3
import bdb
import io
import parser
import symbol
import sys
import pdb

try:
    import _kumir
    debug = _kumir.debug
except ImportError:
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





class Runner(bdb.Bdb):

    def __init__(self):
        super().__init__()
        self.filename = str()
        self.source = bytes()
        self.error = str()
        self.code = None
        self.statements = None
        self.current_line = -1
        self.running = False
        self.call_deep = 0

    def load_program(self, filename, source):
        self.filename = filename
        self.source = source
        self.statements = None
        self.code = None
        success = False
        try:
            self.code = compile(source, filename, "exec")
            success = True
            self.error = ""
        except SyntaxError as err:
            success = False
            self.error = err.msg
        if not self.code is None:
            unicode_source = str(source, 'utf-8')
            st = parser.suite(unicode_source)
            ptree = parser.st2tuple(st, line_info=True, col_info=True)
            self.statements = []
            for item in ptree:
                if isinstance(item, tuple) and item[0] == symbol.stmt:
                    self.statements += [item[1]]
        return success

    def __prepare_to_run(self):
        self.running = True
        self.reset()
        self.current_line = -1
        self.call_deep = 0
        try:
            self.run(self.source)
        except BaseException:
            pass

    def run_blind(self):
        self.__prepare_to_run()
        self.set_continue()
        self.running = False

    def run_continuous(self):
        if not self.running:
            self.__prepare_to_run()
        self.set_continue()
        self.running = self.call_deep > -1

    def run_step_over(self):
        self.run_step_into()

    def run_step_into(self):
        if not self.running:
            self.__prepare_to_run()
        self.set_step()
        self.running = self.call_deep > -1

    def run_step_out(self):
        self.run_step_into()

    def user_return(self, frame, retval):
        self.call_deep -= 1

    def user_call(self, frame, retval):
        self.call_deep += 1

    def user_line(self, frame):
        self.current_line = frame.f_lineno - 1
        try:
            import _kumir
            _kumir.notify_line_changed(self.current_line)
        except ImportError:
            pass

    def user_exception(self, frame, exc_info):
        self.current_line = frame.f_lineno - 1
        self.error = str(exc_info[1])



    def terminate(self):
        self.set_quit()



__runner = Runner()


def load_program(filename: str, source: bytes):
    return __runner.load_program(filename, source)


def get_error():
    return __runner.error


def has_more_instructions():
    return __runner.running


def current_line_no():
    return __runner.current_line


def run_blind():
    __runner.run_blind()


def run_continuous():
    __runner.run_continuous()


def run_step_into():
    __runner.run_step_into()


def run_step_over():
    __runner.run_step_over()


def run_step_out():
    __runner.run_step_out()


def terminate():
    __runner.terminate()

if __name__ == "__main__":
    load_program("test.py", "print(123)\nboo()".encode('utf-8'))
    run_step_over()
    run_step_over()
    sys.stderr.write(get_error())
else:
    sys.stdout = StdOut()
    sys.stderr = StdErr()

