include(../../kumir2_plugin.cmake)

set(MOC_HEADERS
    st_functplugin.h
)

set(SOURCES
    st_functplugin.cpp
)



qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(st_funct)
handleTranslation(st_funct)
add_library(st_funct SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(st_funct ${QT_LIBRARIES} ExtensionSystem KumirStdLib)

install(TARGETS st_funct DESTINATION ${PLUGINS_DIR})
