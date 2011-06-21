TEMPLATE = lib
CONFIG += plugin
TARGET = Editor
QT += script
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/errormessages/errormessages.pri)

QT += gui

HEADERS += editorplugin.h \
    editor.h \
    editorplane.h \
    textcursor.h \
    textdocument.h \
    clipboard.h \
    utils.h \
    statusbar.h \
    editorstandalonewindow.h \
    settingspage.h \
    macro.h \
    keycommand.h
SOURCES += editorplugin.cpp \
    editor.cpp \
    editorplane.cpp \
    textcursor.cpp \
    clipboard.cpp \
    utils.cpp \
    statusbar.cpp \
    editorstandalonewindow.cpp \
    settingspage.cpp \
    macro.cpp \
    keycommand.cpp

OTHER_FILES += Editor.pluginspec

FORMS += \
    statusbar.ui \
    settingspage.ui
