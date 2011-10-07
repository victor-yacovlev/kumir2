#!/usr/bin/env python

class ProjectModel:
    
    def __init__(self):
        self.version = ""
        self.version_extra = ""
        self.components = {}
        self.changelogs = {}
    
    def findLibraryProvider(self, libname):
        for name, item in self.components.items():
            if libname in item.libs:
                return name
        return ""
    
    def findModuleProvider(self, modname):
        for name, item in self.components.items():
            mname = name.replace("module-","").replace("actor-","")
            if mname==modname:
                return name
        return ""
    
    def findWebAppProvider(self, appname):
        for name, item in self.components.items():
            if appname in item.webapps:
                return name
        return ""
    
    def mergeIntoAllInOnePackage(self):
        all_in_one = Component()
        all_in_one.summary["en"] = "Kumir 2 Education System"
        all_in_one.description["en"] = "Second generation of well-known system Kumir"
        for name, item in self.components.items():
            for req in item.requires_qt:
                if not req in all_in_one.requires_qt:
                    all_in_one.requires_qt += [req]
            for req in item.requires_other:
                if not req in all_in_one.requires_other:
                    all_in_one.requires_other += [req]
            all_in_one.provides += [name]
            for prov in item.provides:
                if not prov in all_in_one.provides:
                    all_in_one.provides += [prov]
            for e in item.filesmasks:
                if not e in all_in_one.filesmasks:
                    all_in_one.filesmasks += [e]
            for e in item.dirs:
                if not e in all_in_one.dirs:
                    all_in_one.dirs += [e]
            for e in item.libs:
                if not e in all_in_one.libs:
                    all_in_one.libs += [e]
            for e in item.plugins:
                if not e in all_in_one.plugins:
                    all_in_one.plugins += [e]
            for e in item.buildcmds:
                if not e in all_in_one.buildcmds:
                    all_in_one.buildcmds += [e]
            for e in item.installcmds:
                if not e in all_in_one.installcmds:
                    all_in_one.installcmds += [e]
            for e in item.webapps:
                if not e in all_in_one.webapps:
                    all_in_one.webapps += [e]
            for e in item.bins:
                if not e in all_in_one.bins:
                    all_in_one.bins += [e]
            for e in item.icons:
                if not e in all_in_one.icons:
                    all_in_one.icons += [e]
            for e in item.desktopfiles:
                if not e in all_in_one.desktopfiles:
                    all_in_one.desktopfiles += [e]
            for e in item.libexecs:
                if not e in all_in_one.libexecs:
                    all_in_one.libexecs += [e]
        self.components = { "" : all_in_one }
            

import datetime

class ChangeLogEntry:
    
    def __init__(self):
        self.date = datetime.date
        self.contents = ""
    
class Component:
    
    def __init__(self):
        self.name = ""
        self.requires_kumir2 = []
        self.requires_web = []
        self.requires_libs = []
        self.requires_other = []
        self.provides = []
        self.summary = {}
        self.description = {}
        self.dirs = []
        self.filesmasks = []
        self.bins = []
        self.desktopfiles = []
        self.icons = []
        self.libs = []
        self.plugins = []
        self.isweb = False
        self.isconsole = False
        self.webapps = []
        self.win32_extradirs = []
        self.libexecs = []

import json
import os.path

def __resolve_simlink(f):
    if not os.name=="nt":
        return f
    ff = open(f)
    d = ff.read(5)
    sl = None
    if d=="link ":
        text = ff.readline()
        dirr, name = os.path.split(f)
        sl = dirr+"/"+text
    ff.close()
    if sl:
        return sl
    else:
        return f

def __read_json(filename):
    f = open(__resolve_simlink(filename), 'r')
    data = f.read()
    f.close()
    if os.path.exists("/etc/altlinux-release"):
        return json.read(data)
    else:
        return json.loads(data, "utf-8")


def __translations_list(toplevel, basename):
    result = []
    entries = os.listdir(toplevel+"/share/kumir2/translations/")
    for entry in entries:
        if entry.startswith("."):
            continue
        elif entry.endswith(".ts"):
            if entry.startswith(basename+"_"):
                result += [entry]
    return result


def __scan_library(toplevel, specfilename):
    spec = __read_json(specfilename)
    c = Component()
    c.name = spec["packagename"]
    dirr, basename = os.path.split(specfilename)
    target = basename[0:-8]
    basename = basename.lower()[0:-8]
    c.libs = [target]
    if spec.has_key("summary"):
        c.summary = spec["summary"]
    if spec.has_key("description"):
        c.description = spec["description"]
    if spec.has_key("otherfiles"):
        c.filesmasks = spec["otherfiles"]
    tsfiles = __translations_list(toplevel, target)
    
    for ts in tsfiles:
        if ts.startswith(target):
            qm = ts[0:-3]+".qm"
            fullqm = "%datadir%/kumir2/translations/"+qm
            c.filesmasks += [fullqm]
            if not "%datadir%/kumir2/translations/" in c.dirs:
                c.dirs += ["%datadir%/kumir2/translations/"]
                c.installcmds += ["mkdir -p %datadir%/kumir2/translations/"]
            c.installcmds += ["cp share/kumir2/translations/"+qm+" %datadir%/kumir2/translations/"]
    return c

def __scan_plugin(toplevel, specfilename):
    spec = __read_json(specfilename)
    if spec.has_key("nodeploy") and spec["nodeploy"]:
        return None
    c = Component()
    c.name = "module-"+spec["name"]
    if spec.has_key("libexecs"):
        c.libexecs = spec["libexecs"]
    if spec.has_key("requiresother"):
	c.requires_other = spec["requiresother"]
    srcdir = specfilename[len(toplevel)+5:]
    if srcdir.startswith("actors"):
        c.name = "actor-"
        if spec["name"].startswith("Actor"):
            c.name += spec["name"][5:]
        else:
            c.name += spec["name"] 
    dirr, basename = os.path.split(specfilename)
    target = basename[0:-11]
    basename = basename.lower()[0:-11]
    if basename.startswith("actor"):
        basename = basename[5:]
    profilename = dirr+"/"+basename+".pro"
    
    if spec.has_key("libs"):
        c.requires_libs = spec["libs"]
    if not "ExtensionSystem" in c.requires_libs:
        c.requires_libs += ["ExtensionSystem"]
    if spec.has_key("requires"):
        c.requires_kumir2 = spec["requires"]
    if spec.has_key("provides"):
        c.provides = spec["provides"]
    if spec.has_key("summary"):
        c.summary = spec["summary"]
    if spec.has_key("description"):
        c.description = spec["description"]
    if spec.has_key("otherfiles"):
        c.filemasks = spec["otherfiles"]
    tsfiles = __translations_list(toplevel, target)
    for ts in tsfiles:
        if ts.startswith(target):
            qm = ts[0:-3]+".qm"
            fullqm = "%datadir%/kumir2/translations/"+qm
            c.filesmasks += [fullqm]
            if not "%datadir%/kumir2/translations/" in c.dirs:
                c.dirs += ["%datadir%/kumir2/translations/"]
                c.installcmds += ["mkdir -p %datadir%/kumir2/translations/"]
            c.installcmds += ["cp share/kumir2/translations/"+qm+" %datadir%/kumir2/translations/"]
    for lx in c.libexecs:
        c.installcmds += ["mkdir -p %libexecdir%/kumir2/"]
        c.installcmds += ["cp libexec/kumir2/"+lx+" %libexecdir%/kumir2/"]
    if os.path.exists(toplevel+"/share/kumir2/"+basename):
        c.dirs += ["%datadir%/kumir2/"+basename]
        c.filesmasks += ["%datadir%/kumir2/"+basename+"/*"]
    return c

def __extract_icon_from_desktop_file(filename):
    f = open(filename)
    lines = f.read().split("\n")
    f.close()
    for line in lines:
        if line.startswith("Icon="):
            return line[5:].strip()
    return ""

def __get_icon_dirs(toplevel, basename):
    SIZES = ["128x128", "64x64", "256x256", "scalable"]
    result = []
    for size in SIZES:
        if os.path.exists(toplevel+"/app_icons/linux/hicolor/"+size+"/apps/"+basename+".png"):
            result += [ (basename+".png", "/hicolor/"+size+"/apps") ]
    return result

def __scan_application(toplevel, specfilename):
    spec = __read_json(specfilename)
    c = Component()
    c.name = spec["packagename"]
    dirr, basename = os.path.split(specfilename)
    target = basename[0:-8]
    basename = basename.lower()[0:-8]
    
    c.bins += [target]
    if spec.has_key("win32_extradirs"):
        c.win32_extradirs = spec["win32_extradirs"]
    if spec.has_key("requires"):
        c.requires_kumir2 = spec["requires"]
    if spec.has_key("webapps"):
        c.requires_web = spec["webapps"]
    c.isconsole = spec["console"]
    c.filesmasks += ["%bindir%/"+target]
    if spec.has_key("desktopfile"):
        c.desktopfiles += [spec["desktopfile"]]
        c.filesmasks += ["%datadir%/applications/"+spec["desktopfile"]]
        icon = __extract_icon_from_desktop_file(toplevel+"/"+spec["desktopfile"])
        if len(icon)>0:
            icondirs = __get_icon_dirs(toplevel, icon)
            for f, d in icondirs:
                c.dirs += ["%datadir%/icons"+d]
                c.filesmasks += ["%datadir%/icons"+d+"/"+f]
        c.icons += [icon]
        c.dirs += ["%datadir%/applications"]
    if spec.has_key("summary"):
        c.summary = spec["summary"]
    if spec.has_key("description"):
        c.description = spec["description"]
    return c

def __scan_webapplication(toplevel, specfilename):
    dirr, fn = os.path.split(specfilename)
    dirr = dirr[len(toplevel)+1:]
    basename = fn[0:-8]
    spec = __read_json(specfilename)
    c = Component()
    c.name = spec["packagename"]
    c.webapps += [basename]
    c.dirs += ["%datadir%/kumir2/webapps/"+basename+"/"]
    c.filesmasks += ["%datadir%/kumir2/webapps/"+basename+"/*"]
    
    if spec.has_key("extrafiles"):
        extrafiles = spec["extrafiles"]
        for ef in extrafiles:
            c.installcmds += ["mkdir -p "+ef["targetdir"]]
            c.installcmds += ["cp "+ef["source"]+" "+ef["targetdir"]]
            if not ef["targetdir"] in c.dirs:
                c.dirs += [ef["targetdir"]]
    c.isweb = True
    if spec.has_key("summary"):
        c.summary = spec["summary"]
    if spec.has_key("description"):
        c.description = spec["description"]
    return c

def __scan_dir(toplevel, dirname):
    result = []
    entries = os.listdir(dirname)
    for entry in entries:
        if entry.startswith("."):
            continue
        elif os.path.isdir(dirname+"/"+entry):
            subcomponents = __scan_dir(toplevel, dirname+"/"+entry)
            result += subcomponents
        elif entry.endswith(".libspec"):
            libcomponent = __scan_library(toplevel, dirname+"/"+entry)
            result += [libcomponent]
        elif entry.endswith(".pluginspec"):
            plugincomponent = __scan_plugin(toplevel, dirname+"/"+entry) 
            if plugincomponent:
                result += [plugincomponent]
        elif entry.endswith(".appspec"):
            appcomponent = __scan_application(toplevel, dirname+"/"+entry) 
            result += [appcomponent]
        elif entry.endswith(".webspec"):
            webcomponent = __scan_webapplication(toplevel, dirname+"/"+entry) 
            result += [webcomponent]
    return result

def __read_version_h(filename):
    f = open(filename, 'r')
    lines = f.read().split('\n')
    f.close()
    maj = None
    minor = None
    rel = None
    alpha = None
    beta = None
    for line in lines:
        if line.startswith("#define VERSION_MAJOR"):
            maj = line[len("#define VERSION_MAJOR"):].strip()
        elif line.startswith("#define VERSION_MINOR"):
            minor = line[len("#define VERSION_MINOR"):].strip()
        elif line.startswith("#define VERSION_RELEASE"):
            rel = line[len("#define VERSION_RELEASE"):].strip()
        if line.startswith("#define VERSION_ALPHA"):
            alpha = line[len("#define VERSION_ALPHA"):].strip()
        if line.startswith("#define VERSION_BETA"):
            beta = line[len("#define VERSION_BETA"):].strip()
    version = maj+"."+minor+"."+rel
    extra = ""
    if alpha=="0" and beta!="0":
        extra = "beta"+beta
    elif alpha!="0" and beta=="0":
        extra = "alpha"+alpha
    return version, extra

def __merge_components(lst):
    result = {}
    for item in lst:
        name = item.name
        if not result.has_key(name):
            result[name] = item
        else:
            c = result[name]
            for r in item.requires_qt:
                if not r in c.requires_qt:
                    c.requires_qt += [r]
            for r in item.requires_kumir2:
                if not r in c.requires_kumir2:
                    c.requires_kumir2 += [r]
            for r in item.requires_other:
                if not r in c.requires_other:
                    c.requires_other += [r]
            for r in item.provides:
                if not r in c.provides:
                    c.provides += [r]
            for r in item.dirs:
                if not r in c.dirs:
                    c.dirs += [r]
            for r in item.filesmasks:
                if not r in c.filesmasks:
                    c.filesmasks += [r]
            for r in item.libs:
                if not r in c.libs:
                    c.libs += [r]
            for r in item.webapps:
                if not r in c.webapps:
                    c.webapps += [r]
            for r in item.plugins:
                if not r in c.plugins:
                    c.plugins += [r]
            for r in item.buildcmds:
                if not r in c.buildcmds:
                    c.buildcmds += [r]
            for r in item.installcmds:
                if not r in c.installcmds:
                    c.installcmds += [r]
            result[name] = c
    return result

def __read_changelog(filename):
    f = open(filename)
    lines = f.read().split("\n");
    f.close();
    result = []
    cur = None
    for line in lines:
        line = line.strip()
        if line.startswith("#"):
            continue
        elif line.startswith("---"):
            date = line[3:].strip().split("/")
            if not cur is None:
                result += [cur]
            cur = ChangeLogEntry()
            year = int(date[0])
            month = int(date[1])
            day = int(date[2])
            cur.date = datetime.date(year, month, day)
        elif cur:
            if len(cur.contents)>0:
                cur.contents += "\n"
            cur.contents += line
    return result 

def scan_project(projectdirname):
    components_src = __scan_dir(projectdirname, projectdirname+"/src")
    components_srcwww = __scan_dir(projectdirname, projectdirname+"/src-www")
    cs = ProjectModel()
    cs.components = __merge_components(components_src + components_srcwww)
    cs.version, cs.version_extra = __read_version_h(projectdirname+"/src/VERSION.h")
    cs.changelogs["en"] = __read_changelog(projectdirname+"/CHANGELOG")
    return cs

