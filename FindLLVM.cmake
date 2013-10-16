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

find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config PATHS LLVM_ROOT)

if(LLVM_CONFIG_EXECUTABLE)        
    exec_program(${LLVM_CONFIG_EXECUTABLE} ARGS "--libs ${COMPONENTS}" OUTPUT_VARIABLE LLVM_LIBRARIES)
    if(NOT WIN32)
        set(LLVM_LIBRARIES "${LLVM_LIBRARIES} -ldl -lm")
        set(LLVM_DEFINITIONS "-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS")
        # TODO implement correct def-parsing
    endif(NOT WIN32)
    find_path(LLVM_INCLUDE_DIR llvm/LLVMContext.h HINTS LLVM_ROOT)
    if (LLVM_INCLUDE_DIR)
        set(LLVM_FOUND 1)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    LLVM 
    FOUND_VAR LLVM_FOUND
    REQUIRED_VARS LLVM_LIBRARIES LLVM_DEFINITIONS LLVM_INCLUDE_DIR
    HANDLE_COMPONENTS
)
