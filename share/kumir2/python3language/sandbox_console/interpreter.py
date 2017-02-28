import code
import sys
import threading
import json
import builtins
import sys
import copy

old_sys_exit = sys.exit


class ExitException(BaseException):
    def __init__(self, exit_code):
        self.exit_code = exit_code


class Interpreter(code.InteractiveInterpreter):

    existing_threads = {}

    def __init__(self):
        locls = {"reset": self.reset}
        super().__init__(locls)

    def runsource_separate_thread(self, async_id, eval_string):
        thread = threading.Thread(target=self.r, args=(async_id, eval_string))
        Interpreter.existing_threads[thread] = None
        thread.start()

    def r(self, async_id, eval_string):
        sys.exit = self.exit
        result = self.runsource(eval_string)
        out_message = {
            "type": "eval_return",
            "async_id": async_id,
            "more_lines_required": result
        }
        message_line = json.dumps(out_message, separators=(',', ':'))
        sys.__stdout__.write(message_line)
        sys.__stdout__.write("\n")
        sys.__stdout__.flush()

    def reset(self):
        sys.stdout.write("Restarting python interpreter...\n")
        sys.stdout.flush()
        out_message = {
            "type": "reset"
        }
        message_line = json.dumps(out_message, separators=(',', ':'))
        sys.__stdout__.write(message_line)
        sys.__stdout__.write("\n")
        sys.__stdout__.flush()

    def exit(self, exit_code):
        Interpreter.existing_threads[threading.current_thread()] = exit_code
        raise ExitException(exit_code)

    def showtraceback(self):
        _, e, _ = sys.exc_info()
        if isinstance(e, ExitException):
            pass
        else:
            super().showtraceback()

    @staticmethod
    def check_for_zombies():
        assert isinstance(Interpreter.existing_threads, dict)
        keys = Interpreter.existing_threads.keys()
        first_exit_code = None
        keys_to_remove = []
        for thread in keys:
            assert isinstance(thread, threading.Thread)
            if not thread.is_alive():
                exit_code = Interpreter.existing_threads[thread]
                keys_to_remove += [thread]
                if exit_code is not None and first_exit_code is None:
                    first_exit_code = exit_code
        for key in keys_to_remove:
            del Interpreter.existing_threads[key]
        return first_exit_code