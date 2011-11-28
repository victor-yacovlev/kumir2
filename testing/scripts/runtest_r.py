#!/usr/bin/python

# coding=UTF-8

import sys
import os.path
import os
import kumirutils

TEST_DIRS = [ "tLeonovKarpov","tErrors","tKushnirenko","tKushnirenko_Nov","tOur","tTOS" ]

out = sys.stdout

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

def print_difference(gsdata, data, title):
    out.write("Difference in "+title+":\n")
    out.write("===== OLD:\n")
    out.write(gsdata+'\n')
    out.write("===== NEW:\n")
    out.write(data+'\n')

def ask_difference(filename, data, title):
    f = open(filename, 'r')
    gsdata = f.read()
    f.close()
    if gsdata!=data:
        print_difference(gsdata, data, title)
        sys.stderr.write("Write [n]ew or [k]eep old? ")
        answer = _read_char()
        if answer=='n':
            sys.stderr.write("n\n")
            f = open(filename, 'w')
            f.write(data)
            f.close()
        else:
            sys.stderr.write("k\n")
            

def process_dir(dirname):
    for filename in os.listdir(dirname):
        if not filename.endswith(".kum"):
            continue
        kodfilename = dirname+"/"+filename[0:-4]+".kod"
        kumirutils.compile_to_bytecode(dirname+"/"+filename)
        if not os.path.exists(kodfilename):
            out.write("Error creating "+kodfilename+'\n')
            continue
        else:
            indata = ""
            if os.path.exists("new_standards/"+dirname+"/"+filename+".in"):
                f = open("new_standards/"+dirname+"/"+filename+".in",'r')
                indata = f.read()
                f.close()
            new_output, new_rterror = kumirutils.run_bytecode(kodfilename, indata)
            
        out_gs_name = "new_standards/"+dirname+"/"+filename+".out"
        err_gs_name = "new_standards/"+dirname+"/"+filename+".rterr"
        

        if "-i" in sys.argv:
            if not os.path.exists(out_gs_name):
                sys.stderr.write(new_output+"\n")
                sys.stderr.write("Output standard not exist. Create? ([y]es or [n]o): ")
                answer = _read_char()
                if answer=='y':
                    sys.stderr.write("y\n")
                    f = open(out_gs_name, 'w')
                    f.write(new_output)
                    f.close()
                else:
                    sys.stderr.write("n\n")
            else:
                ask_difference(out_gs_name, new_output, "output for "+filename)
            if not os.path.exists(err_gs_name):
                sys.stderr.write(new_rterror+"\n")
                sys.stderr.write("Runtime error standard not exist. Create? ([y]es or [n]o): ")
                answer = _read_char()
                if answer=='y':
                    sys.stderr.write("y\n")
                    f = open(err_gs_name, 'w')
                    f.write(new_rterror)
                    f.close()
                else:
                    sys.stderr.write("n\n")
            else:
                ask_difference(err_gs_name, new_rterror, "runtime error for "+filename)
        else:
            if not os.path.exists(out_gs_name):
                out.write("No output standard for "+filename+"\n")
            else:
                f = open(out_gs_name, 'r')
                old_output = f.read()
                f.close()
                if old_output!=new_output:
                    print_difference(old_output, new_output, "output for "+filename)
                    out.write("----------------------------\n")
            if not os.path.exists(err_gs_name):
                out.write("No runtime error standard for "+filename+"\n")
            else:
                f = open(err_gs_name, 'r')
                old_rterror = f.read()
                f.close()
                if old_rterror!=new_rterror:
                    print_difference(old_rterror, new_rterror, "runtime error for "+filename)
                    out.write("----------------------------\n")
            
            

if __name__=="__main__":
    for arg in sys.argv:
        if arg.startswith("--out="):
            out = open(arg[6:], 'w')
    for dirname in TEST_DIRS:
        process_dir(dirname)
    out.close()
