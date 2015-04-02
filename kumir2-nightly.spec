# norootforbuild

# Required for version information in case of not in GIT repo
# define date 20150401
# define branch master
# define hash 255daa697c2cd10763def72c12141c41b51502b8
# define tag 2.1.0-beta7
# define timestamp 1427898729

# Build kumir2-llvm or not
%define with_llvm 1

# If target system has modern version of Boost,
# use it. Otherwise use bundled files
%define with_bundled_boost 0

# Build kumir2-python (experimental!!!) or not
%define with_python_ide 0


# Note:
# OBS service freezes on LaTeX pre-installation,
# required to build PDF documentation
%define with_pdf_documentation 0

Name:		kumir2-nightly
Summary:	Kumir education system (Nightly *unstable* builds)
License:	GPL2+
BuildRequires:	python
BuildRequires:	cmake >= 2.8
BuildRequires:	gcc-c++ >= 4.6
Group:	Productivity/Scientific/Other
Version:	git%date
Release:	1
BuildRoot:	%{_tmppath}/%{name}-%{version}
Vendor:		NIISI RAS
Packager:	Kumir Team <kumir@lpm.org.ru>
Source:		kumir2-%branch.tar.gz
BuildRequires:	pkgconfig(QtCore) >= 4.7.0
BuildRequires:	pkgconfig(QtGui) >= 4.7.0
BuildRequires:	pkgconfig(QtXml) >= 4.7.0
BuildRequires:	pkgconfig(QtWebKit) >= 4.7.0
BuildRequires:	pkgconfig(QtScript) >= 4.7.0
BuildRequires:	pkgconfig(QtDeclarative) >= 4.7.0
%if %with_bundled_boost
%else
BuildRequires:	boost-devel >= 1.49
%endif
%if %with_python_ide
BuildRequires:	python3-devel >= 3.2
BuildRequires:	pkgconfig(python3)
%endif
%if %with_llvm
BuildRequires:	clang >= 3.2
BuildRequires:	llvm-devel >= 3.2
%endif
%if %with_pdf_documentation
BuildRequires:	texlive
BuildRequires:	texlive-collection-langcyrillic
BuildRequires:	texlive-collection-latex
BuildRequires:	texlive-collection-latexextra
BuildRequires:	texlive-collection-latexrecommended
# fixes fedora ambiguous dependency
BuildRequires:	texlive-simplecv 
%endif
URL:	http://www.niisi.ru/kumir/
%if 0%{?opensuse_bs}
%if 0%{?suse_version}
BuildRequires:  procps
BuildRequires:	-post-build-checks -rpmlint-Factory
%endif
%endif

# meta-package requirements
Requires:       kumir2-desktop-integration = %version
Requires:	kumir2-libs = %version
Requires:	kumir2-bc = %version
Requires:	kumir2-run = %version
Requires:	kumir2-xrun = %version
Requires:	kumir2-ide = %version
Requires:	kumir2-classic = %version
Requires:	kumir2-highgrade = %version
Requires:	kumir2-teacher = %version
%if %with_python_ide
Requires:	kumir2-python = %version
%endif
%if %with_llvm
Requires:	kumir2-llvmc = %version
%endif
%if %with_pdf_documentation
Requires:       kumir2-doc = %version
%endif


%description
Meta-package to full kumir2 installation

%prep
%setup -q -n kumir2-%branch

%build
# debug messages for OBS
%if %{defined fedora}
echo "Build for Fedora"
%endif
%if %{defined suse_version}
echo "Build for openSUSE"
%endif
touch version_info.cmake
echo set\(GIT_TIMESTAMP %timestamp\) > version_info.cmake
echo set\(GIT_BRANCH %branch\) >> version_info.cmake
echo set\(GIT_TAG %tag\) >> version_info.cmake
echo set\(GIT_HASH %hash\) >> version_info.cmake

%if %with_bundled_boost
%else
rm -rf src/3rdparty/boost-*
%endif

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$RPM_BUILD_ROOT/%_prefix ../
make -j 5
cd ..

%if %with_pdf_documentation
mkdir pdf_userdoc
mkdir pdf_userdoc/pdf_a4
mkdir pdf_userdoc/pdf_tablet

export PATH=`pwd`/build/Release/bin:$PATH
export LD_LIBRARY_PATH=`pwd`/build/Release/lib/kumir2:`pwd`/build/Release/lib64/kumir2:$LD_LIBRARY_PATH

cd userdocs
docbook2latex --profile=a4 --configuration=classic --output=kumir2-classic.tex system/index-classic.xml
docbook2latex --profile=a4 --configuration=highgrade --output=kumir2-highgrade.tex system/index-highgrade.xml
docbook2latex --profile=a4 --configuration=ide --output=kumir2-ide.tex system/index-ide.xml
docbook2latex --profile=a4 --configuration=teacher --output=kumir2-teacher.tex system/index-teacher.xml
docbook2latex --profile=a4 --output=language.tex language/index.xml
for file in Actor*.xml
    docbook2latex --profile=a4 $file
done   

for file in *.tex
    pdflatex $file
done    
# Run pdflatex again to include TOC
for file in *.tex
    pdflatex $file
done

mv *.pdf ../pdf_userdoc/pdf_a4/
rm *.tex

docbook2latex --profile=tablet --configuration=classic --output=kumir2-classic.tex system/index-classic.xml
docbook2latex --profile=tablet --configuration=highgrade --output=kumir2-highgrade.tex system/index-highgrade.xml
docbook2latex --profile=tablet --configuration=ide --output=kumir2-ide.tex system/index-ide.xml
docbook2latex --profile=tablet --configuration=teacher --output=kumir2-teacher.tex system/index-teacher.xml
docbook2latex --profile=tablet --output=language.tex language/index.xml
for file in Actor*.xml
    docbook2latex --profile=tablet $file
done    

for file in *.tex
    pdflatex $file
done    
# Run pdflatex again to include TOC
for file in *.tex
    pdflatex $file
done

mv *.pdf ../pdf_userdoc/pdf_tablet/

cd ..
%else
echo "Skip build PDF documentation"
%endif

%install
cd build
make install
cd ..

%if %with_pdf_documentation
mkdir -p $RPM_BUILD_ROOT/%_docdir/%name/pdf_a4
mkdir -p $RPM_BUILD_ROOT/%_docdir/%name/pdf_tablet
cp pdf_userdoc/pdf_a4/*.pdf $RPM_BUILD_ROOT/%_docdir/%name/pdf_a4/
cp pdf_userdoc/pdf_tablet/*.pdf $RPM_BUILD_ROOT/%_docdir/%name/pdf_tablet/
%endif


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%dir %_libdir/kumir2
%dir %_datadir/kumir2

%package libs
Summary:	Shared files used by various Kumir components
%if %{defined fedora}
Requires:	qt4 >= 4.7.0
%endif
%if %{defined suse_version}
Requires:	libqt4 >= 4.7.0
%endif
Requires:	libQtCore.so.4
Requires:	libQtGui.so.4
Requires:	libQtXml.so.4
Requires:	libQtWebKit.so.4
Requires:	libQtDeclarative.so.4
%if %with_python_ide
Requires:	python3 >= 3.2
Requires:	libpython3.so
%endif

%description libs
Shared files used by various Kumir components

%files libs
%defattr(-,root,root)
%dir %_libdir/kumir2
%dir %_libdir/kumir2/plugins
%dir %_datadir/kumir2
%_libdir/kumir2/*.so
%_libdir/kumir2/plugins/libActor*.so
%_libdir/kumir2/plugins/Actor*.pluginspec
%_libdir/kumir2/plugins/libBrowser.so
%_libdir/kumir2/plugins/Browser.pluginspec
%_libdir/kumir2/plugins/libCoreGUI.so
%_libdir/kumir2/plugins/CoreGUI.pluginspec
%_libdir/kumir2/plugins/libCourseManager.so
%_libdir/kumir2/plugins/CourseManager.pluginspec
%_libdir/kumir2/plugins/libEditor.so
%_libdir/kumir2/plugins/Editor.pluginspec
%_libdir/kumir2/plugins/libKumirAnalizer.so
%_libdir/kumir2/plugins/KumirAnalizer.pluginspec
%_libdir/kumir2/plugins/libKumirCompilerTool.so
%_libdir/kumir2/plugins/KumirCompilerTool.pluginspec
%_libdir/kumir2/plugins/libKumirCodeGenerator.so
%_libdir/kumir2/plugins/KumirCodeGenerator.pluginspec
%_libdir/kumir2/plugins/libKumirCodeRun.so
%_libdir/kumir2/plugins/KumirCodeRun.pluginspec
%if %with_python_ide
%_libdir/kumir2/plugins/libPython3Language.so
%_libdir/kumir2/plugins/Python3Language.pluginspec
%endif
%if %with_llvm
%_libdir/kumir2/plugins/libLLVMCodeGenerator.so
%_libdir/kumir2/plugins/LLVMCodeGenerator.pluginspec
%endif
%_datadir/kumir2/actors/*
%_datadir/kumir2/coregui/*
%_datadir/kumir2/coursemanager/*
%_datadir/kumir2/editor/*
%_datadir/kumir2/icons/*
%_datadir/kumir2/kumiranalizer/*
%if %with_python_ide
%_datadir/kumir2/python3language/*
%endif
%if %with_llvm
%_datadir/kumir2/llvmcodegenerator/*
%endif
%_datadir/kumir2/translations/*
%_datadir/kumir2/userdocs/*
%_datadir/kumir2/widgets/*
%_datadir/kumir2/docbookviewer/*

%post libs -p /sbin/ldconfig

%postun libs -p /sbin/ldconfig



%package ide
Summary:	Modern UI for Kumir
Requires:	%name-libs = %version
Provides:       %name-gui = %version
Requires:	/usr/bin/gtk-update-icon-cache

%description ide
Modern UI for Kumir with tabs and session support

%files ide
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%dir %_datadir/icons/hicolor/*/apps
%dir %_datadir/applications
%endif
%_bindir/kumir2-ide
%_datadir/applications/kumir2-professional.desktop
%_datadir/icons/hicolor/*/apps/kumir2.*

%post ide -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
%postun ide -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true


%package classic
Summary:	Classic UI for Kumir
Provides:       %name-gui = %version
Requires:	%name-libs = %version
Requires:	/usr/bin/gtk-update-icon-cache

%description classic
Classic Kumir UI like 1.x

%files classic
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%dir %_datadir/icons/hicolor/*/apps
%dir %_datadir/applications
%endif
%_bindir/kumir2-classic
%_datadir/applications/kumir2-classic.desktop
%_datadir/icons/hicolor/*/apps/kumir2-classic.*

%post classic -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
%postun classic -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true

%package highgrade
Summary:	Classic UI for Kumir + High grade features
Provides:       %name-gui = %version
Requires:	%name-libs = %version
Requires:	/usr/bin/gtk-update-icon-cache

%description highgrade
Classic Kumir UI like 1.x with high grade packages
enabled by default

%files highgrade
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%dir %_datadir/icons/hicolor/*/apps
%dir %_datadir/applications
%endif
%_bindir/kumir2-highgrade
%_datadir/applications/kumir2-highgrade.desktop
%_datadir/icons/hicolor/*/apps/kumir2-highgrade.*

%post highgrade -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
%postun highgrade -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true

%package teacher
Summary:	Teacher mode Kumir UI
Provides:       %name-gui = %version
Requires:	%name-libs = %version
Requires:	/usr/bin/gtk-update-icon-cache

%description teacher
Kumir UI with teacher features

%files teacher
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%dir %_datadir/icons/hicolor/*/apps
%dir %_datadir/applications
%endif
%_bindir/kumir2-teacher
%_datadir/applications/kumir2-teacher.desktop
%_datadir/icons/hicolor/*/apps/kumir2-teacher.*

%post teacher -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
%postun teacher -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true

%package desktop-integration
Summary:        Kumir2 XDG desktop integration
Requires:       %name-gui = %version
Requires:       %name-run = %version
Requires:       /usr/bin/gtk-update-icon-cache
Requires:       /usr/bin/xdg-mime

%description desktop-integration
Enables open Kumir files from file manager

%files desktop-integration
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%dir %_datadir/icons/hicolor/*/mimetypes
%dir %_datadir/applications
%dir %_datadir/mime/packages
%endif
%_bindir/kumir2-open
%_datadir/applications/kumir2-open.desktop
%_datadir/applications/kumir2-run.desktop
%_datadir/icons/hicolor/*/mimetypes/*
%_datadir/mime/packages/kumir2-mimetypes.xml

%post desktop-integration -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
xdg-menu install /usr/share/mime/packages/kumir2-mimetypes.xml || true

%postun desktop-integration -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
xdg-menu uninstall /usr/share/mime/packages/kumir2-mimetypes.xml || true

%if %with_python_ide
%package python
Summary:	Python3 IDE based on Kumir UI
Requires:	%name-libs = %version
Requires:	/usr/bin/gtk-update-icon-cache
Requires:	python3 >= 3.2

%description python
Python3 IDE with editor and debugger

%files python
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%dir %_datadir/icons/hicolor/*/apps
%dir %_datadir/applications
%endif
%_bindir/kumir2-python
%_datadir/applications/kumir2-python.desktop
%_datadir/icons/hicolor/*/apps/kumir2-python.png

%post python -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
%postun python -p /sbin/ldconfig
gtk-update-icon-cache --quiet --force "hicolor" || true
%endif

%package bc
Summary:	Kumir2 language compiler
Requires:	%name-libs = %version
Obsoletes:	kumir2 < 2.0.90

%description bc
Kumir source to bytecode compiler

%files bc
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%endif
%_bindir/kumir2-bc

%package xrun
Summary:	Kumir2 language complete runtime
Requires:	%name-libs = %version

%description xrun
Complete runtime for kumir2 language supports actors,
but requires Qt and Kumir libraries

%files xrun
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%endif
%_bindir/kumir2-xrun

%package run
Summary:	Kumir2 language lightweight runtime
Requires:	libstdc++.so.6
Obsoletes:	kumir2 < 2.0.90

%description run
Lightweight kumir2 language runtime that do not support
actors, but depends only on standard C++ library

%files run
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%endif
%_bindir/kumir2-run

%package extra-pictomir2course
Summary:        Pictomir to Kumir2's Course Manager conversion tool
Requires:       libstdc++.so.6
Requires:       libz.so.1

%description extra-pictomir2course
A tool to convert Pictomir >= 0.15.0 "Games" created by
Pictomir creator into Kumir2 Course Manager XML files

%files extra-pictomir2course
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%endif
%_bindir/pictomir2course

%package extra-courseeditor
Summary:       Course editor tool
%if %{defined fedora}
Requires:       qt4 >= 4.7.0
%endif
%if %{defined suse_version}
Requires:       libqt4 >= 4.7.0
%endif
Requires:       libQtCore.so.4
Requires:       libQtGui.so.4
Requires:       libQtXml.so.4
Requires:       libQtWebKit.so.4

%description extra-courseeditor
A tool for authoring Kumir Course Manager XML files

%files extra-courseeditor
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%endif
%_bindir/kumir2-courseeditor

%if %with_llvm
%package llvmc
Summary:	Kumir2 to native code compiler based on LLVM
Requires:	clang >= 3.2
Requires:	/usr/bin/ld
Requires:	/usr/bin/llc
Requires:	/usr/bin/as

%description llvmc
A tool to create both native (default run) and LLVM-bitcode
executables from Kumir programs

NOTE: current version does not support actors

%files llvmc
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%endif
%_bindir/kumir2-llvmc
%endif

%if %with_pdf_documentation

%package doc
BuildArch:	noarch
Summary:        Russian PDF-documentation for Kumir

%description doc
PDF files for printing using A4 printer and for 9-inch tablets/bookreaders

%files doc
%defattr(-,root,root)
%if %{defined suse_version}
%dir %_bindir
%endif
%dir %_docdir/%name/*
%_docdir/%name/*/*.pdf

# ends "with_pdf_documentation"
%endif

%changelog
* Thu Apr 02 2015 - Kumir Team <kumir@lpm.org.ru>
- Updated to build nightly versions within TeamCity

* Wed Apr 01 2015 - Kumir Team <kumir@lpm.org.ru>
- Updated to actual beta version

* Mon Apr 28 2014 - Kumir Team <kumir@lpm.org.ru>
- Updated to next beta version

* Fri Nov 29 2013 - Kumir Team <kumir@lpm.org.ru>
- Memory leaks fixed
- Desktop integration for Kumir-handled files

* Wed Nov 20 2013 - Kumir Team <kumir@lpm.org.ru>
- Bugfixes planned for Beta-2
- Integrated LLVM branch (Linux version only)

* Thu Sep 26 2013 - Kumir Team <kumir@lpm.org.ru>
- Fixed functionality to testing branch 2.1

* Mon Jul 22 2013 - Kumir Team <kumir@lpm.org.ru>
- Updated to current master version and packages layout

* Tue Oct 4 2011 - Kumir Team <kumir@lpm.org.ru>
- MacOS X crash fix
- Preliminary Pascal language support

* Wed Sep 7 2011 - Kumir Team <kumir@lpm.org.ru>
- CLang backend for native code generation
- Shared memory mechanism for IPC replaced by process streams

* Tue Sep 6 2011 - Kumir Team <kumir@lpm.org.ru>
- Various UI fixes

* Tue Aug 30 2011 - Kumir Team <kumir@lpm.org.ru>
- Implemented help viewer
- Implemented code autocompleter
- Fixed some bugs

* Wed Aug 24 2011 - Kumir Team <kumir@lpm.org.ru>
- Implemented teacher mode
- Implemented variables view while running

* Mon Aug 15 2011 - Kumir Team <kumir@lpm.org.ru>
- Implemented integer/double overflow checking while running
- Added kumir2-run tool

* Thu Aug 11 2011 - Kumir Team <kumir@lpm.org.ru>
- Ready for preliminary testing

* Wed Jul 20 2011 - Kumir Team <kumir@lpm.org.ru>
- Usable functionality

