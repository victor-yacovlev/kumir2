project(Editor)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

if(${USE_QT} GREATER 4 AND UNIX AND NOT APPLE)
    find_package(Qt5 5.3.0 COMPONENTS X11Extras REQUIRED)
    include_directories(${Qt5X11Extras_INCLUDE_DIRS})
    list(APPEND QT_LIBRARIES ${Qt5X11Extras_LIBRARIES})
endif()

set(EXTRA_LIBS)

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    set(EXTRA_LIBS ${X11_LIBRARIES})
endif()

add_definitions(-DQT_NONLATIN_SHORTCUTS_BUG)

set(SOURCES
    editorplugin.cpp
    editor.cpp
    editorplane.cpp
    textcursor.cpp
    clipboard.cpp
    utils.cpp
    settingspage.cpp
    macro.cpp
    keycommand.cpp
    textdocument.cpp
    editcommands.cpp
    suggestionswindow.cpp
    findreplace.cpp
    macroeditor.cpp
    macrolisteditor.cpp
)

if(APPLE)
    set(SOURCES ${SOURCES}
        mac-util.mm
    )
    find_library(COCOA_LIBRARY Cocoa)
    set(EXTRA_LIBS ${COCOA_LIBRARY})
endif(APPLE)

set(MOC_HEADERS
    editorplugin.h
    editor.h
    editorplane.h
    textcursor.h
    textdocument.h
    clipboard.h
    settingspage.h
    suggestionswindow.h
    findreplace.h
    macroeditor.h
    macrolisteditor.h
)

set(FORMS
    settingspage.ui
    suggestionswindow.ui
    findreplace.ui
    macroeditor.ui
    macrolisteditor.ui
)

kumir2_wrap_ui(UI_SOURCES ${FORMS})
kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_plugin(
    NAME        Editor
    SOURCES     ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES} ${EXTRA_LIBS} ExtensionSystem DataFormats Widgets DocBookViewer
)
