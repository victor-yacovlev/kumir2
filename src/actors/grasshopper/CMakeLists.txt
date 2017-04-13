project(ActorGrasshopper)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui Svg)


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

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

kumir2_add_actor(
    NAME        Grasshopper
    SOURCES     ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
