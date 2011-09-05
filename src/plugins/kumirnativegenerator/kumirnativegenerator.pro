TEMPLATE = lib
TARGET = KumirNativeGenerator
QT += script
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/dataformats/dataformats.pri)
include(../../shared/interfaces/generator.pri)



HEADERS += kumirnativegeneratorplugin.h \
    nameprovider.h

SOURCES += kumirnativegeneratorplugin.cpp \
    nameprovider.cpp

contains(CONFIG,clang) {
    LIBS += -lclangFrontendTool -lclangFrontend -lclangDriver \
           -lclangSerialization -lclangCodeGen -lclangParse -lclangSema \
           -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers \
           -lclangStaticAnalyzerCore \
           -lclangAnalysis -lclangIndex -lclangRewrite \
           -lclangAST -lclangLex -lclangBasic

    LIBS += $$system(llvm-config --libs)

    HEADERS += clangcompiler.h
    SOURCES += clangcompiler.cpp
    DEFINES += USE_CLANG

}

OTHER_FILES += KumirNativeGenerator.pluginspec
