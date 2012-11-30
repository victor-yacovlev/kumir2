include(../../kumir2_plugin.cmake)

set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

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

