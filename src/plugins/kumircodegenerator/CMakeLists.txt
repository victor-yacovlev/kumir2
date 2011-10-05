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
handleTranslation(KumirCodeGenerator)
add_library(KumirCodeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(KumirCodeGenerator ${QT_LIBRARIES} ExtensionSystem DataFormats)
install(TARGETS KumirCodeGenerator LIBRARY DESTINATION ${LIB_BASENAME}/kumir2/plugins)
