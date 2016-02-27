project(ActorIsometricRobot)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets Script Svg REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Script_INCLUDE_DIRS} ${Qt5Svg_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Script_LIBRARIES} ${Qt5Svg_LIBRARIES})
    set(MOC_PARAMS "-I/usr/include/qt5" "-I/usr/include/qt5/QtCore" "-I${_qt5Core_install_prefix}/include/QtCore" "-DQT_PLUGIN")
    foreach(IDIR ${Qt5Core_INCLUDE_DIRS})
        set(MOC_PARAMS "-I${IDIR}" ${MOC_PARAMS})
    endforeach()
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg QtScript REQUIRED)
    include(${QT_USE_FILE})
endif()

find_package(PythonInterp 2.6 REQUIRED)
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
    remotecontrol.cpp
)

set(MOC_HEADERS
    isometricrobotmodule.h
    robotview.h
    robot25dwindow.h
    robotitem.h
    remotecontrol.h
)

set(FORMS
    robot25dwindow.ui
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.h ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.h ${CMAKE_CURRENT_BINARY_DIR}/ActorIsometricRobot.pluginspec
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py --update ${CMAKE_CURRENT_SOURCE_DIR}/isometricrobot.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/isometricrobot.json ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(ActorIsometricRobotPluginSpec ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/ActorIsometricRobot.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorIsometricRobot.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_PARAMS} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotmodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_PARAMS} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/isometricrobotplugin.h
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

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()

install(FILES ${PLUGIN_OUTPUT_PATH}/ActorIsometricRobot.pluginspec DESTINATION ${PLUGINS_DIR})
handleTranslation(ActorIsometricRobot)
add_library(ActorIsometricRobot SHARED ${MOC_SOURCES} ${SOURCES} ${MOC_SOURCES2} ${SOURCES2} ${UI_SOURCES})
target_link_libraries(ActorIsometricRobot ${QT_LIBRARIES} ExtensionSystem Widgets  ${STDCXX_LIB} ${STDMATH_LIB})
copyResources(actors/isometricrobot)
install(TARGETS ActorIsometricRobot DESTINATION ${PLUGINS_DIR})

