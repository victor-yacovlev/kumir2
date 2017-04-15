project(Actor_Colorer)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)


set(SOURCES _colorermodule.cpp)
set(MOC_HEADERS _colorermodule.h)


kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_add_actor(
    NAME        _Colorer
    SOURCES     ${SOURCES} ${MOC_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
