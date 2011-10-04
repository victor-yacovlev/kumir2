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

PPCNAME = ppc386
linux-* {
    ARCH = $$system(arch)
    contains(ARCH,x86_64): PPCNAME = ppcx64
    MAKE = make
    FPCMAKE = fpcmake
}

macx-* {
    MAKE = make
    FPCMAKE = fpcmake
}

FPCNAME = PPCNAME
IPPCBIN = ippc

win32 {
    FPCMAKE = fpcmake.exe
    PPCNAME = ppc386.exe
    MAKE = mingw32-make
    FPCNAME = fpc
    IPPCBIN = ippc.exe
}

exists($$IDE_BUILD_TREE/libexec/kumir2) {
    MKDIR = echo Directory /libexec/kumir2 already exists
}
else {
    MKDIR = $$QMAKE_MKDIR $$targetPath($$IDE_BUILD_TREE/libexec/kumir2/)
}

ippcbin.target = $$IDE_BUILD_TREE/libexec/kumir2/ippc

ippcbin.commands = \
    cd $${_PRO_FILE_PWD_}/ippc/rtl && \
    $$FPCMAKE && \
    cd $${_PRO_FILE_PWD_}/ippc/compiler && \
    $$FPCMAKE && \
    cd $${_PRO_FILE_PWD_}/ippc/ && \
    $$MAKE rtl FPC=$$FPCNAME && \
    $$MAKE compiler FPC=$$FPCNAME && \
    $$MKDIR && \
    $$QMAKE_COPY $$targetPath($${_PRO_FILE_PWD_}/ippc/compiler/$$PPCNAME) $$targetPath($$IDE_BUILD_TREE/libexec/kumir2/$$IPPCBIN)

ippcbin.files = $$IDE_BUILD_TREE/libexec/kumir2/ippc
ippcbin.path = $$INSTALL_ROOT/libexec/kumir2




QMAKE_EXTRA_TARGETS += ippcbin
PRE_TARGETDEPS += $$IDE_BUILD_TREE/libexec/kumir2/ippc
INSTALLS += ippcbin




