TEMPLATE = lib
TARGET = PascalAnalizer

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)

HEADERS += \
    pasplugin.h \
    ippc.h

OTHER_FILES = \
    PascalAnalizer.pluginspec \

#LIBS += -L/home/victor/work/kumirfreepascal/compiler -lfreepascal

SOURCES += \
    pasplugin.cpp \
    ippc.cpp







