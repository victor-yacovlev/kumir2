#!/usr/bin/env python

import project_spider
import sys
import os.path

def print_usage_and_exit(errcode):
    sys.stderr.write("Usage: \n")
    sys.stderr.write("   "+sys.argv[0]+" [--out=outfile]\n")
    sys.exit(errcode)

if __name__ == '__main__':
    WEEKDAYS = ["Mon","Tue","Wed","Thu","Fri","Sat","Sun"]
    MONTH = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    out = sys.stdout
    for arg in sys.argv:
        if arg.startswith("--help") or arg.startswith("-h"):
            print_usage_and_exit(0)
        elif arg.startswith("--out="):
            out = open(arg[6:], 'w')
            
    proj_root, scriptname = os.path.split(sys.argv[0])
    proj_root += "/.."
    proj_root = os.path.abspath(proj_root)
    proj = project_spider.scan_project(proj_root)
    changelog_en = proj.changelogs["en"]
    for entry in changelog_en:
        out.write("* "+WEEKDAYS[entry.date.weekday()]+
                                " "+MONTH[entry.date.month-1]+
                                " "+str(entry.date.day)+
                                " "+str(entry.date.year)+
                                " - Kumir Team <kumir@lpm.org.ru>\n")
        out.write(entry.contents)
        if not entry.contents.endswith("\n"):
            out.write("\n")
        out.write("\n")
    out.close()