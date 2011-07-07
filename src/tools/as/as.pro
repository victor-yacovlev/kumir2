TEMPLATE = app
TARGET = kumir2-as
CONFIG += console
QT = core
include(../../../kumir2.pri)
include(../../rpath.pri)
INCLUDEPATH += ../../
#!macx:LIBS += -L$$IDE_LIBRARY_PATH -lBytecode
macx {
  #  release:LIBS += -L$$IDE_LIBRARY_PATH -lBytecode
    debug:LIBS += -L$$IDE_LIBRARY_PATH -lBytecode_debug
}
DESTDIR = $$IDE_APP_PATH
SOURCES = main.cpp
