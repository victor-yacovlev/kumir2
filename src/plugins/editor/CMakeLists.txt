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
    autocompletewidget.cpp
)

set(EXTRA_LIBS
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
    editorstandalonewindow.h
    settingspage.h
    autocompletewidget.h
    editor_p.h
)

set(FORMS
    settingspage.ui
)

qt4_wrap_ui(UI_SOURCES ${FORMS})
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(Editor)
add_library(Editor SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
handleTranslation(Editor)
target_link_libraries(Editor ${QT_LIBRARIES} ${EXTRA_LIBS} ExtensionSystem DataFormats)

copyResources(editor)

install(TARGETS Editor DESTINATION ${PLUGINS_DIR})
