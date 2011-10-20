#!/usr/bin/env python

from kumirutils import *
import os, sys, fnmatch
import shutil


# mask = '.kum'
TEST_PATH = '/home/victor/off2/' #Path to test dir
TEST_DIR = ['tErrors']           #List of test dir
OUT_DIR = 'rOUT'                 #Output dir
ETALON_DIR = 'new_standards'     #Path to new etalons dir                
#KUMIR_DIR = '/home/victor/kumir2/build/'
out = sys.stdout
def find_differences_in_compile_errors(fullname, old, new):
    for oe in old:
        line = oe.line
        ne = None
        for nne in new:
            if nne.line==line:
                ne = nne
                break
        if ne is None:
            out.write("File: "+fullname+"\n")
            out.write("OLD: "+str(oe)+"\n")
            out.write("NEW: \n")
            out.write("===========\n")
        else:
            if ne.pos!=oe.pos or ne.end!=oe.end or ne.text!=oe.text:
                out.write("File: "+fullname+"\n")
                out.write("OLD: "+str(oe)+"\n")
                out.write("NEW: "+str(ne)+"\n")
                out.write("===========\n")
    for ne in new:
        line = ne.line
        oe = None
        for ooe in old:
            if ooe.line==line:
                oe = ooe
                break
        if oe is None:
            out.write("File: "+fullname+"\n")
            out.write("OLD: \n")
            out.write("NEW: "+str(ne)+"\n")
            out.write("===========\n")

os.chdir(TEST_PATH)
print 'Carent path:', TEST_PATH

for dd in os.listdir(TEST_PATH):
        if dd == OUT_DIR:
                shutil.rmtree(dd)
                print 'Delete', dd
        else:
                print dd

#os.open

os.mkdir(OUT_DIR)

for Tdir in TEST_DIR:
        for Tname in os.listdir(Tdir):
                if Tname[-3:] == 'kod' :
                        os.remove(Tdir+'/'+Tname)
                        print 'File: ', Tname, 'deleted'
                else:
                        objerr = compile_to_bytecode(Tdir+'/'+Tname)
                        #rf = OUT_DIR+'/'+Tname+'.err.txt'
                        rf = OUT_DIR+'/'+Tname+'.2'
                        print "Open file: ", rf
                        f = open (rf, 'w')
                        for Err in objerr:
                                #tmpstr = 'L:'+str(Err.line) +', S:'+ str(Err.pos)+ ', E:'+ str(Err.end)+ ', T:'+ Err.text+'\n'
                                tmpstr = str(Err)+'\n'
                                f.write (tmpstr)
                        f.close
                        print "Close file"

print 'Start compare'
print 'Carent path:', os.getcwd()
out = open('newreport.txt', 'w')

for Tname in OUT_DIR:
        fn = OUT_DIR+os.path.sep+Tname
        fnew = open (fn, 'r')
        newlines = fnew.read().split('\n')
        fnew.close()
        fe = ETALON_DIR+os.path.sep+Tname 
        fetalon = open (fe, 'r')
        etalines = fetalon.read().split('\n')
        fetalon.close()
        find_differences_in_compile_errors(Tname, etalines, newlines)

out.close
# zapusk *.kod
