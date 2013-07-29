set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)


set(SOURCES
    kumircompilertoolplugin.cpp
)

set(MOC_HEADERS
    kumircompilertoolplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirCompilerTool)
add_library(KumirCompilerTool SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirCompilerTool)
target_link_libraries(KumirCompilerTool ${QT_LIBRARIES} ExtensionSystem DataFormats)

install(TARGETS KumirCompilerTool DESTINATION ${PLUGINS_DIR})
