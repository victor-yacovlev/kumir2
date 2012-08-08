#!/usr/bin/env python

import sys
import os.path
from subprocess import Popen, PIPE

GIT = "git"
OUT = sys.stdout
CWD = "."

for arg in sys.argv:
    if arg.startswith("--git="):
        GIT = arg[6:]
    if arg.startswith("--out="):
        if os.path.exists(arg[6:]):
            sys.exit(0) # for faster re-build
        OUT = open(arg[6:], "w")
    if arg.startswith("--root="):
        CWD = arg[7:]

COMMAND = "$GIT --no-pager show".replace("$GIT", GIT)

process = Popen(COMMAND.split(), stdin=PIPE, stdout=PIPE, stderr=PIPE, cwd=CWD)
out, err = process.communicate()
err = err.strip()
sys.stderr.write(err)
if not err:
    lines = out.split("\n")
    for line in lines:
        if line.startswith("commit "):
            GIT_HASH = line[7:].strip()
            OUT.write("#define GIT_HASH \""+GIT_HASH+"\"\n")
        if line.startswith("Date: "):
            DATE = line[6:].strip()
            OUT.write("#define GIT_LAST_MODIFIED \""+DATE+"\"\n")

OUT.close()