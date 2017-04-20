project(KumirCoreGenerator)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_use_qt(Core)

set(SOURCES
    kumircodegeneratorplugin.cpp
    generator.cpp
)

set(MOC_HEADERS
    kumircodegeneratorplugin.h
    generator.h
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_plugin(
    NAME        KumirCodeGenerator
    SOURCES     ${MOC_SOURCES} ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ExtensionSystem DataFormats ErrorMessages
)
