TEMPLATE = lib
TARGET = KumirCodeGenerator
QT += script
include(../../kumir2plugin.pri)
include(../../plugins/kumiranalizer/kumiranalizer.pri)
include(../../shared/errormessages/errormessages.pri)
include(../../shared/dataformats/dataformats.pri)

HEADERS += kumircodegeneratorplugin.h \
    generator.h

SOURCES += kumircodegeneratorplugin.cpp \
    generator.cpp

OTHER_FILES += KumirCodeGenerator.pluginspec
