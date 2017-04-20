project(ActorKeyboard)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)


set(SOURCES
    keyboardmodule.cpp
)

set(MOC_HEADERS
    keyboardmodule.h
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_actor(
    NAME        Keyboard
    SOURCES     ${SOURCES} ${MOC_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
