Prerequirements to build
------------------------

0. QtSDK MinGW - http://qt.nokia.com/
1. Python 2.x - http://www.python.org/
2. Java Runtime Environment - http://www.oracle.com/
3. FreePascal Compiler - http://www.freepascal.org/
4. CMake - http://www.cmake.org/


Native generator toolchain
--------------------------

Win32 build uses CLang+MinGW compiler toolchain.
Precompiled binaries can be downloaded using svn:

    cd ${kumir2_build_dir}
    svn export http://lpm.org.ru/svn/kumir2-support/clang-mingw/


Building
--------
1. Set PATH environment variable pointing to 
   cmake.exe, qmake.exe, mingw32-make.exe and fpc.exe.
2. Open terminal and cd to Kumir2 source root.
3. Type 'mkdir build' and then 'cd build'. This will make build directory.
4. Type 'cmake -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=C:\kumir2-dist ../'.
   This will make skeleton of build and Makefiles.
   C:\kumir2-dist points to place, where kumir files will be copied after pseudo-installation.
5. Type 'mingw32-make'. Be patient - build process takes a while.
6. Type 'mingw32-make install'. This will copy kumir files to installation directory
   (e.g. C:\kumir2-dist)
