if(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            set(CMAKE_CXX_FLAGS "-Zm300 -Zc:wchar_t- -EHsc -MDd -GR -Y- -Zi")
        else()
            set(CMAKE_CXX_FLAGS "-Zm300 -Zc:wchar_t- -EHsc -MD -GR -Y-")
        endif()
else()
    if(NOT APPLE)
        set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS}"-Wl,--no-undefined")
    endif(NOT APPLE)
endif(MSVC)


if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_CXX_FLAGS "-Werror -Wreturn-type ${CMAKE_CXX_FLAGS}")
    endif()
endif()

if(APPLE)
    set(CMAKE_CXX_FLAGS "-DAPPLE ${CMAKE_CXX_FLAGS}")
endif()

if(MSVC_IDE)
	#set(CMAKE_SHARED_LIBRARY_PREFIX "../")
	#set(CMAKE_IMPORT_LIBRARY_PREFIX "../")
endif()
if(XCODE)
	set(CMAKE_SHARED_LIBRARY_PREFIX "../lib")
	set(CMAKE_IMPORT_LIBRARY_PREFIX "../lib")
	
#	if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
#		set(CMAKE_OSX_ARCHITECTURES x86_64)
#	else()
#		set(CMAKE_OSX_ARCHITECTURES i386)
#	endif()
	
endif(XCODE)

if(EXISTS(version_info.cmake))
    include(version_info.cmake)
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
    add_definitions(-DGIT_TIMESTAMP=${GIT_TIMESTAMP})
    add_definitions(-DGIT_TAG=\"${GIT_TAG}\")
    add_definitions(-DGIT_BRANCH=\"${GIT_BRANCH}\")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(shared)
add_subdirectory(plugins)
#if (NOT WIN32)
    add_subdirectory(actors)
#endif()
add_subdirectory(app)
add_subdirectory(tools)
