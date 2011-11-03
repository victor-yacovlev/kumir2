include(../../kumir2_plugin.cmake)

set(SOURCES
    mainwindow.cpp
    plugin.cpp
    tabwidget.cpp
    kumirprogram.cpp
    terminal.cpp
    terminal_onesession.cpp
    terminal_plane.cpp
    tabbar.cpp
    kumirvariableswebobject.cpp
    aboutdialog.cpp
    pascalprogram.cpp
)


set(MOC_HEADERS
    mainwindow.h
    plugin.h
    tabwidget.h
    kumirprogram.h
    terminal.h
    terminal_onesession.h
    terminal_plane.h
    tabbar.h
    kumirvariableswebobject.h
    aboutdialog.h
    pascalprogram.h
    tabwidgetelement.h
)

set(FORMS
    mainwindow.ui
    aboutdialog.ui
)

set(RESOURCES
    resources.qrc
)

qt4_wrap_ui(UI_SOURCES ${FORMS})
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
copySpecFile(CoreGUI)
add_library(CoreGUI SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES} ${QRC_SOURCES})
handleTranslation(CoreGUI)
target_link_libraries(CoreGUI ${QT_LIBRARIES} ExtensionSystem DataFormats KumirStdLib)

copyResources(coregui)
install(TARGETS CoreGUI DESTINATION ${PLUGINS_DIR})
