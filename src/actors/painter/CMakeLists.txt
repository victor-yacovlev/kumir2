set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
find_package(PythonInterp 2.6 REQUIRED)

include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)

set(SOURCES
    paintermodule.cpp
    painternewimagedialog.cpp
    painterruler.cpp
    paintertools.cpp
    painterview.cpp
    painterwindow.cpp
)

set(MOC_HEADERS
    paintermodule.h
    painternewimagedialog.h
    painterruler.h
    painterview.h
    painterwindow.h
)

set(FORMS
    painternewimagedialog.ui
    painterwindow.ui
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/paintermodulebase.cpp ${CMAKE_CURRENT_BINARY_DIR}/paintermodulebase.h ${CMAKE_CURRENT_BINARY_DIR}/painterplugin.cpp ${CMAKE_CURRENT_BINARY_DIR}/painterplugin.h ${CMAKE_CURRENT_BINARY_DIR}/ActorPainter.pluginspec
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py --update ${CMAKE_CURRENT_SOURCE_DIR}/painter.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/painter.json ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(ActorPainterPluginSpec ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/ActorPainter.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorPainter.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/paintermodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/paintermodulebase.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/paintermodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/paintermodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/painterplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/painterplugin.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/painterplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/painterplugin.h
)

set(SOURCES2
    paintermodulebase.cpp
    painterplugin.cpp
)

set(MOC_SOURCES2
    paintermodulebase.moc.cpp
    painterplugin.moc.cpp
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
qt4_wrap_ui(UI_SOURCES ${FORMS})

install(FILES ${PLUGIN_OUTPUT_PATH}/ActorPainter.pluginspec DESTINATION ${PLUGINS_DIR})
handleTranslation(ActorPainter)
add_library(ActorPainter SHARED ${MOC_SOURCES} ${SOURCES} ${MOC_SOURCES2} ${SOURCES2} ${UI_SOURCES})
target_link_libraries(ActorPainter ${QT_LIBRARIES} ExtensionSystem Widgets ${STDCXX_LIB} ${STDMATH_LIB})
copyResources(actors/painter)
install(TARGETS ActorPainter DESTINATION ${PLUGINS_DIR})
