import sys
import sandbox_io
import sandbox_functions

sys.stdout = sandbox_io.stdout
sys.stderr = sandbox_io.stderr
sys.stdin = sandbox_io.stdin

reset = sandbox_functions.reset

sandbox_io.debug("Kumir <--> Python SandBox wrapper initialized.");


