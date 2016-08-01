include(${CMAKE_SOURCE_DIR}/kumir2_common.cmake)

# -- linux
if(NOT APPLE AND NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'\$ORIGIN/../${LIB_BASENAME}/kumir2'")
endif(NOT APPLE AND NOT MSVC)
# -- mac
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'../Plugins'")
endif(APPLE)

find_package(PythonLibs 3.2)
find_package(Llvm)

if(EXISTS "${CMAKE_SOURCE_DIR}/version_info.cmake")
    message(STATUS "Found explicit version info")
    include(${CMAKE_SOURCE_DIR}/version_info.cmake)

    add_definitions(-DGIT_HASH=\"${GIT_HASH}\")
    add_definitions(-DGIT_TIMESTAMP=\"${GIT_TIMESTAMP}\")
    add_definitions(-DGIT_TAG=\"${GIT_TAG}\")
    add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
else()
    find_package(PythonInterp 3.2.0 REQUIRED)

    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/query_version_info.py" "--mode=cmake_version_info"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VERSION_DEFINITIONS
    )
    add_definitions(${VERSION_DEFINITIONS})
endif()

if(MSVC_IDE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
endif (MSVC_IDE)

if(NOT APPLE)
    # Various GUI configurations

    add_opt_subdirectory(kumir2-classic)
    add_opt_subdirectory(kumir2-highgrade)
    add_opt_subdirectory(kumir2-ide)
    add_opt_subdirectory(kumir2-teacher)

    # Check if Python present in this git branch and Python3 libraries found
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kumir2-python")
        if(PYTHONLIBS_FOUND)
            add_opt_subdirectory(kumir2-python)
            add_opt_subdirectory(kumir2-python-teacher)
        else()
            message(WARNING "No Python 3.x libs found, building of Python language support disabled")
        endif()
    endif() # kumir2-python exists

endif(NOT APPLE)

if(APPLE)
    # Apple platform has exact one GUI configuration
    add_opt_subdirectory(kumir2-macx)
endif(APPLE)

add_opt_subdirectory(kumir2-bc)
add_opt_subdirectory(kumir2-xrun)

# kumir2-llvmc is optional in case if LLVM libraries present
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kumir2-llvmc")
    if(Llvm_FOUND)
        add_opt_subdirectory(kumir2-llvmc)
    else()
        message(WARNING "LLVM developer libraries not found, building of LLVM support disabled")
    endif(Llvm_FOUND)
endif()
