TEMPLATE = lib
TARGET = PascalAnalizer

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)

HEADERS += \
    pasplugin.h

OTHER_FILES = \
    PascalAnalizer.pluginspec \

LIBS += -lfreepascal

SOURCES += \
    pasplugin.cpp


