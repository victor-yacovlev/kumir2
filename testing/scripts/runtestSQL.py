# coding=UTF-8

import sys
import os
import os.path
import kumirutils
import sqlite3

# "tOur", 
DIRS = ["tErrors" ]

BaseName = 'errdb'


def find_differences_in_compile_errors(fullname, old, new):
    for oe in old:
        line = oe.line
        ne = None
        for nne in new:
            if nne.line==line:
                ne = nne
                break
        if ne is None:
            sys.stdout.write("File: "+fullname+"\n")
            sys.stdout.write("OLD: "+str(oe)+"\n")
            sys.stdout.write("NEW: \n")
            sys.stdout.write("===========\n")
        else:
            if ne.pos!=oe.pos or ne.end!=oe.end or ne.text!=oe.text:
                sys.stdout.write("File: "+fullname+"\n")
                sys.stdout.write("OLD: "+str(oe)+"\n")
                sys.stdout.write("NEW: "+str(ne)+"\n")
                sys.stdout.write("===========\n")
    for ne in new:
        line = ne.line
        oe = None
        for ooe in old:
            if ooe.line==line:
                oe = ooe
                break
        if oe is None:
            sys.stdout.write("File: "+fullname+"\n")
            sys.stdout.write("OLD: \n")
            sys.stdout.write("NEW: "+str(ne)+"\n")
            sys.stdout.write("===========\n")
            

def parse_old_compile_errors(filename1, filename2):
    f = open(filename2, 'r')
    lines = f.read().split('\n')
    f.close()
    result = {}
    lineNo = 1
    for line in lines:
        line = line.strip()
        if len(line)>0:
            err = kumirutils.CompileError()
            if line.endswith(')'):
                brpos = line.rfind('(')
                if brpos!=-1:
                    line = line[0:brpos-1].strip()
            err.line = lineNo
            err.text = line
            result[lineNo] = err
        lineNo += 1
    f = open(filename1, 'r')
    lines = f.read().split('\n')
    f.close()
    lineNo = 1
    for line in lines:
        while line.startswith(". "):
            line = line[2:].strip()
            if result.has_key(lineNo):
                start = line.find("<u>")+1
                end = line.find("</u>")-3
                result[lineNo].pos = start
                result[lineNo].end = end
        lineNo += 1
    return result.values()

def process_file(fullname, filename):
    newErrors = kumirutils.compile_to_bytecode(fullname)
    oldErrors = parse_old_compile_errors("Etalon"+os.path.sep+filename+".1","Etalon"+os.path.sep+filename+".2")
    find_differences_in_compile_errors(fullname, oldErrors, newErrors)
    
    

def process_dir(name):
    filenames = os.listdir(name)
    filenames = filter(lambda x: x.endswith(".kum"), filenames)
    for fn in filenames:
        fullname = name + os.path.sep + fn
        process_file(fullname, fn)

if __name__=="__main__":
    conn = sqlite3.connect(H_DIR+BaseName)
    for d in DIRS:
        process_dir(d)

