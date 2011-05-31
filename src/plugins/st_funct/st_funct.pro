TEMPLATE = lib
CONFIG += plugin
TARGET = st_funct

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)


HEADERS += st_functplugin.h \  
    genericinputoutput.h

SOURCES += st_functplugin.cpp \  
    genericinputoutput.cpp

OTHER_FILES += st_funct.pluginspec
