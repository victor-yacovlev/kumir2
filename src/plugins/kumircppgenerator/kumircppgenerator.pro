TEMPLATE = lib
TARGET = KumirCppGenerator
QT += script
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/abstractsyntaxtree/abstractsyntaxtree.pri)
include(../../shared/interfaces/generator.pri)

HEADERS += kumircppgeneratorplugin.h \
    nameprovider.h

SOURCES += kumircppgeneratorplugin.cpp \
    nameprovider.cpp

OTHER_FILES += KumirCppGenerator.pluginspec
