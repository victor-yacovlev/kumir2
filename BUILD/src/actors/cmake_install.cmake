# Install script for directory: /Users/denis/kumir2Master/src/actors

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/denis/kumir2Master/BUILD/src/actors/_colorer/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/complexnumbers/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/draw/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/grasshopper/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/isometricrobot/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/keyboard/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/painter/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/robot/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/turtle/cmake_install.cmake")
  include("/Users/denis/kumir2Master/BUILD/src/actors/vodoley/cmake_install.cmake")

endif()

