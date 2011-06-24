TEMPLATE = lib
TARGET = KumirBCompiler

include(../../kumir2plugin.pri)
include(../../shared/interfaces/analizer.pri)
include(../../shared/interfaces/generator.pri)
include(../../shared/abstractsyntaxtree/abstractsyntaxtree.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/bytecode/bytecode.pri)
QT += script
HEADERS += kumirbcompilerplugin.h

SOURCES += kumirbcompilerplugin.cpp

OTHER_FILES += KumirBCompiler.pluginspec
