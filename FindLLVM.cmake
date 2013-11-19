# Copyright (C) 2013 Victor Yacovlev <victor@lpm.org.ru>
# You can use this file under the terms of GNU LGPL 2 licensee
# Find LLVM devel libraries

# Usage:
#  find_package(LLVM 
#    [REQUIRED]
#    [COMPONENTS <libs>...]
#  )
 
# As of result, the module creates the following variables:
#  LLVM_FOUND
#  LLVM_INCLUDE_DIR
#  LLVM_LIBRARIES
#  LLVM_DEFINITIONS
# This module relies on following variable to search
#  LLVM_ROOT

# ------------------ implementation

find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config PATHS ${LLVM_ROOT})

if(LLVM_CONFIG_EXECUTABLE)
    exec_program(${LLVM_CONFIG_EXECUTABLE} ARGS "--version" OUTPUT_VARIABLE LLVM_VERSION)
    if(EXISTS /usr/${LIB_BASENAME}/llvm/libLLVM-${LLVM_VERSION}.so)
        # Fedora-specific .so-version
        set(LLVM_LIBRARIES "-L/usr/${LIB_BASENAME}/llvm -lLLVM-${LLVM_VERSION}")
        set(LLVM_DEFINITIONS " ")
    else()
        exec_program(${LLVM_CONFIG_EXECUTABLE} ARGS "--libs Linker BitReader BitWriter" OUTPUT_VARIABLE LLVM_LIBRARIES)
        if(NOT WIN32)
            set(LLVM_LIBRARIES "${LLVM_LIBRARIES} -ldl -lm")
            set(LLVM_DEFINITIONS "-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
        endif(NOT WIN32)
        set(LLVM_DEFINITIONS " ")
    endif()
    find_path(LLVM_INCLUDE_DIR llvm/Config/llvm-config.h HINTS ${LLVM_ROOT})
    if (LLVM_INCLUDE_DIR)
        message(STATUS "Found LLVM includes: ${LLVM_INCLUDE_DIR}")
        message(STATUS "Found LLVM libs: ${LLVM_LIBRARIES}")
        set(LLVM_FOUND 1)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LLVM LLVM_LIBRARIES LLVM_DEFINITIONS LLVM_INCLUDE_DIR)
mark_as_advanced(LLVM_LIBRARIES LLVM_DEFINITIONS LLVM_INCLUDE_DIR)
