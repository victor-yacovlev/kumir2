project(KumirCompilerTool)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core)

set(SOURCES
    kumircompilertoolplugin.cpp
)

set(MOC_HEADERS
    kumircompilertoolplugin.h
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_plugin(
    NAME        KumirCompilerTool
    SOURCES     ${MOC_SOURCES} ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ExtensionSystem DataFormats
)
