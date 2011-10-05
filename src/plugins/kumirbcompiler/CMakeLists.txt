include(../../kumir2_plugin.cmake)

set(SOURCES
    kumirbcompilerplugin.cpp
)

set(MOC_HEADERS
    kumirbcompilerplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirBCompiler)
handleTranslation(KumirBCompiler)
add_library(KumirBCompiler SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(KumirBCompiler ${QT_LIBRARIES} ExtensionSystem DataFormats)

install(TARGETS KumirBCompiler DESTINATION ${PLUGINS_DIR})
