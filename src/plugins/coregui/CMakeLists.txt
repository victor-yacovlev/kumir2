set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)

set(SOURCES
    mainwindow.cpp
    plugin.cpp
    tabwidget.cpp
    tabwidgetelement.cpp
    kumirprogram.cpp
    terminal.cpp
    terminal_onesession.cpp
    terminal_plane.cpp
    tabbar.cpp
    aboutdialog.cpp
    statusbar.cpp
    debuggerview.cpp
    row.cpp
    switchworkspacedialog.cpp
    systemopenfilesettings.cpp
)

set(EXTRA_LIBS
)

if(APPLE)
    set(SOURCES ${SOURCES}
        mac-fixes.mm
    )
    find_library(COCOA_LIBRARY Cocoa)
    set(EXTRA_LIBS ${COCOA_LIBRARY})
endif(APPLE)

set(MOC_HEADERS
    mainwindow.h
    plugin.h
    tabwidget.h
    kumirprogram.h
    terminal.h
    terminal_onesession.h
    terminal_plane.h
    tabbar.h
    aboutdialog.h
    tabwidgetelement.h
    statusbar.h
    debuggerview.h
    row.h
    switchworkspacedialog.h
    systemopenfilesettings.h
)

set(FORMS
    mainwindow.ui
    aboutdialog.ui
    switchworkspacedialog.ui
    systemopenfilesettings.ui
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
target_link_libraries(CoreGUI ${QT_LIBRARIES} ${EXTRA_LIBS} ExtensionSystem DataFormats Widgets DocBookViewer  ${STDCXX_LIB} ${STDMATH_LIB})

copyResources(coregui)
install(TARGETS CoreGUI DESTINATION ${PLUGINS_DIR})
