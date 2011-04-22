TEMPLATE = lib
TARGET = KumirCompiler

include(../../kumir2plugin.pri)
include(../../plugins/kumiranalizer/kumiranalizer.pri)

HEADERS += kumircompilerplugin.h

SOURCES += kumircompilerplugin.cpp

OTHER_FILES += KumirCompiler.pluginspec
