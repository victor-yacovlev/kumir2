# norootforbuild


%define relno 0
%define packager Victor Yacovlev <victor@lpm.org.ru>
%define is_buildservice 0
%define alphaversion 3

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
%if %{is_suse}
qmake
%endif
%if %{is_fedora}
qmake-qt4
%endif
%if %{is_mandriva}
qmake
%endif
make

%install
make INSTALL_ROOT=$RPM_BUILD_ROOT/%{_prefix} install

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
%{_libdir}/kumir2/libAbstractSyntaxTree.so*
%{_libdir}/kumir2/libErrorMessages.so*
%{_libdir}/kumir2/libExtensionSystem.so*
%{_libdir}/kumir2/libBytecode.so*
%dir %{_datadir}/kumir2/icons
%{_datadir}/kumir2/icons/*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/AbstractSyntaxTree*.qm
%{_datadir}/kumir2/translations/Bytecode*.qm
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
Requires:	kumir2-module-NativeGenerator

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
Requires:	kumir2-module-CodeGenerator

%description module-KumirBCompiler
Provides ability to bytecode-compile Kumir programs

%files module-KumirBCompiler
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirBCompiler.so
%{_libdir}/kumir2/plugins/KumirBCompiler.pluginspec
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirBCompiler*.qm

%package libs
Summary:	Libraries required to run Kumir-compiled programs

%description
This package is required in order to distribute Kumir-compiled
programs without Kumir system itself.

%files libs
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%{_libdir}/kumir2/libKumirStdLib.so*
%{_libdir}/kumir2/libKumirGuiRunner.so*
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/KumirStdLib*.qm
%{_datadir}/kumir2/translations/KumirGuiRunner*.qm

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

%files module-Editor
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libEditor.so
%{_libdir}/kumir2/plugins/Editor.pluginspec
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


%package module-CoreGui
Summary:	GUI for Kumir
Requires:	kumir2-module-Analizer
Requires:	kumir2-module-Generator
Requires:	kumir2-module-Editor
Requires:	kumir2-module-Browser

%description module-CoreGui
GUI for Kumir

%files module-CoreGui
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libCoreGui.so
%{_libdir}/kumir2/plugins/CoreGui.pluginspec
%dir %{_datadir}/kumir2/coregui
%{_datadir}/kumir2/coregui
%dir %{_datadir}/kumir2/translations
%{_datadir}/kumir2/translations/CoreGui*.qm


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
Summary:	Kumir to bytecode execuable compiler tool
Requires:	kumir2-common
Requires:	kumir2-module-KumirBCompiler

%description utils-bc
A tool to compile kumir-programs into portable execuable bytecode
Run "kumir2-bc --help" for more information

%files utils-as
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

%package professional
Summary:	Kumir Professional Edition
Requires:	kumir2-libs
Requires:	kumir2-module-CoreGui
Requires:	kumir2-module-Editor
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-KumirCodeGenerator
Requires:	kumir2-module-KumirNativeGenerator
Requires:	kumir2-module-st_funct

%description professional
Kumir IDE for high school applications

%files professional
%defattr(-,root,root)
%{_bindir}/kumir2-ide

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

%post libs-painter -p /sbin/ldconfig
%postun libs-painter -p /sbin/ldconfig

%package actor-painter
Summary:	Actor Painter for Kumir
Requires:	kumir2-common
Requires:	libs-painter

%description actor-painter
Actor Painter adds raster-painting feauteres for Kumir

%files actor-painter
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libActorPainter.so
%{_libdir}/kumir2/plugins/ActorPainter.pluginspec
