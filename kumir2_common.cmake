find_package(PythonInterp 2.7.0 REQUIRED)

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/read_disabled_modules.py"
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