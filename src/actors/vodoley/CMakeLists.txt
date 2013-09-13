set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
find_package(PythonInterp 2.6 REQUIRED)
include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)

set(SOURCES
    vodoleymodule.cpp
)

set(MOC_HEADERS
    vodoleymodule.h
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
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vodoleymodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vodoleyplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
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

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

install(
    FILES ${PLUGIN_OUTPUT_PATH}/ActorVodoley.pluginspec
    DESTINATION ${PLUGINS_DIR}
)

handleTranslation(ActorVodoley)

add_library(
    ActorVodoley
    SHARED
    ${MOC_SOURCES} ${SOURCES}
    ${MOC_SOURCES_GENERATED} ${SOURCES_GENERATED}
)

target_link_libraries(
    ActorVodoley
    ${QT_LIBRARIES}
    ExtensionSystem
    Widgets
)

copyResources(actors/vodoley)

install(
    TARGETS ActorVodoley
    DESTINATION ${PLUGINS_DIR}
)
