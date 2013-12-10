#!/usr/bin/env python

TS_MODULES = {
        "ExtensionSystem": "src/shared/extensionsystem",
        "DocBookViewer": "src/shared/docbookviewer",
        "KumirStdLib": "src/shared/stdlib",
        "Widgets": "src/shared/widgets",
        "ActorPainter": "src/actors/painter",
        "ActorRobot": "src/actors/robot",
        "ActorIsometricRobot": "src/actors/isometricrobot",
        "Browser": "src/plugins/browser",
        "CoreGUI": "src/plugins/coregui",
        "Editor": "src/plugins/editor",
        "KumirAnalizer": "src/plugins/kumiranalizer",
        "KumirCompilerTool": "src/plugins/kumircompilertool",
        "KumirCompiler": "src/plugins/kumircompiler",
        "KumirCodeRun": "src/plugins/kumircoderun",
        "KumirCodeGenerator": "src/plugins/kumircodegenerator",
        "KumirNativeGenerator": "src/plugins/kumirnativegenerator",
        "PascalAnalizer": "src/plugins/pascalanalizer",
#        "st_funct": "src/plugins/st_funct"
    }

TS_LANGUAGES = [ "ru" ]


import os
import os.path
import subprocess
import sys

LUPDATE = "lupdate"

if os.name=="posix":
    if os.path.exists("/usr/bin/lupdate-qt4") or os.path.exists("/usr/local/bin/lupdate-qt4"):
        LUPDATE = "lupdate-qt4"

for lang in TS_LANGUAGES:
    sys.stdout.write("Updating translations for language "+lang+"\n")
    for key, value in TS_MODULES.items():
        sys.stdout.write(key+"\n")
        SOURCES = filter(lambda x: x.endswith(".cpp") and not x.startswith("moc_"), os.listdir(value))
        FORMS = filter(lambda x: x.endswith(".ui"), os.listdir(value))
        lstfile = open(value+"/"+key+".lst", 'w')
        
        for x in SOURCES+FORMS:
            lstfile.write(value+"/"+x+"\n")

        lstfile.close()
        cmd = [LUPDATE, "-target-language", lang+"_"+lang.upper(),
                   "@"+value+"/"+key+".lst",
                   "-ts", "share/kumir2/translations/"+key+"_"+lang+".ts"]
        proc = subprocess.Popen(cmd)
        proc.communicate();
        os.unlink(value+"/"+key+".lst")
    
