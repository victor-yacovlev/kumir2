set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtSvg REQUIRED)
find_package(PythonInterp 2.6 REQUIRED)
include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)

set(SOURCES
    grasshoppermodule.cpp
    kuznec.cpp
    pult.cpp
    dialog.cpp
)

set(MOC_HEADERS
    grasshoppermodule.h
    kuznec.h
    dialog.h
    pult.h
)
set(FORMS
    dialog.ui
    pult.ui
    )
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/grasshoppermodulebase.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/grasshoppermodulebase.h
        ${CMAKE_CURRENT_BINARY_DIR}/grasshopperplugin.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/grasshopperplugin.h
        ${CMAKE_CURRENT_BINARY_DIR}/ActorGrasshopper.pluginspec
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
            --update
            ${CMAKE_CURRENT_SOURCE_DIR}/grasshopper.json
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/grasshopper.json
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(
    ActorGrasshopperPluginSpec
    ALL
    ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/ActorGrasshopper.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorGrasshopper.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/grasshoppermodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/grasshoppermodulebase.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/grasshoppermodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/grasshoppermodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/grasshopperplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o${CMAKE_CURRENT_BINARY_DIR}/grasshopperplugin.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/grasshopperplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/grasshopperplugin.h
)

set(SOURCES_GENERATED
    grasshoppermodulebase.cpp
    grasshopperplugin.cpp
)

set(MOC_SOURCES_GENERATED
    grasshoppermodulebase.moc.cpp
    grasshopperplugin.moc.cpp
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
qt4_wrap_ui(UI_SOURCES ${FORMS})
install(
    FILES ${PLUGIN_OUTPUT_PATH}/ActorGrasshopper.pluginspec
    DESTINATION ${PLUGINS_DIR}
)

handleTranslation(ActorGrasshopper)

add_library(
    ActorGrasshopper
    SHARED
    ${MOC_SOURCES} ${SOURCES} ${UI_SOURCES}
    ${MOC_SOURCES_GENERATED} ${SOURCES_GENERATED}
)

target_link_libraries(
    ActorGrasshopper
    ${QT_LIBRARIES}
    ExtensionSystem
    Widgets
    ${STDCXX_LIB} ${STDMATH_LIB}
)

copyResources(actors/grasshopper)

install(
    TARGETS ActorGrasshopper
    DESTINATION ${PLUGINS_DIR}
)
