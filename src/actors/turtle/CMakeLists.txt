project(ActorTurtle)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui Svg)

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


kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

kumir2_add_actor(
    NAME        Turtle
    SOURCES     ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
