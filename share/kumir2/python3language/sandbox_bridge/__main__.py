import os
import sys


sys.stderr.write("Launching Python interpreter bridge...\n")
sys.stderr.write("My PATH: " + repr(sys.path) + "\n")
sys.stderr.flush()


try:
    import pydevd
    pn = os.getenv("PY_KUMIR_DEBUG_PORT", "")
    sys.stderr.write("Connecting to debug port: " + pn + "\n")
    sys.stderr.flush()
    if pn:
        pydevd.settrace("localhost", port=int(pn), stderrToServer=True, suspend=False, overwrite_prev_trace=True)
except ImportError as e:
    sys.stderr.write("Module 'pydevd' is not available: " + repr(e) + "\n")
    sys.stderr.flush()

from . import event_loop
from . import io_wrapper

io_wrapper.register()
event_loop.process()
