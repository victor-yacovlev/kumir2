project(ActorTurtle)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets Svg REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Svg_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Svg_LIBRARIES})
    set(MOC_PARAMS "-I/usr/include/qt5" "-I/usr/include/qt5/QtCore" "-I${_qt5Core_install_prefix}/include/QtCore" "-DQT_PLUGIN")
    foreach(IDIR ${Qt5Core_INCLUDE_DIRS})
        set(MOC_PARAMS "-I${IDIR}" ${MOC_PARAMS})
    endforeach()
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

find_package(PythonInterp 2.6 REQUIRED)
include(../../kumir2_plugin.cmake)

set(SOURCES
    turtlemodule.cpp
    turtle.cpp
    pult.cpp

)
set(FORMS
    pult.ui
    )

set(MOC_HEADERS
    turtlemodule.h
    turtle.h
    pult.h
)
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/turtlemodulebase.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/turtlemodulebase.h
        ${CMAKE_CURRENT_BINARY_DIR}/turtleplugin.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/turtleplugin.h
        ${CMAKE_CURRENT_BINARY_DIR}/ActorTurtle.pluginspec
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
            --update
            ${CMAKE_CURRENT_SOURCE_DIR}/turtle.json
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/turtle.json
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(
    ActorTurtlePluginSpec
    ALL
    ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/ActorTurtle.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorTurtle.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/turtlemodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        ${MOC_PARAMS}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/turtlemodulebase.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/turtlemodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/turtlemodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/turtleplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        ${MOC_PARAMS}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/turtleplugin.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/turtleplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/turtleplugin.h
)

set(SOURCES_GENERATED
    turtlemodulebase.cpp
    turtleplugin.cpp
)

set(MOC_SOURCES_GENERATED
    turtlemodulebase.moc.cpp
    turtleplugin.moc.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()

install(
    FILES ${PLUGIN_OUTPUT_PATH}/ActorTurtle.pluginspec
    DESTINATION ${PLUGINS_DIR}
)

handleTranslation(ActorTurtle)

add_library(
    ActorTurtle
    SHARED
    ${MOC_SOURCES} ${SOURCES} ${UI_SOURCES}
    ${MOC_SOURCES_GENERATED} ${SOURCES_GENERATED}
)

target_link_libraries(
    ActorTurtle
    ${QT_LIBRARIES}
    ExtensionSystem
    Widgets
    ${STDCXX_LIB} ${STDMATH_LIB}
)

copyResources(actors/turtle)

install(
    TARGETS ActorTurtle
    DESTINATION ${PLUGINS_DIR}
)
