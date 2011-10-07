if(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(MSVC_IDE)
	set(CMAKE_SHARED_LIBRARY_PREFIX "../")
	set(CMAKE_IMPORT_LIBRARY_PREFIX "../")
endif()

add_subdirectory(shared)
add_subdirectory(plugins)
add_subdirectory(actors)
add_subdirectory(app)
add_subdirectory(tools)
