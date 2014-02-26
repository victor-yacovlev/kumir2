project(gdbmi)
cmake_minimum_required(VERSION 2.8)

set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../kumir2_library.cmake")
    message(STATUS "GDBMI library will be build as a part of Kumir")
    include(../../kumir2_library.cmake)
    set(BUILD_AS_PART_OF_KUMIR 1)
    add_definitions(-DBUILD_AS_PART_OF_KUMIR)
else()
    set(BUILD_AS_PART_OF_KUMIR 0)
endif()

set(SOURCES
    midebugger.cpp
    midebuggerprivate.cpp
    gdbmivalue.cpp
    breakhandler.cpp
    breakpoint.cpp
    stackframe.cpp
)

set(MOC_HEADERS
    midebugger.h
    midebuggerprivate.h
    breakhandler.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
add_definitions("-DGDBMI_LIBRARY")
add_library(GdbMi SHARED ${SOURCES} ${MOC_SOURCES})
target_link_libraries(GdbMi ${STDCXX_LIB} ${QT_LIBRARIES})


include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

