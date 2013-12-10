set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)

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

qt4_wrap_ui(UI_SOURCES ${FORMS})
qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(Editor)
add_library(Editor SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
handleTranslation(Editor)
target_link_libraries(Editor ${QT_LIBRARIES} ${EXTRA_LIBS} ExtensionSystem DataFormats Widgets DocBookViewer  ${STDCXX_LIB} ${STDMATH_LIB})

copyResources(editor)

install(TARGETS Editor DESTINATION ${PLUGINS_DIR})
