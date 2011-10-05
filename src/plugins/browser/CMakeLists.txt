include(../../kumir2_plugin.cmake)


set(SOURCES
    component.cpp
    plugin.cpp
    webpage.cpp
    dir.cpp
    localhostserver.cpp
    networkaccessmanager.cpp
)

set(MOC_HEADERS
    component.h
    plugin.h
    webpage.h
    dir.h
    localhostserver.h
    networkaccessmanager.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(Browser)
handleTranslation(Browser)
add_library(Browser SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(Browser ${QT_LIBRARIES} ExtensionSystem)

install(TARGETS Browser DESTINATION ${PLUGINS_DIR})
