# -- linux
if(NOT APPLE AND NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'\$ORIGIN/../${LIB_BASENAME}/kumir2'")
endif(NOT APPLE AND NOT MSVC)
# -- mac
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'../Plugins'")
endif(APPLE)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_subdirectory(as)
endif()
add_subdirectory(run)

if(NOT APPLE AND NOT WIN32)
    add_subdirectory(pictomir2course)
endif()

add_subdirectory(docbook2latex)
if (NOT APPLE)
    add_subdirectory(open)
endif()
add_subdirectory(courseeditor)
