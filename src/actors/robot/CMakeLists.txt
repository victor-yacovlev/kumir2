set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtDeclarative REQUIRED)
find_package(PythonInterp 2.6 REQUIRED)

include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)


set(SOURCES
    robotmodule.cpp
)

set(MOC_HEADERS
    robotmodule.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/robotmodulebase.cpp ${CMAKE_CURRENT_BINARY_DIR}/robotmodulebase.h ${CMAKE_CURRENT_BINARY_DIR}/robotplugin.cpp ${CMAKE_CURRENT_BINARY_DIR}/robotplugin.h ${CMAKE_CURRENT_BINARY_DIR}/ActorRobot.pluginspec
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py --update ${CMAKE_CURRENT_SOURCE_DIR}/robot.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/robot.json ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(ActorRobotPluginSpec ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/ActorRobot.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorRobot.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/robotmodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/robotmodulebase.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/robotmodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/robotmodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/robotplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/robotplugin.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/robotplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/robotplugin.h
)

set(SOURCES2
    robotmodulebase.cpp
    robotplugin.cpp
)

set(MOC_SOURCES2
    robotmodulebase.moc.cpp
    robotplugin.moc.cpp
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

install(FILES ${PLUGIN_OUTPUT_PATH}/ActorRobot.pluginspec DESTINATION ${PLUGINS_DIR})
handleTranslation(ActorRobot)
add_library(ActorRobot SHARED ${MOC_SOURCES} ${SOURCES} ${MOC_SOURCES2} ${SOURCES2})
target_link_libraries(ActorRobot ${QT_LIBRARIES} ExtensionSystem)
copyResources(actors/robot)
install(TARGETS ActorRobot DESTINATION ${PLUGINS_DIR})

