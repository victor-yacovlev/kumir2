import os
import sys
import json

from . import query_version_info

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
Requires:   /usr/bin/gtk-update-icon-cahce
Requires:   /usr/bin/xdg-mime
Conflicts:  $mainPackageConflictName
$buildRequires

%description
Kumir education system for schools 
$unstableNotice

%prep
%setup -q

%build
mkdir build
pushd build
$cmake -DUSE_QT=5 -DKUMIR2_SDK_CMAKE_DIR=$RPM_BUILD_ROOT/$cmakeFilesLocation -DKUMIR2_SDK_SCRIPTS_DIR=$RPM_BUILD_ROOT/$develScriptsLocation ../
$make
popd

%install
pushd build
$makeInstall
popd

%clean
rm -rf $rpmBuildRoot

%files$mainFilesPrefs
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
$dataDir/icons/hicolor/*/apps/teacher-highgrade.*
$datadir/icons/hicolor/*/mimetypes/*
$datadir/mime/packages/kumir2-mimetypes.xml


%post $name -p /sbin/ldconfig
/bin/touch --no-create $dataDir/icons/hicolor/ &>/dev/null || :
xdg-menu install $dataDir/mime/packages/kumir2-mimetypes.xml || true

%postun $name -p /sbin/ldconfig
if [ $1 -eq 0 ] ; then
  /bin/touch --no-create $dataDir/icons/hicolor/ &>/dev/null
  /usr/bin/gtk-update-icon-cache -f $dataDir/icons/hicolor/ &>/dev/null || :
fi

%posttrans $name
/usr/bin/gtk-update-icon-cache -f $dataDir/icons/hicolor &>/dev/null || :

%package $kumirLibsPackageFullName
$requires

%description $kumirLibsPackageFullName
Common files for Kumir-2 platform

%files $kumirLibsPackageFullName$libsFilesPrefs
$libDir/kumir2/*.so
$libDir/kumir2/plugins/*.so
$dataDir/kumir2/*

%post $kumirLibsPackageFullName -p /sbin/ldconfig
%postun $kumirLibsPackageFullName -p /sbin/ldconfig

%package $kumirDevelPackageFullName
Requires:      $kumirLibsPackageName = %version
$buildRequiresAsRequires

%description $kumirDevelPackageFullName
Development files to build new Actors and plugins for Kumir-2

%files $kumirDevelPackageFullName$develFilesPrefs
$includesDir/kumir2/*
$includesDir/kumir2-libs/*
$cmakeFilesLocation/*
$develScriptsLocation/gen_actor_source.py


%package $kumirToolsPackageFullName
Requires:      $kumirLibsPackageName = %version

%description $kumirToolsPackageFullName
Kumir-2 tools for using in non-GUI mode

%files $kumirToolsPackageFullName
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
    for key, value in substitutions:
        template = template.replace(key, value)
    return template


def read_conventions(dist: str):
    file_name = "rpm-conventions.{}.json".format(dist.lower())
    if not os.path.exists(file_name):
        sys.stderr.write("Warning: No RPM conventions for {}. Using openSUSE\n".format(dist))
        file_name = "rpm-conventions.opensuse.json"
    with open(file_name, "r"):
        return json.read(file_name)


def generate_rpm_spec_contents(dist: str, release: int, packager: str):
    conventions = read_conventions(dist)
    subst = {}
    version_info = query_version_info.get_version_information(os.getcwd())
    if version_info["taggedRelease"]:
        subst["name"] = "kumir2"
        subst["version"] = version_info["version"]
    else:
        subst["name"] = "kumir2-" + conventions["unstablePackageSuffix"]
        subst["version"] = version_info["date"]
    subst["release"] = str(release)
    subst["package"] = packager
    subst["rpmGroup"] = conventions["rpmGroup"]



def main():
    out_file = sys.stdout
    conventions = "openSUSE"
    out_file_name = None
    for arg in sys.argv:
        if arg.startswith("--dist="):
            conventions = arg[7:]
        elif arg.startswith("--out="):
            out_file_name = arg[6:]
    if out_file_name:
        out_file = open(out_file_name, "w")
    data = generate_rpm_spec_contents(conventions)
    out_file.write(data)
    out_file.close()


if __name__ == "__main__":
    main()
