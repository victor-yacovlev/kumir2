import sys
import sandbox_io

sys.stdout = sandbox_io.stdout
sys.stderr = sandbox_io.stderr
sys.stdin = sandbox_io.stdin

sandbox_io.debug("Kumir <--> Python SandBox wrapper initialized.");
