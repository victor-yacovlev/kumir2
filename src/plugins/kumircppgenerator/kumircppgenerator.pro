TEMPLATE = lib
TARGET = KumirCppGenerator

include(../../kumir2plugin.pri)
include(../../plugins/kumiranalizer/kumiranalizer.pri)

HEADERS += kumircppgeneratorplugin.h

SOURCES += kumircppgeneratorplugin.cpp

OTHER_FILES += KumirCppGenerator.pluginspec
