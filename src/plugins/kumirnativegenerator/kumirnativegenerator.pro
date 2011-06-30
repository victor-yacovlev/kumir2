TEMPLATE = lib
TARGET = KumirNativeGenerator
QT += script
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/abstractsyntaxtree/abstractsyntaxtree.pri)
include(../../shared/interfaces/generator.pri)

HEADERS += kumirnativegeneratorplugin.h \
    nameprovider.h

SOURCES += kumirnativegeneratorplugin.cpp \
    nameprovider.cpp

OTHER_FILES += KumirNativeGenerator.pluginspec
