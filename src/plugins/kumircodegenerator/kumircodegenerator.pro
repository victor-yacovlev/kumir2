TEMPLATE = lib
TARGET = KumirCodeGenerator
QT += script
include(../../kumir2plugin.pri)
include(../../plugins/kumiranalizer/kumiranalizer.pri)

HEADERS += kumircodegeneratorplugin.h \
    instruction.h

SOURCES += kumircodegeneratorplugin.cpp \
    instruction.cpp

OTHER_FILES += KumirCodeGenerator.pluginspec
