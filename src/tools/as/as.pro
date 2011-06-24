TEMPLATE = app
TARGET = kumir2-as
CONFIG += console
QT = core
include(../../shared/bytecode/bytecode.pri)
include(../../../kumir2.pri)
include(../../rpath.pri)
INCLUDEPATH += ../../
DESTDIR = $$IDE_APP_PATH
SOURCES = main.cpp
