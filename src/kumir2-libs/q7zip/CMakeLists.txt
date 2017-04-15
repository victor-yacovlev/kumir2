cmake_minimum_required(VERSION 2.8)
project(q7zip)
set(CMAKE_VERBOSE_MAKEFILE 1)

find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

if(MSVC)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_CXX_FLAGS "-Zc:wchar_t- -EHsc- -MDd -GR -Y- -Zi")
    else()
        set(CMAKE_CXX_FLAGS "-Zc:wchar_t- -EHsc- -MD -GR -Y-")
    endif()
else()
    set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

if(WIN32)
    include(7z.cmake)
else()
    include(p7zip.cmake)
endif()

set(SOURCES
    archive.cpp
    archiveimpl.cpp
    sevenziparchive.cpp
    sevenziparchiveimpl.cpp    
    ziparchive.cpp
    ziparchiveimpl.cpp
    instream.cpp
    outstream.cpp
    extractcallback.cpp
)


set(MOC_HEADERS
    archive.h
    sevenziparchive.h
    ziparchive.h
)

set(PUBLIC_HEADERS
    archive.h
    sevenziparchive.h
    ziparchive.h
)

add_definitions(-DQ7ZIP_LIBRARY)
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

add_library(Q7Zip SHARED ${SOURCES_7Z} ${MOC_SOURCES} ${SOURCES})

if(NOT APPLE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")
    set(STDCXX_LIB "stdc++")
    set(STDMATH_LIB "m")
endif()


target_link_libraries(Q7Zip ${QT_LIBRARIES} ${EXTRA_LIBS} ${STDCXX_LIB})

if(WIN32)
    set(LIBS_DIR lib)
endif(WIN32)

if(NOT DEFINED(LIBS_DIR))
    if((${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") AND NOT (EXISTS /etc/ubuntu-release) AND NOT (EXISTS /etc/debian_version))
        set(LIBS_DIR lib64)
    else()
        set(LIBS_DIR lib)
    endif()
endif()

install(TARGETS Q7Zip LIBRARY DESTINATION ${LIBS_DIR} ARCHIVE DESTINATION ${LIBS_DIR})
install(FILES ${PUBLIC_HEADERS} DESTINATION "include/Q7Zip/")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_executable(test-q7z test.cpp)
    target_link_libraries(test-q7z Q7Zip ${QT_LIBRARIES} ${EXTRA_LIBS} ${STDCXX_LIB})
    set_target_properties(test-q7z PROPERTIES COMPILE_DEFINITIONS "DISABLE_EXPORT_LIBQ7Z")
endif()
