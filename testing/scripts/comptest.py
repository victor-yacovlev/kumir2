#!/usr/bin/env python

#from kumirutils import *
import os, sys, fnmatch
import shutil
import kumirutils

# mask = '.kum'
TEST_PATH = '/home/victor/off2/' #Path to test dir
TEST_DIR = ['tErrors']           #List of test dir
OUT_DIR = 'rOUT'                 #Output dir
ETALON_DIR = 'new_standards'     #Path to new etalons dir                
#KUMIR_DIR = '/home/victor/kumir2/build/'
#out = sys.stdout

def find_differences_in_compile_errors(fullname, old, new):
    listdiff = []
    difnum = 1
    for oe in old:
        line = oe.line
        ne = None
        for nne in new:
            if nne.line==line:
                ne = nne
                break
        if ne is None:
            #out.write("OLD: "+str(oe)+"\n")
            #out.write("NEW: \n")
            #out.write("----------------\n")
            listdiff += ["OLD: "+str(oe)+"\n"]
            difnum +=1
            listdiff += ["NEW: \n"]
            difnum +=1
            listdiff += ["----------------\n"]
            difnum +=1
        else:
            if ne.pos!=oe.pos or ne.end!=oe.end or ne.text!=oe.text:
                #out.write("OLD: "+str(oe)+"\n")
                #out.write("NEW: "+str(ne)+"\n")
                #out.write("----------------\n")
                listdiff += ["OLD: "+str(oe)+"\n"]
                difnum +=1
                listdiff += ["NEW: "+str(ne)+"\n"]
                difnum +=1
                listdiff += ["----------------\n"]
                difnum +=1

    for ne in new:
        line = ne.line
        oe = None
        for ooe in old:
            if ooe.line==line:
                oe = ooe
                break
        if oe is None:
            #out.write("OLD: \n")
            #out.write("NEW: "+str(ne)+"\n")
            #out.write("----------------\n")
            listdiff += ["OLD: \n"]
            difnum +=1
            listdiff += ["NEW: "+str(ne)+"\n"]
            difnum +=1
            listdiff += ["----------------\n"]
            difnum +=1

    if listdiff:
            out.write("File: "+fullname+"\n")
            for dd in listdiff:
                out.write(dd)
            out.write("========================\n")
    
def pars_file(filename):
    f = open (filename, 'r')
    lines = f.read().split('\n')
    f.close()
    result = {}
    nerr=kumirutils.CompileError()
    lineNO=1
    for line in lines:
        if len(line)>0:
            pl = line.split(',', 4)
            nerr.line=int(pl[0][2:])
            nerr.pos=int(pl[1][3:])
            nerr.end=int(pl[2][3:])
            nerr.text=pl[3][3:]
            result[lineNO]=nerr
            lineNO +=1
    return result.values()

os.chdir(TEST_PATH)


print 'Start compare'
print 'Carent path:', os.getcwd()
out = open('newreport.txt', 'w')

for Tname in os.listdir(OUT_DIR):
        print 'Compare file', Tname
        if Tname[-5:] == 'kum.2':
                fn = OUT_DIR+os.path.sep+Tname
                newerr = pars_file(fn)
                fe = ETALON_DIR+os.path.sep+Tname
                if os.path.exists(fe):
                    etaerr = pars_file(fe)
                    find_differences_in_compile_errors(Tname, etaerr, newerr)
                else:
                    out.write("File: "+Tname+"\n")
                    out.write("OLD: Etalon absent\n")
                    out.write("NEW: \n")
                    out.write("===========\n")

out.close
# zapusk *.kod
