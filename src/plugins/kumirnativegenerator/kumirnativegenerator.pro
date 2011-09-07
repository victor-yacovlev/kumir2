TEMPLATE = lib
TARGET = KumirNativeGenerator
QT += script
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/dataformats/dataformats.pri)
include(../../shared/interfaces/generator.pri)

win32:CONFIG += llvm

HEADERS += kumirnativegeneratorplugin.h \
    nameprovider.h \
    compilerbackend.h

SOURCES += kumirnativegeneratorplugin.cpp \
    nameprovider.cpp

contains(CONFIG,llvm) {
#    LIBS += $$system(llvm-config --libs)
    HEADERS += llvmbackend.h
    SOURCES += llvmbackend.cpp
    DEFINES += USE_LLVM
} else {
    HEADERS += gccbackend.h
    SOURCES += gccbackend.cpp
    DEFINES += USE_GCC
}

OTHER_FILES += KumirNativeGenerator.pluginspec






