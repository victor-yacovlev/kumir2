# norootforbuild


%define relno 0
%define packager Victor Yacovlev <victor@lpm.org.ru>
%define is_buildservice 0
%define alphaversion 1

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

%package shared-libs
Summary:	Shared libraries for various Kumir components

%description shared-libs
Contains kumir2 extension system skeleton and other libraries


%files shared-libs
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%{_libdir}/kumir2/libAbstractSyntaxTree.so*
%{_libdir}/kumir2/libErrorMessages.so*
%{_libdir}/kumir2/libExtensionSystem.so*


%post shared-libs -p /sbin/ldconfig
%postun shared-libs -p /sbin/ldconfig

%package module-KumirAnalizer
Summary:	Kumir language analizer
Provides:	kumir2-module-Analizer
Requires:	kumir2-libs
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

%package module-KumirCppGenerator
Summary:	Kumir AST to native execuable translator
Provides:	kumir2-module-Generator
Requires:	kumir2-libs
Requires:	gcc >= 4

%description module-KumirCppGenerator
Generator of execuables by Kumir language AST (via gcc toolchain)

%files module-KumirCppGenerator
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirCppGenerator.so
%{_libdir}/kumir2/plugins/KumirCppGenerator.pluginspec
%dir %{_datadir}/kumir2/kumircppgenerator/
%{_datadir}/kumir2/kumircppgenerator/*


%package module-KumirCodeGenerator
Summary:	Kumir AST to execuable bytecode translator
Provides:	kumir2-module-Generator
Requires:	kumir2-libs

%description module-KumirCodeGenerator
Generator of bytecode to be evaluated by Kumir VM

%files module-KumirCodeGenerator
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirCodeGenerator.so
%{_libdir}/kumir2/plugins/KumirCodeGenerator.pluginspec


%package module-KumirCompiler
Summary:	Kumir compiler toolchain module
Requires:	kumir2-libs
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-Generator

%description module-KumirCompiler
Provides ability to compile Kumir programs

%files module-KumirCompiler
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libKumirCompiler.so
%{_libdir}/kumir2/plugins/KumirCompiler.pluginspec

%package runtime
Summary:	Libraries required to run Kumir-compiled programs

%description
This package is required in order to distribute Kumir-compiled
programs without Kumir system itself.

%files runtime
%defattr(-,root,root)
%dir %{_libdir}/kumir2
%{_libdir}/kumir2/libKumirStdLib.so*
%{_libdir}/kumir2/libKumirGuiRunner.so*

%post runtime -p /sbin/ldconfig
%postun runtime -p /sbin/ldconfig

%package module-st_funct
Summary:	Kumir Standart Library
Requires:	kumir2-libs

%description module-st_funct
Standart library module

%files module-st_funct
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libst_funct.so
%{_libdir}/kumir2/plugins/st_funct.pluginspec

%package module-Editor
Summary:	Kumir program editor
Requires:	kumir2-libs
Requires:	kumir2-module-Analizer

%description module-Editor
Kumir program editor module

%files module-Editor
%defattr(-,root,root)
%dir %{_libdir}/kumir2/plugins
%{_libdir}/kumir2/plugins/libEditor.so
%{_libdir}/kumir2/plugins/Editor.pluginspec

%package launcher-cc
Summary:	Kumir to native execuable compiler tool
Requires:	kumir2-libs
Requires:	kumir2-module-KumirAnalizer
Requires:	kumir2-module-KumirCppGenerator

%description launcher-cc
A tool to compile kumir-programs into native execuables via gcc.
Run "kumir2-cc --help" for more information

%files launcher-cc
%defattr(-,root,root)
%{_bindir}/kumir2-cc


%package launcher-ide
Summary:	Kumir IDE
Requires:	kumir2-libs
Requires:	kumir2-module-Editor

%description launcher-ide
Kumir IDE

%files launcher-ide
%defattr(-,root,root)
%{_bindir}/kumir2-ide
