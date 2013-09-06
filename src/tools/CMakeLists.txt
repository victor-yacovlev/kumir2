if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_subdirectory(as)
endif()
add_subdirectory(run)

if(UNIX AND NOT APPLE)
    add_subdirectory(pictomir2course)
endif()
