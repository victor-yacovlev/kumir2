# -- linux
if(NOT APPLE AND NOT MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--enable-new-dtags -Wl,-rpath,'\$ORIGIN/../${LIB_BASENAME}/kumir2'")
endif(NOT APPLE AND NOT MSVC)
# -- mac
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,'../Plugins'")
endif(APPLE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/as")
    if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_subdirectory(as)
    endif()
endif()

add_subdirectory(run)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pictomir2course")
    if(NOT APPLE AND NOT WIN32)
	add_subdirectory(pictomir2course)
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/docbook2latex")
    add_subdirectory(docbook2latex)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/open")
    if(NOT APPLE)
	add_subdirectory(open)
    endif()
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/courseeditor")
    add_subdirectory(courseeditor)
endif()

