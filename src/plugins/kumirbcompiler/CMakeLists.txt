set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)


set(SOURCES
    kumirbcompilerplugin.cpp
)

set(MOC_HEADERS
    kumirbcompilerplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirBCompiler)
add_library(KumirBCompiler SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirBCompiler)
target_link_libraries(KumirBCompiler ${QT_LIBRARIES} ExtensionSystem DataFormats)

install(TARGETS KumirBCompiler DESTINATION ${PLUGINS_DIR})
