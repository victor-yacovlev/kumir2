# Install script for directory: /Users/denis/kumir2Master/src/tools/run

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS" TYPE EXECUTABLE FILES "/Users/denis/kumir2Master/BUILD/Kumir.app/Contents/MacOS/kumir2-run")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      endif()
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS" TYPE EXECUTABLE FILES "/Users/denis/kumir2Master/BUILD/Kumir.app/Contents/MacOS/kumir2-run")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      endif()
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS" TYPE EXECUTABLE FILES "/Users/denis/kumir2Master/BUILD/Kumir.app/Contents/MacOS/MinSizeRel/kumir2-run")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      endif()
    endif()
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS" TYPE EXECUTABLE FILES "/Users/denis/kumir2Master/BUILD/Kumir.app/Contents/MacOS/RelWithDebInfo/kumir2-run")
    if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/Kumir.app/Contents/MacOS/kumir2-run")
      endif()
    endif()
  endif()
endif()

