# norootforbuild

%if 0%{?opensuse_bs}
%define is_fedora %{defined fedora}
%else
%define is_fedora %(if [ -f /etc/redhat-release ] ; then echo -n 1 ; else echo -n 0 ; fi)
%endif
Name:		kumir2
Summary:	Kumir education system
License:	GPL2+
Group:		Productivity/Scientific/Other
Version:	2.0.0
Release:	0
BuildRoot:	%_tmppath/%name-%version
%if %{is_fedora}
BuildRequires:	qt-devel >= 4.7.0
%else
BuildRequires:	libqt4-devel >= 4.7.0
%endif
BuildRequires:	python
BuildRequires:	gcc-c++ >= 4.5
BuildRequires:	pkgconfig(QtCore)
BuildRequires:	pkgconfig(QtGui)
BuildRequires:	cmake >= 2.8
Vendor:		NIISI RAS
Packager:	Victor Yacovlev <victor@lpm.org.ru>
Source:		%name-%version.tar.gz
URL:		http://www.niisi.ru/kumir/
%if 0%{?opensuse_bs}
BuildRequires:	-post-build-checks -rpmlint-Factory
%endif

%description
Second generation of well-known Kumir system

%prep
%setup

%build
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%buildroot/usr
make -j 8

%install
cd build
make install

%clean
rm -rf %buildroot


%package bc
Summary:	Kumir compiler tool
Requires:	qt >= 4.7.0
Requires:	libQtCore.so.4
Requires:	libQtGui.so.4

%description bc
A console tool to compile Kumir program sources into 
executable byte code

%files bc
%defattr(-,root,root)
%dir %_libdir/kumir2/*
%dir %_bindir
%dir %_datadir/kumir2/*
%_libdir/kumir2/*
%_datadir/kumir2/*
%_bindir/kumir2-bc

%post bc -p /sbin/ldconfig

%postun bc -p /sbin/ldconfig

%package run
Summary:	Lightweight console Kumir bytecode interpreter

%description run
A stack-based executable engine with build-in standard modules
for evaluating Kumir programs


%files run
%defattr(-,root,root)
%dir %_bindir
%_bindir/kumir2-run


%changelog
* Thu Jan 24 2013 - Victor Yacovlev <kumir@lpm.org.ru>
- Initial public release of console-only tools

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

