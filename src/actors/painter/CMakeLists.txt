project(ActorPainter)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)

set(SOURCES
    paintermodule.cpp
    painternewimagedialog.cpp
    painterruler.cpp
    paintertools.cpp
    painterview.cpp
    painterwindow.cpp
    scrollarea.cpp
)

set(MOC_HEADERS
    paintermodule.h
    painternewimagedialog.h
    painterruler.h
    painterview.h
    painterwindow.h
    scrollarea.h
)

set(FORMS
    painternewimagedialog.ui
    painterwindow.ui
)


kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_wrap_ui(UI_SOURCES ${FORMS})

kumir2_add_actor(
    NAME        Painter
    SOURCES     ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
add_dependencies(ActorPainter Actor_Colorer)

