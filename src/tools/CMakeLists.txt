if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_subdirectory(as)
endif()
add_subdirectory(run)

if(NOT APPLE AND NOT WIN32)
    add_subdirectory(pictomir2course)
endif()

add_subdirectory(docbook2latex)
