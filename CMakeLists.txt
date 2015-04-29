project(Kumir2)

cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

#if(APPLE)
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
#endif(APPLE)
#set(QT_USE_QTMAIN 1)
#find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtScript QtNetwork QtXml QtSvg QtWebKit QtDeclarative REQUIRED)
#include (${QT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR}/src/shared)
include_directories(${CMAKE_SOURCE_DIR}/src)

#if(WIN32)
#    include_directories(${CMAKE_SOURCE_DIR}/boost_include)
#endif()



if((${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") AND NOT (EXISTS /etc/ubuntu-release) AND NOT (EXISTS /etc/debian_version))
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
    if(XCODE)
        set(EXEC_DIR Debug/Kumir.app/Contents/MacOS/)
        set(LIBS_DIR Debug/Kumir.app/Contents/PlugIns)
        set(PLUGINS_DIR Debug/Kumir.app/Contents/PlugIns)
        set(RESOURCES_DIR Debug/Kumir.app/Contents/Resources)
        set(LIBEXECS_DIR Debug/Kumir.app/Contents/MacOS)
    else()
        set(EXEC_DIR Kumir.app/Contents/MacOS)
        set(LIBS_DIR Kumir.app/Contents/PlugIns)
        set(PLUGINS_DIR Kumir.app/Contents/PlugIns)
        set(RESOURCES_DIR Kumir.app/Contents/Resources)
        set(LIBEXECS_DIR Kumir.app/Contents/MacOS)
    endif(XCODE)
else()
  set(EXEC_DIR bin)
  set(LIBS_DIR ${LIB_BASENAME}/kumir2)
  set(PLUGINS_DIR ${LIB_BASENAME}/kumir2/plugins)
  set(RESOURCES_DIR share/kumir2)
  set(LIBEXECS_DIR libexec/kumir2)
endif()

add_definitions(-DIDE_LIBRARY_BASENAME="${LIB_BASENAME}")

set(PLUGIN_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PLUGINS_DIR}
)

set(SHARE_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${RESOURCES_DIR})

set(LIBRARY_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${LIBS_DIR}
)

set(EXECUTABLE_OUTPUT_PATH
    ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXEC_DIR}
)

set(LIBEXECS_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${LIBEXECS_DIR})

add_subdirectory(src)
add_subdirectory(userdocs)
add_subdirectory(share/kumir2/courses)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications)
file(GLOB_RECURSE icons RELATIVE ${CMAKE_SOURCE_DIR}/app_icons/linux ${CMAKE_SOURCE_DIR}/app_icons/linux/*)
foreach(icon IN ITEMS ${icons})
    get_filename_component(subdir ${icon} PATH)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/icons/${subdir})
    file(COPY ${CMAKE_SOURCE_DIR}/app_icons/linux/${icon} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/icons/${subdir})
    install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/icons/${icon} DESTINATION share/icons/${subdir})
endforeach(icon)

# TODO move into python subproject CMakeLists.txt
if(EXISTS ${CMAKE_SOURCE_DIR}/kumir2-python.desktop)
    file(COPY ${CMAKE_SOURCE_DIR}/kumir2-python.desktop DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications)
    install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications/kumir2-python.desktop DESTINATION share/applications)
endif()


file(MAKE_DIRECTORY ${SHARE_PATH}/icons)
file(GLOB_RECURSE icons RELATIVE ${CMAKE_SOURCE_DIR}/share/kumir2/icons ${CMAKE_SOURCE_DIR}/share/kumir2/icons/*)
foreach(icon IN ITEMS ${icons})
    get_filename_component(subdir ${icon} PATH)
    file(MAKE_DIRECTORY ${SHARE_PATH}/icons/${subdir})
    file(COPY ${CMAKE_SOURCE_DIR}/share/kumir2/icons/${icon} DESTINATION ${SHARE_PATH}/icons/${subdir})
    install(FILES ${SHARE_PATH}/icons/${icon} DESTINATION ${RESOURCES_DIR}/icons/${subdir})
endforeach(icon)
