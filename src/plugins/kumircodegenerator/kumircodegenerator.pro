TEMPLATE = lib
TARGET = KumirCodeGenerator

include(../../kumir2plugin.pri)
include(../../plugins/kumiranalizer/kumiranalizer.pri)

HEADERS += kumircodegeneratorplugin.h

SOURCES += kumircodegeneratorplugin.cpp

OTHER_FILES += KumirCodeGenerator.pluginspec
