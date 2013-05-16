set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)


set(SOURCES
    kumircodegeneratorplugin.cpp
    generator.cpp
)

set(MOC_HEADERS
    kumircodegeneratorplugin.h
    generator.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirCodeGenerator)
add_library(KumirCodeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirCodeGenerator)
target_link_libraries(KumirCodeGenerator ${QT_LIBRARIES} ExtensionSystem DataFormats ErrorMessages)

install(TARGETS KumirCodeGenerator DESTINATION ${PLUGINS_DIR})

