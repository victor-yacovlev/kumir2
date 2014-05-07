project(ActorVodoley)
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
    include (${QT_USE_FILE})
endif()

find_package(PythonInterp 2.6 REQUIRED)
include(../../kumir2_plugin.cmake)
set(SOURCES
    vodoleymodule.cpp
    vodoley.cpp
    pult.cpp
    dialog.cpp
)

set(MOC_HEADERS
    vodoleymodule.h
    vodoley.h
    pult.h
    dialog.h
)

set(FORMS
    dialog.ui
    pult.ui
    )

       
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.h
        ${CMAKE_CURRENT_BINARY_DIR}/vodoleyplugin.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/vodoleyplugin.h
        ${CMAKE_CURRENT_BINARY_DIR}/ActorVodoley.pluginspec
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
            --update
            ${CMAKE_CURRENT_SOURCE_DIR}/vodoley.json
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/vodoley.json
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(
    ActorVodoleyPluginSpec
    ALL
    ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/ActorVodoley.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorVodoley.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        ${MOC_PARAMS}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vodoleyplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        ${MOC_PARAMS}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/vodoleyplugin.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/vodoleyplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vodoleyplugin.h
)

set(SOURCES_GENERATED
    vodoleymodulebase.cpp
    vodoleyplugin.cpp
)

set(MOC_SOURCES_GENERATED
    vodoleymodulebase.moc.cpp
    vodoleyplugin.moc.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()

install(
    FILES ${PLUGIN_OUTPUT_PATH}/ActorVodoley.pluginspec
    DESTINATION ${PLUGINS_DIR}
)

handleTranslation(ActorVodoley)

add_library(
    ActorVodoley
    SHARED
    ${MOC_SOURCES} ${SOURCES} ${UI_SOURCES}
    ${MOC_SOURCES_GENERATED} ${SOURCES_GENERATED}
)

target_link_libraries(
    ActorVodoley
    ${QT_LIBRARIES}
    ExtensionSystem
    Widgets
    ${STDCXX_LIB} ${STDMATH_LIB}
)

copyResources(actors/vodoley)

install(
    TARGETS ActorVodoley
    DESTINATION ${PLUGINS_DIR}
)
