include(../../kumir2_plugin.cmake)

set(SOURCES
    editorplugin.cpp
    editor.cpp
    editorplane.cpp
    textcursor.cpp
    clipboard.cpp
    utils.cpp
    editorstandalonewindow.cpp
    settingspage.cpp
    macro.cpp
    keycommand.cpp
    textdocument.cpp
    editcommands.cpp
    suggestionswindow.cpp
    findreplace.cpp
)

set(EXTRA_LIBS
)

if(UNIX AND NOT APPLE)
    find_package(X11 REQUIRED)
    set(EXTRA_LIBS ${X11_LIBRARIES})
endif()

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
    editorstandalonewindow.h
    settingspage.h
    editor_p.h
    suggestionswindow.h
    findreplace.h
)

set(FORMS
    settingspage.ui
    suggestionswindow.ui
    findreplace.ui
)

qt4_wrap_ui(UI_SOURCES ${FORMS})
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(Editor)
add_library(Editor SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
handleTranslation(Editor)
target_link_libraries(Editor ${QT_LIBRARIES} ${EXTRA_LIBS} ExtensionSystem DataFormats Widgets)

copyResources(editor)

install(TARGETS Editor DESTINATION ${PLUGINS_DIR})
