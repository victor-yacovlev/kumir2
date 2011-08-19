TEMPLATE = lib
TARGET = KumirBCompiler

include(../../kumir2plugin.pri)
include(../../shared/interfaces/analizer.pri)
include(../../shared/interfaces/generator.pri)
include(../../shared/dataformats/dataformats.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
HEADERS += kumirbcompilerplugin.h

SOURCES += kumirbcompilerplugin.cpp

OTHER_FILES += KumirBCompiler.pluginspec
