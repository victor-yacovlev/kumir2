#!/usr/bin/env python
# coding=UTF-8

import project_spider
import sys
import os.path
import string

ENCODING = "CP1251"
OUTFILE = sys.stdout

def print_usage_and_exit(errcode):
    sys.stderr.write("Usage: \n")
    sys.stderr.write("   "+sys.argv[0]+" [--out=outfile] [--encoding=name]\n")
    sys.exit(errcode)

def __write(data):
    lines = unicode(data).encode(ENCODING).split("\n")
    winlines = string.join(lines, "\r\n")
    OUTFILE.write(winlines)

class Section:
    def __init__(self, name, checked):
        self.files = set()
        self.name = name
        self.checked = checked
        self.desktop = ""

def __add_files(component, proj):
    r = set()
    for f in component.filesmasks:
        fn = f.replace("%bindir%","bin").replace("%datadir%","share").replace("%libdir%","lib")
        fn = fn.replace("/","\\")
        if not fn.startswith("share\\applications") and not fn.startswith("share\\icons") and not fn.endswith("\\"):
            if fn.startswith("bin\\"):
                fn += ".exe"
            r.add(fn)
        
    for plugin in component.requires_kumir2:
        provider = proj.findModuleProvider(plugin)
        if len(provider)>0:
            r |= __add_files(proj.components[provider], proj)
            for p in proj.components[provider].plugins:
                r.add("lib\\kumir2\\plugins\\"+p+".dll")
                r.add("lib\\kumir2\\plugins\\"+p+".pluginspec")
                
    for lib in component.requires_libs:
        provider = proj.findLibraryProvider(lib)
        if len(provider)>0:
            r |= __add_files(proj.components[provider], proj)
            for p in proj.components[provider].libs:
                r.add("bin\\"+p+".dll")
                
    for web in component.requires_web:
        provider = proj.findWebAppProvider(web)
        if len(provider)>0:
            r |= __add_files(proj.components[provider], proj)
        
    for dirr in component.win32_extradirs:
        r.add(dirr+"\\*")
        
    r.add("bin\\mingwm10.dll")
    r.add("bin\\libgcc_s_dw2-1.dll")
    r.add("bin\\phonon4.dll")
    r.add("bin\\Qt*4.dll")
    return r

if __name__=="__main__":
    proj_root, scriptname = os.path.split(sys.argv[0])
    proj_root += "/.."
    proj_root = os.path.abspath(proj_root)
    proj = project_spider.scan_project(proj_root)
    
    for arg in sys.argv:
        if arg=="--help":
            print_usage_and_exit(0)
        elif arg.startswith("--out="):
            OUTFILE = open(arg[6:], 'w')
    __write(ur"""
Name "Кумир-2"


# General Symbol Definitions
!define REGKEY "SOFTWARE\Kumir2"
    
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "НИИСИ РАН\Кумир 2"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages 
!insertmacro MUI_LANGUAGE Russian

# Installer attributes
OutFile setup.exe
InstallDir "$PROGRAMFILES\NIISI RAS\Kumir2"
CRCCheck on
XPStyle on
ShowInstDetails show
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails show
""")
    
    sections = []
    console = Section(u"Утилиты", False)
    
    for name, c in proj.components.items():
        if len(c.bins)>0:
            if c.isconsole:
                console.files = __add_files(c, proj)
            else:
                sec = Section(c.summary["ru"], True)
                sec.files = __add_files(c, proj)
                sec.desktop = c.bins[0]
                sections += [sec]
    
    sections += [console]
    
    for sec in sections:
        __write("\nSection ")
        if not sec.checked:
            __write("/o ")
        __write("\""+sec.name+"\"\n")
        op = None
        for f in list(sec.files):
            dirr, name = os.path.split(f)
            if dirr!=op:
                __write("SetOutPath $INSTDIR"+dirr+"\n")
                op = dirr
            if f.endswith("*"):
                __write("File /r \""+f+"\"\n")
            else:
                __write("File \""+f+"\"\n")
        if len(sec.desktop)>0:
            __write("CreateDirectory \"$SMPROGRAMS\\$StartMenuGroup\"\n")
            __write("CreateShortcut \"$SMPROGRAMS\\$StartMenuGroup\\"+sec.name+".lnk\" ")
            __write("\"$INSTDIR\\bin\\"+sec.desktop+".exe\"\n")
        __write("SectionEnd\n")
