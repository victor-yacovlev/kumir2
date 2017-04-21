find_package(Kumir2 REQUIRED)
# find_package(Llvm)

# Query for version info and pass it as compile definitions
if(EXISTS "${CMAKE_SOURCE_DIR}/version_info.cmake")
    include(${CMAKE_SOURCE_DIR}/version_info.cmake)

    add_definitions(-DGIT_HASH=\"${GIT_HASH}\")
    add_definitions(-DGIT_TIMESTAMP=\"${GIT_TIMESTAMP}\")
    add_definitions(-DGIT_TAG=\"${GIT_TAG}\")
    add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
    message(STATUS "File-provided version info: HASH=${GIT_HASH}")
    message(STATUS "File-provided version info: TIMESTAMP=${GIT_TIMESTAMP}")
    message(STATUS "File-provided version info: TAG=${GIT_TAG}")
    message(STATUS "File-provided version info: BRANCH=${GIT_BRANCH}")
elseif(PROVIDED_VERSION_INFO)
    add_definitions(-DGIT_HASH=\"${GIT_HASH}\")
    add_definitions(-DGIT_TIMESTAMP=\"${GIT_TIMESTAMP}\")
    add_definitions(-DGIT_TAG=\"${GIT_TAG}\")
    add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
    message(STATUS "Variable-provided version info: HASH=${GIT_HASH}")
    message(STATUS "Variable-provided version info: TIMESTAMP=${GIT_TIMESTAMP}")
    message(STATUS "Variable-provided version info: TAG=${GIT_TAG}")
    message(STATUS "Variable-provided version info: BRANCH=${GIT_BRANCH}")
else()
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/query_version_info.py" "--mode=cmake_version_info"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VERSION_DEFINITIONS
    )
    add_definitions(${VERSION_DEFINITIONS})
    message(STATUS "Queried version info from git: ${VERSION_DEFINITIONS}")
endif()

#if(NOT APPLE)
    # Various GUI configurations

    add_opt_subdirectory(kumir2-classic)
    add_opt_subdirectory(kumir2-highgrade)
    add_opt_subdirectory(kumir2-ide)
    add_opt_subdirectory(kumir2-teacher)
    #add_opt_subdirectory(kumir2-checkcourse)

    # Check if Python present in this git branch and Python3 libraries found
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kumir2-python")
        add_opt_subdirectory(kumir2-python)
        add_opt_subdirectory(kumir2-python-teacher)
    endif() # kumir2-python exists

#endif(NOT APPLE)

#if(APPLE)
    # Apple platform has exact one GUI configuration
  #  add_opt_subdirectory(kumir2-macx)
#endif(APPLE)

add_opt_subdirectory(kumir2-bc)
add_opt_subdirectory(kumir2-xrun)

# kumir2-llvmc is optional in case if LLVM libraries present
#if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kumir2-llvmc")
#    if(Llvm_FOUND)
#        add_opt_subdirectory(kumir2-llvmc)
#    else()
#        message(WARNING "LLVM developer libraries not found, building of LLVM support disabled")
#    endif(Llvm_FOUND)
#endif()
