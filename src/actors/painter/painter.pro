TEMPLATE = lib
TARGET = ActorPainter

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/stdlib/stdlib.pri)
include(painter_c.pri)



OTHER_FILES += ActorPainter.pluginspec

HEADERS += \
    painterplugin.h

SOURCES += \
    painterplugin.cpp
