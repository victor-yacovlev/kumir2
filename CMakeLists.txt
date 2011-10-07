project(Kumir2)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtScript QtNetwork QtXml QtWebKit REQUIRED)
include (${QT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR}/src/shared)
include_directories(${CMAKE_SOURCE_DIR}/src)




if((${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") AND NOT (EXISTS /etc/ubuntu-release))
    set(LIB_BASENAME lib64)
else()
    set(LIB_BASENAME lib)
endif()

if(WIN32)
  set(EXEC_DIR bin)
  set(LIBS_DIR bin)
  set(LIB_BASENAME lib)
  set(PLUGINS_DIR lib/kumir2/plugins)
  set(RESOURCES_DIR share/kumir2)
  set(LIBEXECS_DIR bin)
elseif(APPLE)
  set(EXEC_DIR Kumir.app/Contents/MacOS)
  set(LIBS_DIR Kumir.app/Contents/PlugIns)
  set(PLUGINS_DIR Kumir.app/Contents/PlugIns)
  set(RESOURCES_DIR Kumir.app/Contents/Resources)
  set(LIBEXECS_DIR Kumir.app/Contents/MacOS)
else()
  set(EXEC_DIR bin)
  set(LIBS_DIR ${LIB_BASENAME}/kumir2)
  set(PLUGINS_DIR ${LIB_BASENAME}/kumir2/plugins)
  set(RESOURCES_DIR share/kumir2)
  set(LIBEXECS_DIR libexec/kumir2)
endif()

add_definitions(-DIDE_LIBRARY_BASENAME="${LIB_BASENAME}")

set(PLUGIN_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${PLUGINS_DIR}
)

set(SHARE_PATH ${CMAKE_BINARY_DIR}/${RESOURCES_DIR})

set(LIBRARY_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${LIBS_DIR}
)

set(EXECUTABLE_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${EXEC_DIR}
)

set(LIBEXECS_PATH ${CMAKE_BINARY_DIR}/${LIBEXECS_DIR})

add_subdirectory(src)
add_subdirectory(src-www)
add_subdirectory(share/kumir2/icons)

if(UNIX AND NOT APPLE)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/applications)
    file(COPY ${CMAKE_SOURCE_DIR}/kumir2-professional.desktop DESTINATION ${CMAKE_BINARY_DIR}/share/applications)
    file(COPY ${CMAKE_SOURCE_DIR}/kumir2-standard.desktop DESTINATION ${CMAKE_BINARY_DIR}/share/applications)
    file(COPY ${CMAKE_SOURCE_DIR}/kumir2-teacher.desktop DESTINATION ${CMAKE_BINARY_DIR}/share/applications)
    install(FILES ${CMAKE_BINARY_DIR}/share/applications/kumir2-professional.desktop DESTINATION share/applications)
    install(FILES ${CMAKE_BINARY_DIR}/share/applications/kumir2-standard.desktop DESTINATION share/applications)
    install(FILES ${CMAKE_BINARY_DIR}/share/applications/kumir2-teacher.desktop DESTINATION share/applications)
    file(GLOB_RECURSE icons RELATIVE ${CMAKE_SOURCE_DIR}/app_icons/linux ${CMAKE_SOURCE_DIR}/app_icons/linux/*)
    foreach(icon IN ITEMS ${icons})
        if(NOT ${icon} MATCHES .svn*)
            get_filename_component(subdir ${icon} PATH)
            file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/icons/${subdir})
            file(COPY ${CMAKE_SOURCE_DIR}/app_icons/linux/${icon} DESTINATION ${CMAKE_BINARY_DIR}/share/icons/${subdir})
            install(FILES ${CMAKE_BINARY_DIR}/share/icons/${icon} DESTINATION share/icons/${subdir})
        endif()
    endforeach(icon)
endif(UNIX AND NOT APPLE)
