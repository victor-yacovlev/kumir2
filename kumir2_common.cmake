find_package(PythonInterp 3.2.0 REQUIRED)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/query_version_info.py" "--mode=cmake_disabled_modules"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE DISABLED_SUBDIRS
)
   
macro(add_opt_subdirectory SUBDIR_NAME)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR_NAME})
        string(FIND "${DISABLED_SUBDIRS}" "[${SUBDIR_NAME}]" SubdirDisableIndex)
        if(${SubdirDisableIndex} EQUAL -1)
            add_subdirectory(${SUBDIR_NAME})
        else()
            message(STATUS "Component ${SUBDIR_NAME} disabled for this version")
        endif()
    endif()
endmacro(add_opt_subdirectory)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(NOT USE_QT GREATER 4)
    message(WARNING "Support for Qt4 will be discontinued soon. Please upgrade to Qt5!")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_VERBOSE_MAKEFILE on)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

include_directories(${CMAKE_SOURCE_DIR}/src/shared)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Finds Qt modules by components name
function(use_qt)
    set(_QT_COMPONENTS )
    foreach(_component ${ARGN})
        if(${USE_QT} GREATER 4)
            if(${_component} MATCHES "Gui")
                list(APPEND _QT_COMPONENTS "Gui" "Widgets")
            else()
                list(APPEND _QT_COMPONENTS ${_component})
            endif()
        else()
            list(APPEND _QT_COMPONENTS "Qt${_component}")
        endif()
    endforeach(_component)
endfunction(use_qt)
