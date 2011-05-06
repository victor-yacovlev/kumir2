TEMPLATE = lib
CONFIG += plugin
TARGET = Editor

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/errormessages/errormessages.pri)

QT += gui

HEADERS += editorplugin.h \
    editor.h \
    editorplane.h \
    textcursor.h \
    textdocument.h
SOURCES += editorplugin.cpp \
    editor.cpp \
    editorplane.cpp \
    textcursor.cpp

OTHER_FILES += Editor.pluginspec
