import os
import sys

try:
    import pydevd
    pn = os.getenv("PY_KUMIR_DEBUG_PORT", "")
    sys.stderr.write("Connecting to debug port: " + pn + "\n")
    if pn:
        pydevd.settrace("localhost", port=int(pn), stderrToServer=True, suspend=False, overwrite_prev_trace=True)
except ImportError:
    pass

from . import event_loop

event_loop.process()
