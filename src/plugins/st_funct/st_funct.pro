TEMPLATE = lib
CONFIG += plugin
TARGET = st_funct

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/stdlib/stdlib.pri)

HEADERS += st_functplugin.h

SOURCES += st_functplugin.cpp

OTHER_FILES += st_funct.pluginspec
