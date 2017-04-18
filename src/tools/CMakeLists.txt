if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_opt_subdirectory(as)
endif()
if(NOT MSVC OR MSVC_VERSION LESS 1900)
    add_opt_subdirectory(pictomir2course)
endif()
add_opt_subdirectory(docbook2latex)

if(NOT APPLE)
    add_opt_subdirectory(open)
endif()

add_opt_subdirectory(courseeditor)
add_opt_subdirectory(run)


