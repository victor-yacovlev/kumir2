#!/usr/bin/env python
# codec: UTF-8

import project_spider
import sys
import os.path

ENCODING = "CP1251"
OUTFILE = sys.stdout

def print_usage_and_exit(errcode):
    sys.stderr.write("Usage: \n")
    sys.stderr.write("   "+sys.argv[0]+" [--out=outfile] [--encoding=name]\n")
    sys.exit(errcode)

def __write(data):
    OUTFILE.write(unicode(data).encode("ENCODING"))

if __name__=="__main__":
    proj_root, scriptname = os.path.split(sys.argv[0])
    proj_root += "/.."
    proj_root = os.path.abspath(proj_root)
    proj = project_spider.scan_project(proj_root)
    
    out = sys.stdout
    for arg in sys.argv:
        if arg=="--help":
            print_usage_and_exit(0)
        elif arg.startswith("--out="):
            out = open(arg[6:], 'w')
    __write("Version: "+proj.version)
    if proj.version_extra:
        __write("+"+proj.version_extra)
    __write("\n")
    __write(u"Name: Кумир-2\n")
