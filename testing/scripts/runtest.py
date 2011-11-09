#!/usr/bin/python

# coding=UTF-8

import sys
import os.path
import os
import kumirutils

TEST_DIRS = [ "tErrors","tOur","tTOS" ]

out = sys.stdout

def read_standard_errors(filename):
    f = open(filename, 'r')
    lines = f.read().split('\n')
    f.close()
    result = []
    for line in lines:
        if line.strip()=="":
            continue
        if len(line)>0 and line[0]=='#':
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

def _read_char():
    import tty, termios, sys
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(fd)
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    if ch=='q':
        sys.stderr.write("q\nUser terminated.\n")
        sys.exit(0)
    return ch

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

def ask_differences(filename, old_errors, new_errors):
    old_db = group_errors(old_errors)
    new_db = group_errors(new_errors)
    common = set(old_db.keys()) & set(new_db.keys())
    old_only = set(old_db.keys()) - common
    new_only = set(new_db.keys()) - common
    result = []
    has_changes = False
    for line in list(common):
        e_o = old_db[line]
        e_n = new_db[line]
        if e_o.pos!=e_n.pos or e_o.end!=e_n.end or e_o.text!=e_n.text:
            sys.stderr.write("------------------\n")
            sys.stderr.write("Difference in "+filename+"\n")
            sys.stderr.write("OLD: "+str(e_o)+"\n")
            sys.stderr.write("NEW: "+str(e_n)+"\n")
            sys.stderr.write("--- What should I do? (use [o]ld, use [n]ew, just [s]kip this file): ")
            answer = _read_char()
            if answer=='s':
                sys.stderr.write("s\n")
                return None
            elif answer=='n':
                sys.stderr.write("n\n")
                has_changes = True
                result += [e_n]
            else:
                sys.stderr.write("o\n")
                result += [e_o]
            
        else:
            result += [e_o]
            
    for line in list(old_only):
        e = old_db[line]
        sys.stderr.write("------------------\n")
        sys.stderr.write("Lost error in "+filename+"\n")
        sys.stderr.write(str(e)+"\n")
        sys.stderr.write("--- What should I do? ([r]emove error from standard, [k]eep error in standard, [s]kip this file): ")
        answer = _read_char()
        if answer=='s':
            sys.stderr.write("s\n")
            return None
        elif answer=='r':
            has_changes = True
            sys.stderr.write("r\n")
        else:
            sys.stderr.write("k\n")
            result += [e]
        
    for line in list(new_only):
        e = new_db[line]
        sys.stderr.write("------------------\n")
        sys.stderr.write("New error in "+filename+"\n")
        sys.stderr.write(str(e)+"\n")
        sys.stderr.write("--- What should I do? ([a]dd error to standard, [k]eep standard without this error, [s]kip this file): ")
        answer = _read_char()
        if answer=='s':
            sys.stderr.write("s\n")
            return None
        elif answer=='a':
            has_changes = True
            sys.stderr.write("a\n")
            result += [e]
        else:
            sys.stderr.write("k\n")
        
    if has_changes:
        return result

def write_standard_file(filename, errors):
    f = open(filename, 'w')
    for error in errors:
        f.write(str(error)+'\n')
    f.close()

def process_dir(dirname):
    for filename in os.listdir(dirname):
        if not filename.endswith(".kum"):
            continue
        new_errors = kumirutils.compile_to_bytecode(dirname+"/"+filename)
        gs_name = "new_standards/"+dirname+"/"+filename+".2"
        if "-i" in sys.argv:
            if not os.path.exists(gs_name):
                sys.stderr.write("Standard not exist. Create? ([y]es or [n]o): ")
                answer = _read_char()                
                if answer=='y':
                    sys.stderr.write("y\n")
                    write_standard_file(gs_name, new_errors)
                else:
                    sys.stderr.write("n\n")
            else:
                old_errors = read_standard_errors(gs_name)
                new_standard_errors = ask_differences(dirname+"/"+filename, old_errors, new_errors)
                if not new_standard_errors is None:
                    write_standard_file(gs_name, new_standard_errors)
        else:
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
