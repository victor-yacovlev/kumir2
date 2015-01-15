import sys
import run_io

sys.stdout = run_io.stdout
sys.stderr = run_io.stderr
sys.stdin = run_io.stdin

run_io.debug("Kumir <--> Python wrapper initialized.")
