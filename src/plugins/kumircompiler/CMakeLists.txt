include(../../kumir2_plugin.cmake)

set(SOURCES
    kumircompilerplugin.cpp
)

set(MOC_HEADERS
    kumircompilerplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirCompiler)
handleTranslation(KumirCompiler)
add_library(KumirCompiler SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(KumirCompiler ${QT_LIBRARIES} ExtensionSystem DataFormats)
install(TARGETS KumirCompiler DESTINATION ${PLUGINS_DIR})
