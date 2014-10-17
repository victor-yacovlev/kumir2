# Copyright (C) 2013 Victor Yacovlev <victor@lpm.org.ru>
# You can use this file under the terms of GNU LGPL 2 licensee
# Find LLVM devel libraries

# Usage:
#  find_package(LLVM 
#    [REQUIRED]
#    [COMPONENTS <libs>...]
#  )
 
# As of result, the module creates the following variables:
#  Llvm_FOUND
#  Llvm_INCLUDE_DIR
#  Llvm_LIBRARIES
#  Llvm_DEFINITIONS
# This module relies on following variable to search
#  LLVM_ROOT

# ------------------ implementation
find_program(Llvm_CONFIG_EXECUTABLE NAMES llvm-config PATHS ${LLVM_ROOT} ${LLVM_ROOT}/bin)

if(Llvm_CONFIG_EXECUTABLE)
    exec_program(${Llvm_CONFIG_EXECUTABLE} ARGS "--version" OUTPUT_VARIABLE Llvm_VERSION)
    if(EXISTS /usr/${LIB_BASENAME}/llvm/libLLVM-${Llvm_VERSION}.so)
        # Fedora-specific .so-version
        set(Llvm_LIBRARIES "-L/usr/${LIB_BASENAME}/llvm -lLLVM-${Llvm_VERSION}")
        set(Llvm_DEFINITIONS "-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
    elseif(EXISTS /usr/${LIB_BASENAME}/llvm-${Llvm_VERSION}/${LIB_BASENAME}/libLLVM-${Llvm_VERSION}.so)
        # Ubuntu-specific .so-version
        set(Llvm_LIBRARIES "-L/usr/${LIB_BASENAME}/llvm-${Llvm_VERSION}/${LIB_BASENAME} -lLLVM-${Llvm_VERSION}")
        set(Llvm_DEFINITIONS "-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
    elseif(EXISTS /usr/${LIB_BASENAME}/libLLVM-${Llvm_VERSION}.so)
        # AltLinux-specific .so-version
        set(Llvm_LIBRARIES "-lLLVM-${Llvm_VERSION}")
        set(Llvm_DEFINITIONS "-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
    else()
        set(Llvm_REQUEST_STRING "")
        if(Llvm_FIND_COMPONENTS)
            foreach(component ${Llvm_FIND_COMPONENTS})
                set(Llvm_REQUEST_STRING "${LLVM_REQUEST_STRING} ${component}")
            endforeach()
        endif()
        exec_program(${Llvm_CONFIG_EXECUTABLE} ARGS "--libs ${Llvm_REQUEST_STRING}" OUTPUT_VARIABLE Llvm_LIBRARIES_STRING)
        string(REPLACE " " ";" Llvm_LIBS_LIST ${Llvm_LIBRARIES_STRING})
        foreach(_lib ${Llvm_LIBS_LIST})
            string(REPLACE "-l" "" _lib ${_lib})
#            find_library(_lib_path ${_lib} HINTS ${LLVM_ROOT} ${LLVM_ROOT}/lib ${LLVM_ROOT}/lib64)
            if(WIN32 AND CMAKE_BUILD_TYPE MATCHES "Debug")
                set(_lib "${_lib}_d")
            endif()
            set(Llvm_LIBRARIES "${Llvm_LIBRARIES};${_lib}")
        endforeach()
        if(NOT WIN32)
            set(Llvm_LIBRARIES "${Llvm_LIBRARIES};dl;m")
        endif(NOT WIN32)
        if(WIN32)
            link_directories("${LLVM_ROOT}/lib")
        endif(WIN32)
        set(Llvm_DEFINITIONS "-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
    endif()
    find_path(Llvm_INCLUDE_DIR llvm/Config/llvm-config.h HINTS ${LLVM_ROOT} ${LLVM_ROOT}/include)
    if (Llvm_INCLUDE_DIR)
#        message(STATUS "Found LLVM includes: ${Llvm_INCLUDE_DIR}")
#        message(STATUS "Found LLVM libs: ${Llvm_LIBRARIES}")
        set(Llvm_FOUND 1)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LLVM Llvm_LIBRARIES Llvm_DEFINITIONS Llvm_INCLUDE_DIR)
mark_as_advanced(Llvm_LIBRARIES Llvm_DEFINITIONS Llvm_INCLUDE_DIR)
