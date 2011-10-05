include(../../kumir2_plugin.cmake)

set(SOURCES
    painterplugin.cpp
)

set(MOC_HEADERS
    painterplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(ActorPainter)
handleTranslation(ActorPainter)
add_library(ActorPainter SHARED ${MOC_SOURCES} ${SOURCES})
target_link_libraries(ActorPainter ${QT_LIBRARIES} ExtensionSystem ActorPainterC KumirStdLib)

install(TARGETS ActorPainter DESTINATION ${PLUGINS_DIR})
