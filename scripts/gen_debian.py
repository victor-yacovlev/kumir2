#!/usr/bin/env python

import project_spider
import sys
import os.path
import string
import os
import stat


QT_MIN_VERSION = "4.6.3"
QTWEBKIT_MIN_VERSION = "2.0.0"
PACKAGER = None
    
def gen_changelog(proj):
    WEEKDAYS = ["Mon","Tue","Wed","Thu","Fri","Sat","Sun"]
    MONTH = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    result = ""
    baseline = "kumir2 ("+proj.version
    if len(proj.version_extra)>0:
        baseline += "+"+proj.version_extra+") unstable"
    else:
        baseline += ") stable"
    baseline += "; urgency=low\n"
    for entry in proj.changelogs["en"]:
        result += baseline
        result += "\n"
        lines = entry.contents.split("\n")
        for line in lines:
            line = line.strip()
            if line.startswith("- "):
                line = "* "+line[2:]
            if line.startswith("*"):
                result += "  "+line+"\n"
            else:
                result += "\t"+line+"\n"
        result += " -- Kumir Team <kumir@lpm.org.ru>  "
        result += WEEKDAYS[entry.date.weekday()]+", "
        day = str(entry.date.day)
        if len(day)==1:
            day = "0"+day
        result += day + " "
        result += MONTH[entry.date.month-1]
        result += " "+str(entry.date.year)
        result += " 00:00:00 +0400\n\n"
        
    return result

def gen_compat():
    return "7\n"

def __debian_name(name):
    return name.lower().replace("_","")

def gen_control(proj):
    result = ""
    result += "Source: kumir2\n"
    result += "Section: science\n"
    result += "Priority: optional\n"
    result += "Maintainer: "+PACKAGER+"\n"
    result += "Standards-Version: 3.9.1\n"
    result += "Homepage: http://www.niisi.ru/kumir/\n"
    result += "Build-Depends: debhelper (>= 7.0.50~), libqt4-dev (>= "+QT_MIN_VERSION+"), libqtwebkit-dev (>= "+QTWEBKIT_MIN_VERSION+"), libx11-dev, qt4-qmake, python, ant, gwt\n"
    result += "\n"
    for name, item in proj.components.items():
        name = __debian_name(name)
        result += "Package: "+name+"\n"
        if item.isweb:
            result += "Architecture: all\n"
        else:
            result += "Architecture: any\n"
        result += "Depends: ${shlibs:Depends}, ${misc:depends}"
        qt_req = item.requires_qt
        for qt in qt_req:
            pkg = "libqt4-"+qt[2:].lower()+" (>= "+QT_MIN_VERSION+")"
            result += ", "+pkg
        lib_req = item.requires_libs
        for lib in lib_req:
            provider = proj.findLibraryProvider(lib)
            if len(provider)>0:
                result += ", kumir2-"+__debian_name(provider)+" (= "+proj.version
                if len(proj.version_extra)>0:
                    result += "."+proj.version_extra
                result += ")"
        web_req = item.requires_web
        for web in web_req:
            provider = proj.findWebAppProvider(web)
            if len(provider)>0:
                result += ", kumir2-"+__debian_name(provider)+" (= "+proj.version
                if len(proj.version_extra)>0:
                    result += "."+proj.version_extra
                result += ")"
        kumir_req = item.requires_kumir2
        for kum in kumir_req:
            result += ", kumir2-modules-"+__debian_name(kum)+" (= "+proj.version
            if len(proj.version_extra)>0:
                result += "."+proj.version_extra
            result += ")"
        result += "\n"
        result += "Description: "+item.summary["en"]+"\n"
        if item.description.has_key("en"):
            desc_lines = item.description["en"].split("\n")
        else:
            desc_lines = ["No description"]
        for line in desc_lines:
            line = line.strip()
            result += "    "+line+"\n"
        result += "\n" 
    return result

def gen_copyright():
    return """
Format: http://dep.debian.net/deps/dep5
Upstream-Name: kumir2
Source: http://www.niisi.ru/kumir

Files: *
Copyright: 2005-2011 Mikhail Roytberg <mroytberg@lpm.org.ru>
           2005-2011 Denis Khachko <mordol@lpm.org.ru>
           2006-2011 Victor Yacovlev <victor@lpm.org.ru>
           2007-2009 Nikolay Suboch <suboch@inbox.ru>

License: GPL-2+
 This package is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
 .
 On Debian systems, the complete text of the GNU General
 Public License version 2 can be found in "/usr/share/common-licenses/GPL-2".

    """

def gen_rules(proj):
    result = """#!/usr/bin/make -f\n
export DH_VERBOSE=1
export DH_OPTIONS
export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib:/usr/lib/libfakeroot:$(CURDIR)/debian/tmp/usr/lib/kumir2:/usr/lib/kumir2
%:
    dh $@
    
override_dh_auto_configure:
    qmake

override_dh_auto_build:
    make
"""
    for name, item in proj.components.items():
        buildcmds = item.buildcmds
        for cmd in buildcmds:
            result += "    "+cmd+"\n"
    result += """
override_dh_auto_install:
    make install INSTALL_ROOT=$(CURDIR)/debian/tmp/usr
"""
    for name, item in proj.components.items():
        installcmds = item.installcmds
        for cmd in installcmds:
            cmd = cmd.replace("%datadir%", "$(CURDIR)/debian/tmp/usr/share")
            cmd = cmd.replace("%bindir%", "$(CURDIR)/debian/tmp/usr/bin")
            cmd = cmd.replace("%libdir%", "$(CURDIR)/debian/tmp/usr/lib")
            result += "    "+cmd+"\n"
    
    return result.replace("    ", "\t")

def gen_package_install(component):
    result = ""
    for f in component.filesmasks:
        f = f.replace("%datadir%", "usr/share")
        f = f.replace("%bindir%", "usr/bin")
        f = f.replace("%libdir%", "usr/lib")
        result += f+"\n"
    for f in component.plugins:
        result += "usr/lib/kumir2/plugins/lib"+f+".so\n"
        result += "usr/lib/kumir2/plugins/"+f+".pluginspec\n"
    for f in component.libs:
        result += "usr/lib/kumir2/lib"+f+".so*\n"
    return result

def gen_install(proj):
    result = []
    for name, item in proj.components.items():
        if len(name)>0:
            name = "kumir2-"+__debian_name(name)
        else:
            name = "kumir2"
        result += [(name, gen_package_install(item))]
    return result

def gen_dsc(proj):
    result = ""
    result += "Format: 1.0\n"
    result += "Source: kumir2\n"
    result += "Version: "+proj.version
    if len(proj.version_extra)>0:
        result += "."+proj.version_extra
    result += "\n"
    result += "Maintainer: "+PACKAGER+"\n"
    result += "Architecture: any\n"
    result += "Standards-Version: 3.9.1\n"
    result += "Homepage: http://www.niisi.ru/kumir/\n"
    result += "Build-Depends: debhelper (>= 7.0.50~), libqt4-dev (>= "+QT_MIN_VERSION+"), libqtwebkit-dev (>= "+QTWEBKIT_MIN_VERSION+"), libx11-dev, qt4-qmake, python, ant, gwt\n"
    binaries = []
    for name, item in proj.components.items():
        if len(name)>0:
            name = "kumir2-"+__debian_name(name)
        else:
            name = "kumir2"
        binaries += [name]
    result += "Binary: "+string.join(binaries, ", ")+"\n"
    result += "Files:\n"
    result += " 249694dd02525fc8f386b6f9179d6d23 5944892 kumir2-"+proj.version
    if len(proj.version_extra)>0:
        result += "."+proj.version_extra
    result += ".tar.gz\n"
    return result

def print_usage_and_exit(errcode):
    sys.stderr.write("Usage: \n")
    sys.stderr.write("   "+sys.argv[0]+" --packager=\"Your Name <your@e.mail>\" [--buildservice] [--all-in-one]\n")
    sys.exit(errcode)

def __write_file(obs, basename, data):
    if not obs:
        if not os.path.exists("debian"):
            os.mkdir("debian")
        prefix = "debian/"
    else:
        prefix = "debian."
    if obs and basename.endswith(".dsc"):
        prefix = ""
    f = open(prefix+basename, 'w')
    f.write(data)
    f.close()
    if basename=="rules":
        st = os.stat(prefix+basename).st_mode
        st |= stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH
        os.chmod(prefix+basename, st)
    

if __name__ == '__main__':
    obs = False
    all_in_one = False
    for arg in sys.argv:
        if arg.startswith("--help") or arg.startswith("-h"):
            print_usage_and_exit(0)
        elif arg.startswith("--packager="):
            PACKAGER = arg[11:]
        elif arg=="--buildservice":
            obs = True
        elif arg=="--all-in-one":
            all_in_one = True
        
    if PACKAGER is None:
        print_usage_and_exit(1)
    proj_root, scriptname = os.path.split(sys.argv[0])
    proj_root += "/.."
    proj_root = os.path.abspath(proj_root)
    proj = project_spider.scan_project(proj_root)
    if all_in_one:
        proj.mergeIntoAllInOnePackage()
    debian_changelog = gen_changelog(proj)
    debian_copyright = gen_copyright()
    debian_compat = gen_compat()
    debian_control = gen_control(proj)
    debian_rules = gen_rules(proj)
    debian_packages = gen_install(proj)
    debian_dsc = gen_dsc(proj)
    
    __write_file(obs, "changelog", debian_changelog)
    __write_file(obs, "compat", debian_compat)
    __write_file(obs, "control", debian_control)
    __write_file(obs, "rules", debian_rules)
    __write_file(obs, "copyright", debian_copyright)
    if obs:
        __write_file(True, "kumir2.dsc", debian_dsc)
    if not all_in_one:
        for name, data in debian_packages:
            __write_file(obs, name+".install", data)
    else:
        name, data = debian_packages[0]
        __write_file(obs, "install", data)
    
