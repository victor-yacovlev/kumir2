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
        set(CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
    endif()
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

add_custom_target(
    GitInfoHeader ALL
    ${CMAKE_SOURCE_DIR}/scripts/git_info.py --out=${CMAKE_CURRENT_BINARY_DIR}/GITINFO.h --root=${CMAKE_SOURCE_DIR}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(shared)
add_subdirectory(plugins)
#if (NOT WIN32)
    add_subdirectory(actors)
#endif()
add_subdirectory(app)
add_subdirectory(tools)
