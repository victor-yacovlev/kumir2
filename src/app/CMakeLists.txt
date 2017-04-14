find_package(Kumir2 REQUIRED)
# find_package(Llvm)

# Query for version info and pass it as compile definitions
if(EXISTS "${CMAKE_SOURCE_DIR}/version_info.cmake")
    message(STATUS "Found explicit version info")
    include(${CMAKE_SOURCE_DIR}/version_info.cmake)

    add_definitions(-DGIT_HASH=\"${GIT_HASH}\")
    add_definitions(-DGIT_TIMESTAMP=\"${GIT_TIMESTAMP}\")
    add_definitions(-DGIT_TAG=\"${GIT_TAG}\")
    add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
else()
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/query_version_info.py" "--mode=cmake_version_info"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VERSION_DEFINITIONS
    )
    add_definitions(${VERSION_DEFINITIONS})
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
