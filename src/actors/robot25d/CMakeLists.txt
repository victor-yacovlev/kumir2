set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtScript QtSvg REQUIRED)
find_package(PythonInterp 2.6 REQUIRED)

include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)

set(SOURCES
    isometricrobotmodule.cpp
    robotview.cpp
    util.cpp
    sch_task.cpp
    sch_program.cpp
    sch_game.cpp
    sch_environment.cpp
    sch_command.cpp
    sch_algorithm.cpp
    cellgraphicsitem.cpp
    graphicsimageitem.cpp
    robot25dwindow.cpp
    robotitem.cpp
)

set(MOC_HEADERS
    isometricrobotmodule.h
    robotview.h
    robot25dwindow.h
    robotitem.h
)

set(FORMS
    robot25dwindow.ui
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.h ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.h ${CMAKE_CURRENT_BINARY_DIR}/ActorIsometricRobot.pluginspec
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py --update ${CMAKE_CURRENT_SOURCE_DIR}/robot25d.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/robot25d.json ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(ActorIsometricRobotPluginSpec ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/ActorIsometricRobot.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorIsometricRobot.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.h
)

set(SOURCES2
    isometricrobotmodulebase.cpp
    isometricrobotplugin.cpp
)

set(MOC_SOURCES2
    isometricrobotmodulebase.moc.cpp
    isometricrobotplugin.moc.cpp
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
qt4_wrap_ui(UI_SOURCES ${FORMS})

install(FILES ${PLUGIN_OUTPUT_PATH}/ActorIsometricRobot.pluginspec DESTINATION ${PLUGINS_DIR})
handleTranslation(ActorIsometricRobot)
add_library(ActorIsometricRobot SHARED ${MOC_SOURCES} ${SOURCES} ${MOC_SOURCES2} ${SOURCES2} ${UI_SOURCES})
target_link_libraries(ActorIsometricRobot ${QT_LIBRARIES} ExtensionSystem)
copyResources(actors/robot25d)
install(TARGETS ActorIsometricRobot DESTINATION ${PLUGINS_DIR})

