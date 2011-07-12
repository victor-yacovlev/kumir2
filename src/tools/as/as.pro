
TEMPLATE = app
TARGET = kumir2-as
include(../../../kumir2.pri)
include(../../rpath.pri)

DESTDIR = $$IDE_APP_PATH
macx{
DESTDIR = $$IDE_APP_PATH/Kumir.app/Contents/MacOS/
}
CONFIG += console
macx: CONFIG -= app_bundle
QT -= gui

INCLUDEPATH += ../../

macx {
    debug:LIBS += -L$$IDE_LIBRARY_PATH -lBytecode_debug
} else {
    LIBS += -L$$IDE_LIBRARY_PATH -lBytecode
}

SOURCES = main.cpp
