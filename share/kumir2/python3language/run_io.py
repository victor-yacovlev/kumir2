import io
import _kumir

class StdOut(io.TextIOBase):
    def __init__(self):
        super().__init__()
        self.write = _kumir.write_output


class StdErr(io.TextIOBase):
    def __init__(self):
        super().__init__()
        self.write = _kumir.write_error


class StdIn(io.TextIOBase):
    def __init__(self):
        super().__init__()
        self.readline = _kumir.read_input


stdout = StdOut()
stderr = StdErr()
stdin = StdIn()
debug = _kumir.debug
