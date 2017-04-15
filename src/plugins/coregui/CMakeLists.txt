project(CoreGUI)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)

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
    side.cpp
    switchworkspacedialog.cpp
    systemopenfilesettings.cpp
    guisettingspage.cpp
    toolbarcontextmenu.cpp
    iosettingseditorpage.cpp
    defaultstartpage.cpp
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
    side.h
    switchworkspacedialog.h
    systemopenfilesettings.h
    guisettingspage.h
    toolbarcontextmenu.h
    iosettingseditorpage.h
    defaultstartpage.h
)

set(FORMS
    mainwindow.ui
    aboutdialog.ui
    switchworkspacedialog.ui
    systemopenfilesettings.ui
    guisettingspage.ui
    iosettingseditorpage.ui
    defaultstartpage.ui
)

set(RESOURCES
    resources.qrc
)

kumir2_wrap_ui(UI_SOURCES ${FORMS})
kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
kumir2_add_resources(QRC_SOURCES ${RESOURCES})

kumir2_add_plugin(
    NAME        CoreGUI
    SOURCES     ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES} ${QRC_SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ${EXTRA_LIBS} ExtensionSystem DataFormats Widgets DocBookViewer
)
