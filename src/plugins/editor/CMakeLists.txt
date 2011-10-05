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
handleTranslation(Editor)
add_library(Editor SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
target_link_libraries(Editor ${QT_LIBRARIES} ExtensionSystem DataFormats)

copyResources(editor)

install(TARGETS Editor DESTINATION ${PLUGINS_DIR})
