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
            COMMAND ${GIT} tag
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_TAG
        )
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
        string(STRIP "${GIT_TAG}" GIT_TAG)
        string(STRIP "${GIT_TIMESTAMP}" GIT_TIMESTAMP)
    else()
        message(WARNING "git not found and no version info provided")
    endif()

    add_definitions(-DGIT_HASH=\"${GIT_HASH}\")
    add_definitions(-DGIT_TIMESTAMP=\"${GIT_TIMESTAMP}\")
    add_definitions(-DGIT_TAG=\"${GIT_TAG}\")
    add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
endif()

if (MSVC_IDE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIBRARY_OUTPUT_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIBRARY_OUTPUT_PATH})
endif (MSVC_IDE)

if(NOT APPLE)

    set(CMAKE_INSTALL_RPATH
        "$ORIGIN/../${LIBS_DIR}/"
    )

    add_subdirectory(kumir2-classic)
    add_subdirectory(kumir2-highgrade)
    add_subdirectory(kumir2-ide)
    add_subdirectory(kumir2-teacher)
    #add_subdirectory(kumir2-python)    
endif(NOT APPLE)

if(APPLE)
    add_subdirectory(kumir2-macx)
endif(APPLE)



#add_subdirectory(kumir2-cc)
add_subdirectory(kumir2-bc)
add_subdirectory(kumir2-xrun)

find_package(LLVM)
if(LLVM_FOUND)
    add_subdirectory(kumir2-llvmc)
else()
    message(WARNING "LLVM developer libraries not found, building of LLVM support disabled")
endif(LLVM_FOUND)
