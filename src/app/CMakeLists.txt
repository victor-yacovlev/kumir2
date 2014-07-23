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
    set(GIT_HASH unknown)
    set(GIT_TIMESTAMP 0)
    set(GIT_BRANCH unknown)
    set(GIT_TAG unknown)

    find_program(GIT git)

    if(GIT)
        execute_process(
            COMMAND ${GIT} --no-pager log -1 --pretty=format:%H
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_HASH
        )
        execute_process(
            COMMAND ${GIT} describe --abbrev=0 --tags
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG
            RESULT_VARIABLE GIT_TAG_ERROR
            ERROR_VARIABLE NULL
        )
        if(NOT "${GIT_TAG_ERROR}" EQUAL "0")
            set(GIT_TAG unknown)
        endif()
        execute_process(
            COMMAND ${GIT} --no-pager log -1 --pretty=format:%ct
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TIMESTAMP
        )
        execute_process(
            COMMAND ${GIT} rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_BRANCH
        )
        string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
        string(STRIP "${GIT_HASH}" GIT_HASH)
        set(GIT_TAG "unknown") #        string(STRIP "${GIT_TAG}" GIT_TAG)
        string(STRIP "${GIT_TIMESTAMP}" GIT_TIMESTAMP)
    else()
        message(WARNING "git not found and no version info provided")
    endif()

    add_definitions(-DGIT_HASH=\"${GIT_HASH}\")
    add_definitions(-DGIT_TIMESTAMP=\"${GIT_TIMESTAMP}\")
    add_definitions(-DGIT_TAG=\"${GIT_TAG}\")
    add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
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
    add_subdirectory(kumir2-classic)
    add_subdirectory(kumir2-highgrade)
    add_subdirectory(kumir2-ide)
    add_subdirectory(kumir2-teacher)

    # Check if Python present in this git branch and Python3 libraries found
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kumir2-python")
        if(PYTHONLIBS_FOUND)
            add_subdirectory(kumir2-python)
        else()
            message(WARNING "No Python 3.x libs found, building of Python language support disabled")
        endif()
    endif() # kumir2-python exists

    # Check if Pascal present in this git branch
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kumir2-pascal")
        add_subdirectory(kumir2-pascal)
    endif() # kumir2-pascal exists
endif(NOT APPLE)

if(APPLE)
    # Apple platform has exact one GUI configuration
    add_subdirectory(kumir2-macx)
endif(APPLE)

add_subdirectory(kumir2-bc)
add_subdirectory(kumir2-xrun)

# kumir2-llvmc is optional in case if LLVM libraries present
if(Llvm_FOUND)
    add_subdirectory(kumir2-llvmc)
else()
    message(WARNING "LLVM developer libraries not found, building of LLVM support disabled")
endif(Llvm_FOUND)
