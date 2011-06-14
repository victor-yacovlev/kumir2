import sys
import os
import os.path
import subprocess
import shlex

LRELEASE = "lrelease"

src_dir = sys.argv[1]
out_dir = sys.argv[2]


all_files = os.listdir(src_dir)
ts_files = filter(lambda x: x.endswith(".ts"), all_files)

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

for f in ts_files:
    src = src_dir+"/"+f
    out = out_dir+"/"+f[:-3]+".qm"
    cmd = LRELEASE+" "+src+" -qm "+out
    proc = subprocess.Popen(shlex.split(cmd))
    proc.wait()

