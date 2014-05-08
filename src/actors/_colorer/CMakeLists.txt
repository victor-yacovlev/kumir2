project(Actor_Colorer)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
    set(MOC_PARAMS "-I${_qt5Core_install_prefix}/include/QtCore")
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

find_package(PythonInterp 2.6 REQUIRED)
include(../../kumir2_plugin.cmake)

set(SOURCES
    _colorermodule.cpp
)

set(MOC_HEADERS
    _colorermodule.h
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/_colorermodulebase.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/_colorermodulebase.h
        ${CMAKE_CURRENT_BINARY_DIR}/_colorerplugin.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/_colorerplugin.h
        ${CMAKE_CURRENT_BINARY_DIR}/Actor_Colorer.pluginspec
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
            --update
            ${CMAKE_CURRENT_SOURCE_DIR}/_colorer.json
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/_colorer.json
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(
    Actor_ColorerPluginSpec
    ALL
    ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/Actor_Colorer.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Actor_Colorer.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_colorermodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        ${MOC_PARAMS}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/_colorermodulebase.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/_colorermodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/_colorermodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/_colorerplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        ${MOC_PARAMS}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/_colorerplugin.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/_colorerplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/_colorerplugin.h
)

set(SOURCES_GENERATED
    _colorermodulebase.cpp
    _colorerplugin.cpp
)

set(MOC_SOURCES_GENERATED
    _colorermodulebase.moc.cpp
    _colorerplugin.moc.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()

install(
    FILES ${PLUGIN_OUTPUT_PATH}/Actor_Colorer.pluginspec
    DESTINATION ${PLUGINS_DIR}
)

handleTranslation(Actor_Colorer)

add_library(
    Actor_Colorer
    SHARED
    ${MOC_SOURCES} ${SOURCES}
    ${MOC_SOURCES_GENERATED} ${SOURCES_GENERATED}
)

target_link_libraries(
    Actor_Colorer
    ${QT_LIBRARIES}
    ExtensionSystem
    Widgets
    ${STDCXX_LIB} ${STDMATH_LIB}
)

copyResources(actors/_colorer)

install(
    TARGETS Actor_Colorer
    DESTINATION ${PLUGINS_DIR}
)
