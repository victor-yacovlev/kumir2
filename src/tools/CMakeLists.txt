include(${CMAKE_SOURCE_DIR}/kumir2_common.cmake)

# -- linux
if(NOT APPLE AND NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'\$ORIGIN/../${LIB_BASENAME}/kumir2'")
endif(NOT APPLE AND NOT MSVC)
# -- mac
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'../Plugins'")
endif(APPLE)

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


