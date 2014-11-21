project(CoreGUI)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

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
    side.cpp
    switchworkspacedialog.cpp
    systemopenfilesettings.cpp
    guisettingspage.cpp
    toolbarcontextmenu.cpp
    iosettingseditorpage.cpp
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
)

set(FORMS
    mainwindow.ui
    aboutdialog.ui
    switchworkspacedialog.ui
    systemopenfilesettings.ui
    guisettingspage.ui
    iosettingseditorpage.ui
)

set(RESOURCES
    resources.qrc
)

if(${USE_QT} GREATER 4)
    qt5_wrap_ui(UI_SOURCES ${FORMS})
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_add_resources(QRC_SOURCES ${RESOURCES})
else()
    qt4_wrap_ui(UI_SOURCES ${FORMS})
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_add_resources(QRC_SOURCES ${RESOURCES})
endif()
copySpecFile(CoreGUI)
add_library(CoreGUI SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES} ${QRC_SOURCES})
handleTranslation(CoreGUI)
target_link_libraries(CoreGUI ${QT_LIBRARIES} ${EXTRA_LIBS} ExtensionSystem DataFormats Widgets DocBookViewer  ${STDCXX_LIB} ${STDMATH_LIB})

copyResources(coregui)
install(TARGETS CoreGUI DESTINATION ${PLUGINS_DIR})
