project(KumirCodeRun)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_use_qt(Core Gui)

set(SOURCES
    runplugin.cpp
    run.cpp
    consolerun.cpp
    commonrun.cpp
    util.cpp
    guirun.cpp
    kumvariablesmodel.cpp
)

set(HEADERS
    ../../shared/vm/vm.hpp
)

set(MOC_HEADERS
    runplugin.h
    run.h
    commonrun.h
    guirun.h
    kumvariablesmodel.h
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_plugin(
    NAME        KumirCodeRun
    SOURCES     ${MOC_SOURCES} ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ExtensionSystem
)
