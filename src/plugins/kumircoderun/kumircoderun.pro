TEMPLATE = lib
TARGET = KumirCodeRun

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/dataformats/dataformats.pri)

#win32 {
#    LIBS += -L../../../bin -lKumirStdLib
#}

include(../../shared/stdlib/stdlib.pri)



OTHER_FILES += KumirCodeRun.pluginspec

HEADERS += \
    plugin.h \
    run.h \
    vm.h \
    variant.h \
    context.h

SOURCES += \
    plugin.cpp \
    run.cpp \
    vm.cpp \
    variant.cpp \
    context.cpp
