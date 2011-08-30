# norootforbuild


%define relno 0
%define packager Victor Yacovlev <victor@lpm.org.ru>
%define is_buildservice 1
%define alphaversion 9

# -----------------------------------------------------------------------------


%define release unknown

%if %{is_buildservice}
    %define is_suse %{defined suse_version}
    %define is_mandriva %{defined mdkversion}
    %define is_fedora %{defined fedora}
%else
    %define is_suse %(if [ -f /etc/SuSE-release ] ; then echo -n 1; else echo -n 0; fi)
    %define is_mandriva %(if [ -f /etc/mandriva-release ] ; then echo -n 1; else echo -n 0; fi)
    %define is_fedora %(if [ -f /etc/fedora-release ] ; then echo -n 1; else echo -n 0; fi)
%endif

%if %{is_suse}
    %define release %{relno}.suse%{suse_version}
%endif

%if %{is_mandriva}
    %define release %mkrel %{relno}
%endif

%if %{is_fedora}
    %define release %{relno}.fc%{fedora}
%endif

Name:		kumir2
Summary:	Kumir education system
License:	GPL2
Group:		Education
Version:	1.99.0+alpha%{alphaversion}
Release:	%{release}
BuildRoot:	%{_tmppath}/%{name}-%{version}
BuildRequires:	python
BuildRequires:	ant
BuildRequires:	gwt >= 2.3.0
%if %{is_suse}
BuildRequires:	libqt4-devel >= 4.6.0 libQtWebKit-devel >= 4.6.0
%if %{is_buildservice}
BuildRequires:	-post-build-checks -rpmlint-Factory
%endif
Requires:	libqt4 >= 4.6.0
Requires:	libqt4-x11 >= 4.6.0
Requires:	libQtWebKit4 >= 4.6.0
%endif
%if %{is_mandriva}
BuildRequires:	libqt4-devel >= 4.6.0
Requires:	libqtcore4 >= 4.6.0
Requires:	libqtgui4 >= 4.6.0
Requires:	libqtsvg4 >= 4.6.0
Requires:	libqtxml4 >= 4.6.0
Requires:	libqtwebkit4 >= 4.6.0
%endif
%if %{is_fedora}
BuildRequires:	gcc-c++
BuildRequires:	qt-devel >= 4.6.0
Requires:	qt >= 4.6.0
Requires:	qt-x11 >= 4.6.0
Requires:	qt-webkit >= 4.6.0
%endif
%if 0%{?fedora} >= 14
BuildRequires:	qt-webkit-devel >= 4.6.0
%endif
PreReq:		shared-mime-info
Vendor:		NIISI RAS
Source:		%{name}-%{version}.tar.gz
Packager:	%{packager}
URL:		http://www.niisi.ru/kumir/

%description
Second generation of well-known Kumir system

%description -l ru_RU.UTF-8
Вторая версия известной системы КуМир

%prep
%setup -q -n %{name}-%{version}


%build
# Run qmake to generate makefiles
%if %{is_suse}
qmake
%endif
%if %{is_fedora}
qmake-qt4
# Fedora has lrelease-qt4 instead of lrelease, so it can't be called from script
lrelease-qt4 share/kumir2/translations/*.ts
%endif
%if %{is_mandriva}
qmake
%endif
# Build binary part
make
# Build GWT-applications used by Kumir
pushd src-www/helpviewer
ant
popd

%install
make INSTALL_ROOT=$RPM_BUILD_ROOT/%{_prefix} install
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/share/kumir2/translations
cp share/kumir2/translations/*.qm $RPM_BUILD_ROOT/%{_prefix}/share/kumir2/translations/
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/icons/
cp -R app_icons/linux/* $RPM_BUILD_ROOT/%{_datadir}/icons/
mkdir -p $RPM_BUILD_ROOT/%{_datadir}/applications/
cp *.desktop $RPM_BUILD_ROOT/%{_datadir}/applications/
# Copy Help Viewer web-application
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/share/kumir2/webapps/helpviewer/
cp -R src-www/helpviewer/war/* $RPM_BUILD_ROOT/%{_prefix}/share/kumir2/webapps/helpviewer/
# Remove unneccesary WEB-INF (we use only client-side part of web-application)
rm -rf $RPM_BUILD_ROOT/%{_prefix}/share/kumir2/webapps/helpviewer/WEB-INF 
# Copy DOCBOOK-data for help viewer
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/share/kumir2/webapps/helpviewer/data/russian
cp userdocs/*.xml $RPM_BUILD_ROOT/%{_prefix}/share/kumir2/webapps/helpviewer/data/russian/

%clean
rm -rf $RPM_BUILD_ROOT

%package common
Summary:	Shared libraries for various Kumir components
Requires:	kumir2-libs

%description common
Contains kumir2 extension system skeleton and other libraries


%files common
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%{_libdir}/kumir2/libErrorMessages.so*
%{_libdir}/kumir2/libExtensionSystem.so*
%{_libdir}/kumir2/libDataFormats.so*
%dir %{_datadir}/kumir2/icons
%{_datadir}/kumir2/icons/*
%dir %{_datadir}/kumir2/translations
#%{_datadir}/kumir2/translations/DataFormats*.qm
%{_datadir}/kumir2/translations/ErrorMessages*.qm
%{_datadir}/kumir2/translations/ExtensionSystem*.qm


%post common -p /sbin/ldconfig
%postun common -p /sbin/ldconfig

%package module-KumirAnalizer
Summary:	Kumir language analizer
Provides:	kumir2-module-Analizer
Requires:	kumir2-common
Requires:	kumir2-module-st_funct

%description module-KumirAnalizer
Kumir language analizer

%files module-KumirAnalizer
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirAnalizer.so
%{_libdir}/kumir2/plugins/KumirAnalizer.pluginspec
%dir %{_datadir}/kumir2/kumiranalizer
%{_datadir}/kumir2/kumiranalizer/*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirAnalizer*.qm

%package module-KumirNativeGenerator
Summary:	Kumir AST to native execuable translator
Provides:	kumir2-module-Generator
Requires:	kumir2-common
Requires:	gcc >= 4

%description module-KumirNativeGenerator
Generator of execuables by Kumir language AST (via gcc toolchain)

%files module-KumirNativeGenerator
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirNativeGenerator.so
%{_libdir}/kumir2/plugins/KumirNativeGenerator.pluginspec
%dir %{_datadir}/kumir2/kumirnativegenerator/
%{_datadir}/kumir2/kumirnativegenerator/*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirNativeGenerator*.qm


%package module-KumirCodeGenerator
Summary:	Kumir AST to execuable bytecode translator
Provides:	kumir2-module-Generator
Requires:	kumir2-common

%description module-KumirCodeGenerator
Generator of bytecode to be evaluated by Kumir VM

%files module-KumirCodeGenerator
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirCodeGenerator.so
%{_libdir}/kumir2/plugins/KumirCodeGenerator.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirCodeGenerator*.qm


%package module-KumirCompiler
Summary:	Kumir compiler toolchain module
Requires:	kumir2-common
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-KumirNativeGenerator

%description module-KumirCompiler
Provides ability to compile Kumir programs

%files module-KumirCompiler
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirCompiler.so
%{_libdir}/kumir2/plugins/KumirCompiler.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirCompiler*.qm

%package module-KumirBCompiler
Summary:	Kumir bytecode compiler toolchain module
Requires:	kumir2-common
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-KumirCodeGenerator

%description module-KumirBCompiler
Provides ability to bytecode-compile Kumir programs

%files module-KumirBCompiler
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirBCompiler.so
%{_libdir}/kumir2/plugins/KumirBCompiler.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirBCompiler*.qm

%package module-KumirCodeRun
Summary:	Kumir bytecode interpreter module
Requires:	kumir2-common
Requires:	kumir2-module-st_funct

%description module-KumirCodeRun
Intepreter of Kumir-2 bytecode

%files module-KumirCodeRun
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirCodeRun.so
%{_libdir}/kumir2/plugins/KumirCodeRun.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirCodeRun*.qm

%package libs
Summary:	Libraries required to run Kumir-compiled programs

%description libs
This package is required in order to distribute Kumir-compiled
programs without Kumir system itself.

%files libs
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%{_libdir}/kumir2/libKumirStdLib.so*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirStdLib*.qm

%post libs -p /sbin/ldconfig
%postun libs -p /sbin/ldconfig

%package module-st_funct
Summary:	Kumir Standard Library
Requires:	kumir2-common
Requires:	kumir2-libs

%description module-st_funct
Standart library module

%files module-st_funct
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libst_funct.so
%{_libdir}/kumir2/plugins/st_funct.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/st_funct*.qm

%package module-Editor
Summary:	Kumir program editor
Requires:	kumir2-common
Requires:	kumir2-module-Analizer

%description module-Editor
Kumir program editor module

%files module-Editor
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libEditor.so
%{_libdir}/kumir2/plugins/Editor.pluginspec
%dir %{_datadir}/kumir2/editor
%{_datadir}/kumir2/editor/*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/Editor*.qm


%package module-Browser
Summary:	Kumir Webkit browser
Requires:	kumir2-common

%description module-Browser
WebKit browser

%files module-Browser
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libBrowser.so
%{_libdir}/kumir2/plugins/Browser.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/Browser*.qm


%package module-CoreGUI
Summary:	GUI for Kumir
Requires:	kumir2-module-Analizer
Requires:	kumir2-module-Generator
Requires:	kumir2-module-Editor
Requires:	kumir2-module-Browser

%description module-CoreGUI
GUI for Kumir

%files module-CoreGUI
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libCoreGUI.so
%{_libdir}/kumir2/plugins/CoreGUI.pluginspec
%dir %{_datadir}/kumir2/coregui
%{_datadir}/kumir2/coregui/*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/CoreGUI*.qm
%dir %{_datadir}/kumir2/webapps/helpviewer
%{_datadir}/kumir2/webapps/helpviewer/*


%package utils-cc
Summary:	Kumir to native execuable compiler tool
Requires:	kumir2-common
Requires:	kumir2-module-KumirCompiler

%description utils-cc
A tool to compile kumir-programs into native execuables via gcc.
Run "kumir2-cc --help" for more information

%files utils-cc
%defattr(-,root,root)
%{_bindir}/kumir2-cc
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirCompiler*.qm


%package utils-bc
Summary:	Kumir to portable bytecode execuable compiler tool
Requires:	kumir2-common
Requires:	kumir2-module-KumirBCompiler

%description utils-bc
A tool to compile kumir-programs into portable execuable bytecode
Run "kumir2-bc --help" for more information

%files utils-bc
%defattr(-,root,root)
%{_bindir}/kumir2-bc
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirBCompiler*.qm

%package utils-as
Summary:	Kumir bytecode (dis)assembler tool
Requires:	kumir2-common

%description utils-as
A tool to assemble/disassemble Kumir portable execuable bytecode
Run "kumir2-as --help" for more information

%files utils-as
%defattr(-,root,root)
%{_bindir}/kumir2-as

%package utils-run
Summary:	Kumir portable bytecode interpreter
Requires:	kumir2-common
Requires:	kumir2-module-KumirCodeRun
Requires:	kumir2-module-st_funct

%description utils-run
Standalone kumir programs launcher

%files utils-run
%defattr(-,root,root)
%{_bindir}/kumir2-run

%package professional
Summary:	Kumir Professional Edition
Requires:	kumir2-libs
Requires:	kumir2-module-CoreGUI
Requires:	kumir2-module-Editor
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-KumirCodeGenerator
Requires:	kumir2-module-KumirCodeRun
Requires:	kumir2-module-KumirNativeGenerator
Requires:	kumir2-module-st_funct

%description professional
Kumir IDE for high school applications

%files professional
%defattr(-,root,root)
%{_bindir}/kumir2-ide
%{_datadir}/applications/kumir2-professional.desktop
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/scalable
%dir %{_datadir}/icons/hicolor/scalable/apps
%dir %{_datadir}/icons/hicolor/128x128
%dir %{_datadir}/icons/hicolor/128x128/apps
%dir %{_datadir}/icons/hicolor/64x64
%dir %{_datadir}/icons/hicolor/64x64/apps
%dir %{_datadir}/icons/hicolor/48x48
%dir %{_datadir}/icons/hicolor/48x48/apps
%{_datadir}/icons/hicolor/*/apps/kumir2.*

%post professional
%update_icon_cache hicolor
%icon_theme_cache_post hicolor

%postun professional
%update_icon_cache hicolor
%icon_theme_cache_post hicolor

%package teacher
Summary:	Kumir Teacher Edition
Requires:	kumir2-libs
Requires:	kumir2-module-CoreGUI
Requires:	kumir2-module-Editor
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-KumirCodeGenerator
Requires:	kumir2-module-KumirCodeRun
Requires:	kumir2-module-KumirNativeGenerator
Requires:	kumir2-module-st_funct

%description teacher
Kumir IDE for tutors

%files teacher
%defattr(-,root,root)
%{_bindir}/kumir2-teacher
%{_datadir}/applications/kumir2-teacher.desktop
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/scalable
%dir %{_datadir}/icons/hicolor/scalable/apps
%dir %{_datadir}/icons/hicolor/128x128
%dir %{_datadir}/icons/hicolor/128x128/apps
%dir %{_datadir}/icons/hicolor/64x64
%dir %{_datadir}/icons/hicolor/64x64/apps
%dir %{_datadir}/icons/hicolor/48x48
%dir %{_datadir}/icons/hicolor/48x48/apps
%{_datadir}/icons/hicolor/*/apps/kumir2-teacher.*

%post teacher
%update_icon_cache hicolor
%icon_theme_cache_post hicolor

%postun teacher
%update_icon_cache hicolor
%icon_theme_cache_post hicolor

%package standard
Summary:	Kumir Standard Edition
Requires:	kumir2-libs
Requires:	kumir2-module-CoreGUI
Requires:	kumir2-module-Editor
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-KumirCodeGenerator
Requires:	kumir2-module-KumirCodeRun
Requires:	kumir2-module-st_funct

%description standard
Kumir IDE for school applications

%files standard
%defattr(-,root,root)
%{_bindir}/kumir2-classic
%{_datadir}/applications/kumir2-standard.desktop
%dir %{_datadir}/icons/hicolor
%dir %{_datadir}/icons/hicolor/scalable
%dir %{_datadir}/icons/hicolor/scalable/apps
%dir %{_datadir}/icons/hicolor/128x128
%dir %{_datadir}/icons/hicolor/128x128/apps
%dir %{_datadir}/icons/hicolor/64x64
%dir %{_datadir}/icons/hicolor/64x64/apps
%dir %{_datadir}/icons/hicolor/48x48
%dir %{_datadir}/icons/hicolor/48x48/apps
%{_datadir}/icons/hicolor/*/apps/kumir2-classic.*

%post standard
%update_icon_cache hicolor
%icon_theme_cache_post hicolor

%postun standard
%update_icon_cache hicolor
%icon_theme_cache_post hicolor

%package libs-painter
Summary:	Actor Painter runtime libraries
Requires:	kumir2-libs

%description libs-painter
Libraries required to distribute Kumir-compiled programs
using actor Painter

%files libs-painter
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%{_libdir}/kumir2/libActorPainterC.so*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/ActorPainterC_*.qm

%post libs-painter -p /sbin/ldconfig
%postun libs-painter -p /sbin/ldconfig

%package actor-painter
Summary:	Actor Painter for Kumir
Requires:	kumir2-common
Requires:	kumir2-libs-painter

%description actor-painter
Actor Painter adds raster-painting feauteres for Kumir

%files actor-painter
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libActorPainter.so
%{_libdir}/kumir2/plugins/ActorPainter.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/ActorPainter_*.qm

%changelog
* Tue Aug 30 2011 - Victor Yacovlev <victor@lpm.org.ru>
- Implemented help viewer
- Implemented code autocompleter
- Fixed some bugs

* Wed Aug 24 2011 - Victor Yacovlev <victor@lpm.org.ru>
- Implemented teacher mode
- Implemented variables view while running

* Mon Aug 15 2011 - Victor Yacovlev <victor@lpm.org.ru>
- Implemented integer/double overflow checking while running
- Added kumir2-run tool

* Thu Aug 11 2011 - Victor Yacovlev <victor@lpm.org.ru>
- Ready for preliminary testing

* Wed Jul 20 2011 - Victor Yacovlev <victor@lpm.org.ru>
- Usable functionality

* Tue May 31 2011 - Victor Yacovlev <victor@lpm.org.ru>
- Initial build for testing
- Almoust ready C-generator and analizer
- Editor is launchable
