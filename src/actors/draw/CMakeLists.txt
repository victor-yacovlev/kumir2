project(ActorDraw)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
    set(MOC_PARAMS "-I/usr/include/qt5/QtCore" "-I${_qt5Core_install_prefix}/include/QtCore")
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

find_package(PythonInterp 2.6 REQUIRED)
include(../../kumir2_plugin.cmake)

set(SOURCES
    drawmodule.cpp
)

set(MOC_HEADERS
    drawmodule.h
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/drawmodulebase.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/drawmodulebase.h
        ${CMAKE_CURRENT_BINARY_DIR}/drawplugin.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/drawplugin.h
        ${CMAKE_CURRENT_BINARY_DIR}/ActorDraw.pluginspec
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
            --update
            ${CMAKE_CURRENT_SOURCE_DIR}/draw.json
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/draw.json
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(
    ActorDrawPluginSpec
    ALL
    ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/ActorDraw.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorDraw.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/drawmodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        -I${CMAKE_SOURCE_DIR}/src/shared
        ${MOC_PARAMS}
        -o${CMAKE_CURRENT_BINARY_DIR}/drawmodulebase.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/drawmodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/drawmodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/drawplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        ${MOC_PARAMS}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/drawplugin.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/drawplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/drawplugin.h
)

set(SOURCES_GENERATED
    drawmodulebase.cpp
    drawplugin.cpp
)

set(MOC_SOURCES_GENERATED
    drawmodulebase.moc.cpp
    drawplugin.moc.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()

install(
    FILES ${PLUGIN_OUTPUT_PATH}/ActorDraw.pluginspec
    DESTINATION ${PLUGINS_DIR}
)

handleTranslation(ActorDraw)

add_library(
    ActorDraw
    SHARED
    ${MOC_SOURCES} ${SOURCES}
    ${MOC_SOURCES_GENERATED} ${SOURCES_GENERATED}
)

add_dependencies(ActorDraw Actor_Colorer)

target_link_libraries(
    ActorDraw
    ${QT_LIBRARIES}
    ExtensionSystem
    Widgets
    ${STDCXX_LIB} ${STDMATH_LIB}
)

copyResources(actors/draw)

install(
    TARGETS ActorDraw
    DESTINATION ${PLUGINS_DIR}
)
