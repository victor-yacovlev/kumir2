project(ActorDraw)
cmake_minimum_required(VERSION 3.0)


find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui Xml Svg)

set(SOURCES
    drawmodule.cpp
)

set(MOC_HEADERS
    drawmodule.h
)


kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_actor(
    NAME        Draw
    SOURCES     ${SOURCES} ${MOC_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)

add_dependencies(ActorDraw Actor_Colorer)
