project(ActorVodoley)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)


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

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

kumir2_add_actor(
    NAME        Vodoley
    SOURCES     ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)

