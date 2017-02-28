import code
import sys
import threading
import json
import builtins

class Interpreter(code.InteractiveInterpreter):
    def __init__(self):
        locls = { "reset": self.reset }
        super().__init__(locls)

    def runsource_separate_thread(self, async_id, eval_string):
        thread = threading.Thread(target=self.r, args=(async_id, eval_string))
        thread.start()

    def r(self, async_id, eval_string):
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