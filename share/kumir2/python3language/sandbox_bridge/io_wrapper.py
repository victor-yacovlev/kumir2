import io
import json
import sys
import builtins
import threading

class StdOut(io.TextIOBase):
    def __init__(self):
        super().__init__()

    def write(self, *args, **kwargs):
        msg = args[0]
        env = {
            "type": "stdout",
            "string_data": msg
        }
        raw = json.dumps(env, separators=(',', ':'))
        sys.__stdout__.write(raw + "\n")
        sys.__stdout__.flush()



class StdErr(io.TextIOBase):
    def __init__(self):
        super().__init__()

    def write(self, *args, **kwargs):
        msg = args[0]
        env = {
            "type": "stderr",
            "string_data": msg
        }
        raw = json.dumps(env, separators=(',', ':'))
        sys.__stdout__.write(raw + "\n")
        sys.__stdout__.flush()




class StdIn(io.TextIOBase):
    def __init__(self):
        super().__init__()
        self.data = ""
        self.semaphore = threading.Semaphore(0)

    def readline(self, *args, **kwargs):
        msg = {
            "type": "input_request",
            "prompt": ""
        }
        raw = json.dumps(msg, separators=(',', ':'))
        sys.__stdout__.write(raw + "\n")
        sys.__stdout__.flush()
        return self._fake_readline()

    def _fake_readline(self):
        if not "\n" in self.data:
            self.semaphore.acquire()
        nl = self.data.index("\n")
        line = self.data[:nl]
        self.data = self.data[nl+1:]
        return line

    def push(self, contents):
        self.data += contents
        if "\n" in contents:
            self.semaphore.release()


def modified_input(prompt):
    msg = {
        "type": "input_request",
        "prompt": prompt
    }
    raw = json.dumps(msg, separators=(',', ':'))
    sys.__stdout__.write(raw + "\n")
    sys.__stdout__.flush()
    return sys.stdin._fake_readline()

def register():
    sys.stdout = StdOut()
    sys.stderr = StdErr()
    sys.stdin = StdIn()
    try:
        builtins.input = modified_input
    except BaseException as e:
        sys.__stderr__.write(repr(e) + "\n")
        sys.__stderr__.flush()