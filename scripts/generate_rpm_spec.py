import functools
import os
import sys
import json

import query_version_info

TEMPLATE = """# norootforbuild
Name:       $name
Summary:    Kumir education system $unstableNotice
License:    GPL2+
Version:    $version
Group:      $rpmGroup
Release:    $release
BuildRoot:	%_tmppath/%name-%version
Vendor:		NIISI RAS
URL:    	http://www.niisi.ru/kumir/
Packager:	$packager
Source:     $source
Requires:   $kumirLibsPackageName = %version
Requires:   /usr/bin/gtk-update-icon-cache
Requires:   /usr/bin/xdg-mime
Conflicts:  $mainPackageConflictName
$buildRequiresList

%description
Kumir education system for schools 
$unstableNotice

%prep
%setup -q -n $archivePrefix

%build
$removeBundledBoostIfNeed
$cmakeCommand \
    -DUSE_QT=5 \
    -DKUMIR2_DISABLED_SUBDIRS="$disabledSubdirs" \
    -DKUMIR2_SDK_CMAKE_DIR=$cmakeFilesLocation \
    -DKUMIR2_SDK_SCRIPTS_DIR=$develScriptsLocation \
    -DPROVIDED_VERSION_INFO:BOOL=ON \
    -DGIT_TAG="$gitTag" \
    -DGIT_BRANCH="$gitBranch" \
    -DGIT_HASH="$gitHash" \
    -DGIT_TIMESTAMP="$gitTimestamp" \
    ../
$makeCommand

%install
$makeInstallCommand


$cleanSection

%files
$mainFilesPrefs
$binDir/kumir2-ide
$binDir/kumir2-classic
$binDir/kumir2-highgrade
$binDir/kumir2-teacher
$binDir/kumir2-open
$binDir/kumir2-courseeditor
$dataDir/applications/kumir2-ide.desktop
$dataDir/icons/hicolor/*/apps/kumir2-ide.*
$dataDir/applications/kumir2-classic.desktop
$dataDir/icons/hicolor/*/apps/kumir2-classic.*
$dataDir/applications/kumir2-highgrade.desktop
$dataDir/icons/hicolor/*/apps/kumir2-highgrade.*
$dataDir/applications/kumir2-teacher.desktop
$dataDir/icons/hicolor/*/apps/kumir2-teacher.*
$dataDir/icons/hicolor/*/mimetypes/*
$dataDir/mime/packages/kumir2-mimetypes.xml


%post
/bin/touch --no-create $dataDir/icons/hicolor/ &>/dev/null || :
/usr/bin/xdg-mime install $dataDir/mime/packages/kumir2-mimetypes.xml || true

%postun
if [ $1 -eq 0 ] ; then
  /bin/touch --no-create $dataDir/icons/hicolor/ &>/dev/null
  /usr/bin/gtk-update-icon-cache -f $dataDir/icons/hicolor/ &>/dev/null || :
fi

%posttrans
/usr/bin/gtk-update-icon-cache -f $dataDir/icons/hicolor &>/dev/null || :

%package $kumirLibsPackageFullName
Summary:        Common files for all Kumir-2 components
Conflicts:      $libsPackageConflictName
$requiresList

%description $kumirLibsPackageFullName
Common files for Kumir-2 platform
$unstableNotice

%files $kumirLibsPackageFullName
$libsFilesPrefs
$libDir/kumir2/*.so
$libDir/kumir2/plugins/*.so
$dataDir/kumir2/*

%post $kumirLibsPackageFullName -p /sbin/ldconfig
%postun $kumirLibsPackageFullName -p /sbin/ldconfig

%package $kumirDevelPackageFullName
Summary:        Kumir-2 development files
Requires:       $kumirLibsPackageName = %version
Conflicts:      $develPackageConflictName
$buildRequiresAsRequiresList

%description $kumirDevelPackageFullName
Development files to build new Actors and plugins for Kumir-2
$unstableNotice

%files $kumirDevelPackageFullName
$develFilesPrefs
$includesDir/kumir2/*
$includesDir/kumir2-libs/*
$cmakeFilesLocation/*
$develScriptsLocation/gen_actor_source.py


%package $kumirToolsPackageFullName
Summary:        Kumir-2 non-GUI tools
Requires:       $kumirLibsPackageName = %version
Conflicts:      $toolsPackageConflictName

%description $kumirToolsPackageFullName
Kumir-2 tools for using in non-GUI mode
$unstableNotice

%files $kumirToolsPackageFullName
$toolsFilesPrefs
$binDir/kumir2-run
$binDir/kumir2-xrun
$binDir/kumir2-bc
$binDir/docbook2latex
$binDir/pictomir2course

%changelog
* Fri Apr 21 2017 - Kumir Team <kumir@lpm.org.ru>
- TODO: generate changelog from git log

"""


def render(template: str, substitutions: dict):
    keys = substitutions.keys()
    sorted_keys = sorted(keys, key=len, reverse=True)
    for key in sorted_keys:
        value = substitutions[key]
        template = template.replace("$"+key, value)
    return template


def read_conventions(dist: str):
    file_name = "rpm-conventions.{}.json".format(dist.lower())
    if not os.path.exists(file_name):
        sys.stderr.write("Warning: No RPM conventions for {}. Using openSUSE\n".format(dist))
        file_name = "rpm-conventions.opensuse.json"
    with open(file_name, "r") as f:
        return json.load(f)


def generate_rpm_spec_contents(dist: str, release: int, packager: str):
    conventions = read_conventions(dist)

    lib_prefix = conventions["commonUsedLibsPackage"]["name"]["prefix"]
    lib_suffix = conventions["commonUsedLibsPackage"]["name"]["suffix"]
    dev_prefix = conventions["develPackage"]["name"]["prefix"]
    dev_suffix = conventions["develPackage"]["name"]["suffix"]
    tools_prefix = conventions["toolsPackage"]["name"]["prefix"]
    tools_suffix = conventions["toolsPackage"]["name"]["suffix"]

    unstable_suffix = conventions["unstablePackageSuffix"]

    subst = {}
    version_info = query_version_info.get_version_information(os.getcwd())
    timestamp = query_version_info.get_timestamp(os.getcwd())
    subst["gitTimestamp"] = str(timestamp)
    disabled_subdirs = query_version_info.disabled_modules()
    subst["disabledSubdirs"] = ";".join(disabled_subdirs)
    if version_info["taggedRelease"]:
        subst["name"] = "kumir2"
        subst["version"] = version_info["version"].replace("-","~")
        subst["unstableNotice"] = ""
        subst["mainPackageConflictName"] = "kumir2" + unstable_suffix
        subst["libsPackageConflictName"] = lib_prefix + "kumir2" + unstable_suffix + lib_suffix
        subst["develPackageConflictName"] = dev_prefix + "kumir2" + unstable_suffix + dev_suffix
        subst["toolsPackageConflictName"] = tools_prefix + "kumir2" + unstable_suffix + tools_suffix
        subst["archivePrefix"] = "kumir2-" + version_info["version"]
        subst["gitTag"] = version_info["version"]
        subst["gitHash"] = "unknown"
        subst["gitBranch"] = "unknown"
    else:
        subst["name"] = "kumir2" + unstable_suffix
        subst["version"] = version_info["date"]
        subst["unstableNotice"] = "(Nightly *unstable* builds)"
        subst["mainPackageConflictName"] = "kumir2"
        subst["libsPackageConflictName"] = lib_prefix + "kumir2" + lib_suffix
        subst["develPackageConflictName"] = dev_prefix + "kumir2" + dev_suffix
        subst["toolsPackageConflictName"] = tools_prefix + "kumir2" + tools_suffix
        subst["archivePrefix"] = "kumir2-" + version_info["branch"] + "-"+ version_info["hash"] + "-" + version_info["date"]
        subst["gitTag"] = "unknown"
        subst["gitHash"] = version_info["hash"]
        subst["gitBranch"] = version_info["branch"]
    subst["kumirLibsPackageName"] = lib_prefix + subst["name"] + lib_suffix
    if lib_prefix:
        subst["kumirLibsPackageFullName"] = "-n " + subst["kumirLibsPackageName"]
    else:
        subst["kumirLibsPackageFullName"] = lib_suffix[1:]
    if dev_prefix:
        subst["kumirDevelPackageFullName"] = "-n " + dev_prefix + subst["name"] + dev_suffix
    else:
        subst["kumirDevelPackageFullName"] = dev_suffix[1:]
    if tools_prefix:
        subst["kumirToolsPackageFullName"] = "-n " + tools_prefix + subst["name"] + tools_suffix
    else:
        subst["kumirToolsPackageFullName"] = tools_suffix[1:]

    subst["release"] = str(release)
    subst["packager"] = packager
    subst["rpmGroup"] = conventions["rpmGroup"]
    subst["source"] = query_version_info.source_file_name("kumir2-", ".tar.gz")
    subst["cmakeCommand"] = conventions["cmakeCommand"]
    subst["makeCommand"] = conventions["makeCommand"]
    subst["makeInstallCommand"] = conventions["makeInstallCommand"]
    subst["rpmBuildRoot"] = conventions["rpmBuildRoot"]
    subst["installPrefix"] = conventions["installPrefix"]
    subst["binDir"] = conventions["binDir"]
    subst["libDir"] = conventions["libDir"]
    subst["dataDir"] = conventions["dataDir"]
    subst["includesDir"] = conventions["includesDir"]
    subst["rpmBuildRoot"] = conventions["rpmBuildRoot"]

    subst["mainFilesPrefs"] = conventions["defAttr"]
    subst["libsFilesPrefs"] = conventions["defAttr"]
    subst["develFilesPrefs"] = conventions["defAttr"]
    subst["toolsFilesPrefs"] = conventions["defAttr"]

    if not conventions["useBundledBoost"]:
        subst["removeBundledBoostIfNeed"] = "rm -rf src/3rdparty/boost*"
    else:
        subst["removeBundledBoostIfNeed"] = ""

    if not conventions["skipCleanSection"]:
        subst["cleanSection"] = "%clean\nrm -rf $rpmBuildRoot\n"
    else:
        subst["cleanSection"] = ""

    if conventions["dirsOwnership"]:
        subst["mainFilesPrefs"] += "\n%dir $binDir"
        subst["mainFilesPrefs"] += "\n%dir $dataDir/applications"
        subst["mainFilesPrefs"] += "\n%dir $dataDir/icons/hicolor/*/apps"
        subst["mainFilesPrefs"] += "\n%dir $dataDir/mime/packages"
        subst["libsFilesPrefs"] += "\n%dir $libDir/kumir2"
        subst["libsFilesPrefs"] += "\n%dir $libDir/kumir2/plugins"
        subst["libsFilesPrefs"] += "\n%dir $dataDir/kumir2"
        subst["develFilesPrefs"] += "\n%dir $includesDir/kumir2"
        subst["develFilesPrefs"] += "\n%dir $includesDir/kumir2-libs"
        subst["develFilesPrefs"] += "\n%dir $cmakeFilesLocation"
        subst["develFilesPrefs"] += "\n%dir $develScriptsLocation"
        subst["toolsFilesPrefs"] += "\n%dir $binDir"

    subst["requiresList"] = "\n".join(["Requires:\t"+entry for entry in conventions["requires"]])
    subst["buildRequiresList"] = "\n".join(["BuildRequires:\t" + entry for entry in conventions["buildRequires"]])
    subst["buildRequiresAsRequiresList"] = "\n".join(["Requires:\t" + entry for entry in conventions["buildRequires"]])
    subst["cmakeFilesLocation"] = conventions["develPackage"]["cmakeFilesLocation"]
    subst["develScriptsLocation"] = conventions["develPackage"]["develScriptsLocation"]

    data = TEMPLATE
    for i in range(0, 3):
        data = render(data, subst)
    return data


def main():
    out_file = sys.stdout
    conventions = "openSUSE"
    release = 1
    out_file_name = None
    packager = "Anonymous <anon@example.com>"
    for arg in sys.argv:
        if arg.startswith("--dist="):
            conventions = arg[7:]
        elif arg.startswith("--out="):
            out_file_name = arg[6:]
        elif arg.startswith("--release="):
            release = int(arg[10:])
        elif arg.startswith("--packager="):
            packager = arg[11:]
            if packager.startswith('"') and packager.endswith('"'):
                packager = packager[1:-1]
            if packager.startswith("'") and packager.endswith("'"):
                packager = packager[1:-1]
    if out_file_name:
        out_file = open(out_file_name, "w")
    data = generate_rpm_spec_contents(conventions, release, packager)
    out_file.write(data)
    out_file.close()


if __name__ == "__main__":
    main()
