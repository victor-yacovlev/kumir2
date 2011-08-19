TEMPLATE = lib
TARGET = KumirCompiler

include(../../kumir2plugin.pri)
include(../../shared/interfaces/analizer.pri)
include(../../shared/interfaces/generator.pri)
include(../../shared/dataformats/dataformats.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
QT += script
HEADERS += kumircompilerplugin.h

SOURCES += kumircompilerplugin.cpp

OTHER_FILES += KumirCompiler.pluginspec
