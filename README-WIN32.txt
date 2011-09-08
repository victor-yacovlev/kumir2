Prerequirements to build
------------------------

1. Python 2.x

    Kumir 2 build infrastructure uses several python scripts.
    Do the following before running qmake:
	a) Download and install Python 2.x from http://www.python.org/
	b) Set PATH system environment to Python2x.exe location

2. Google Web Toolkit
    
    Kumir 2 WebKit-based parts use GWT >= 2.3
    To setup GWT:
	a) Download and install recent version of Java SE (JDK) from http://www.oracle.com/
	b) Download and unzip ant build tool from http://ant.apache.org/
	c) Download and unzip GWT SDK from http://code.google.com/webtoolkit/
	d) Set PATH system environment to javac.exe, java.exe and ant.bat locations


Native generator toolchain
--------------------------

Win32 build uses CLang+MinGW compiler toolchain.
Precompiled binaries can be downloaded using svn:

    cd ${kumir2_build_dir}
    svn export http://lpm.org.ru/svn/kumir2-support/clang-mingw/


Building WebKit-baset parts
---------------------------

Run commands on each WebKit part ( ${web_app_name} ):
    cd ${kumir2_src_root}/src-www/${web_app_name}
    ant -Dgwt.sdk=${path_to_gwt_sdk}
    
Copy all contents of ${web_app_name}/war except directory WEB-INF into
${kumir2_build_dir}/share/kumir2/webapps/${web_app_name}/

For helpviewer app copy data contents (user documentation stored in *.xml files)
    from ${kumir2_src_root}/userdocs/
    to ${kumir2_build_root}/share/kumir2/webapps/helpviewer/data/russian/


