TEMPLATE = lib
TARGET = KumirCppGenerator
QT += script
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/abstractsyntaxtree/abstractsyntaxtree.pri)
include(../../shared/interfaces/generator.pri)

HEADERS += kumircppgeneratorplugin.h

SOURCES += kumircppgeneratorplugin.cpp

OTHER_FILES += KumirCppGenerator.pluginspec
