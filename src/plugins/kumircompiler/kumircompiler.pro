TEMPLATE = lib
TARGET = KumirCompiler

include(../../kumir2plugin.pri)
include(../../shared/interfaces/analizer.pri)
include(../../shared/interfaces/generator.pri)
include(../../shared/abstractsyntaxtree/abstractsyntaxtree.pri)

HEADERS += kumircompilerplugin.h

SOURCES += kumircompilerplugin.cpp

OTHER_FILES += KumirCompiler.pluginspec
