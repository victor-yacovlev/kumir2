if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_opt_subdirectory(as)
endif()
add_opt_subdirectory(pictomir2course)
add_opt_subdirectory(docbook2latex)

if(NOT APPLE)
    add_opt_subdirectory(open)
endif()

add_opt_subdirectory(courseeditor)
add_opt_subdirectory(run)


