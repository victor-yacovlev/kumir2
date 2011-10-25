#!/usr/bin/python

# coding=UTF-8

import sys
import os.path
import os
import kumirutils

TEST_DIRS = [ "tErrors" ]

out = sys.stdout

def read_standard_errors(filename):
    f = open(filename, 'r')
    lines = f.read().split('\n')
    f.close()
    result = []
    for line in lines:
        if line.strip()=="":
            continue
        terms = line.split(',', 4)
        terms = map(lambda x: x.strip(), terms)
        err = kumirutils.CompileError()
        err.line = int(terms[0][2:])
        err.pos  = int(terms[1][2:])
        err.end  = int(terms[2][2:])
        err.text = terms[3][2:].strip()
        result += [ err ]
    result.sort(key=lambda x: x.line)
    return result

def group_errors(lst):
    result = { }
    for e in lst:
        result[e.line] = e
    return result

def print_difference(filename, old_errors, new_errors):
    old_db = group_errors(old_errors)
    new_db = group_errors(new_errors)
    common = set(old_db.keys()) & set(new_db.keys())
    old_only = set(old_db.keys()) - common
    new_only = set(new_db.keys()) - common
    for line in list(common):
        e_o = old_db[line]
        e_n = new_db[line]
        if e_o.pos!=e_n.pos or e_o.end!=e_n.end or e_o.text!=e_n.text:
            out.write("Difference in "+filename+"\n")
            out.write("OLD: "+str(e_o)+"\n")
            out.write("NEW: "+str(e_n)+"\n")
            out.write("------------------\n")
    for line in list(old_only):
        e = old_db[line]
        out.write("Lost error in "+filename+"\n")
        out.write(str(e)+"\n")
        out.write("------------------\n")
    for line in list(new_only):
        e = new_db[line]
        out.write("New error in "+filename+"\n")
        out.write(str(e)+"\n")
        out.write("------------------\n")

def process_dir(dirname):
    for filename in os.listdir(dirname):
        if not filename.endswith(".kum"):
            continue
        new_errors = kumirutils.compile_to_bytecode(dirname+"/"+filename)
        gs_name = "new_standards/"+dirname+"/"+filename+".2"
        if os.path.exists(gs_name):
            old_errors = read_standard_errors(gs_name)
            print_difference(dirname+"/"+filename, old_errors, new_errors)
        else:
            out.write("No standard for "+dirname+"/"+filename+"\n")
            out.write("Suggestion:\n")
            for e in new_errors: out.write(str(e)+"\n")
            out.write("------------------\n")

if __name__=="__main__":
    for arg in sys.argv:
        if arg.startswith("--out="):
            out = open(arg[6:], 'w')
    for dirname in TEST_DIRS:
        process_dir(dirname)
    out.close()
