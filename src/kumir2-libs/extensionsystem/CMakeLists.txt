project(ExtensionSystem)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)


set(MOC_HEADERS
    pluginmanager.h
    kplugin.h
    commandlineparameter.h
)

set(SOURCES
    logger.cpp
    kplugin.cpp
    pluginspec.cpp
    pluginmanager.cpp
    pluginmanager_impl.cpp
    settings.cpp
    commandlineparameter.cpp
)

set(HEADERS
    kplugin.h
    pluginmanager.h
    switchworkspacedialog.h
    pluginspec.h
    settings.h
    commandlineparameter.h
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_library(
    NAME        ExtensionSystem
    SOURCES     ${SOURCES} ${MOC_SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
