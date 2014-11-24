import io
import _kumir_sandbox
import _kumir

class StdOut(io.TextIOBase):
    def __init__(self):
        super().__init__()
        self.write = _kumir_sandbox.write_output


class StdErr(io.TextIOBase):
    def __init__(self):
        super().__init__()
        self.write = _kumir_sandbox.write_error


class StdIn(io.TextIOBase):
    def __init__(self):
        super().__init__()
        self.readline = _kumir_sandbox.read_input


stdout = StdOut()
stderr = StdErr()
stdin = StdIn()
debug = _kumir.debug
