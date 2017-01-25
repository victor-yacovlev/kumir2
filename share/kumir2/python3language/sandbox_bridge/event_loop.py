import sys
import json
import importlib

cout = sys.__stdout__
cin = sys.__stdin__
cerr = sys.__stderr__

preloaded_modules = {}

def do_pong():
    out_message = {
        "type": "pong"
    }
    message_line = json.dumps(out_message, separators=(',', ':'))
    cout.write(message_line)
    cout.write("\n")
    cout.flush()

def do_function_call(module_name, function_name, arguments):
    try:
        if module_name in preloaded_modules:
            module = preloaded_modules[module_name]
        else:
            module = importlib.import_module(module_name)
            preloaded_modules[module_name] = module
        function = module.__dict__[function_name]
        arguments_tuple = tuple(arguments)
        result = function(*arguments_tuple)
        out_message = {
            "type": "blocking_return",
            "return_value": result
        }
    except BaseException as e:
        error = repr(e)
        out_message = {
            "type": "exception",
            "string_data": error
        }
    message_line = json.dumps(out_message, separators=(',', ':'))
    cout.write(message_line)
    cout.write("\n")
    cout.flush()

def process():
    while True:
        message_line = str(cin.readline()).strip()
        if message_line:
            message = json.loads(message_line)
            cmd = message["type"].lower()
            if "exit" == cmd:
                cerr.write("Bye!")
                sys.exit(0)
            elif "ping" == cmd:
                do_pong()
            elif "blocking_call" == cmd:
                module_name = message["module_name"]
                function_name = message["function_name"]
                arguments = message["arguments"]
                do_function_call(module_name, function_name, arguments)
            else:
                cerr.write("Unknown message type {}\n".format(cmd))
