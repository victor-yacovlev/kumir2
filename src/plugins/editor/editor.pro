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
    editorstandalonewindow.cpp \
    settingspage.cpp \
    macro.cpp \
    keycommand.cpp

OTHER_FILES += Editor.pluginspec

FORMS += \
    settingspage.ui

linux-* {
    LIBS += -lX11
}
