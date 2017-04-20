project(ActorIsometricRobot)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui Svg Script Xml)

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
    robotmodel.cpp
)

set(MOC_HEADERS
    isometricrobotmodule.h
    robotview.h
    robot25dwindow.h
    robotitem.h
    remotecontrol.h
    robotmodel.h
)

set(FORMS
    robot25dwindow.ui
)


kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

kumir2_add_actor(
    NAME        IsometricRobot
    SOURCES     ${SOURCES} ${MOC_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
