TEMPLATE = lib
TARGET = ExtensionSystem
DEFINES += EXTENSIONSYSTEM_LIBRARY
include(../../kumir2library.pri)
QT += script

unix:!macx:!freebsd*:LIBS += -ldl

DEFINES += IDE_TEST_DIR=\\\"$$IDE_SOURCE_TREE\\\"

HEADERS += \
    kplugin.h \
    pluginspec.h \
    pluginmanager.h

SOURCES += \
    kplugin.cpp \
    pluginspec.cpp \
    pluginmanager.cpp
