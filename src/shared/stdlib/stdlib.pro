TEMPLATE = lib
TARGET = KumirStdLib
HEADERS = \
    kumstdlib.h genericinputoutput.h
SOURCES = \
    kumstdlib.cpp genericinputoutput.cpp
include(../../kumir2library.pri)
include(../../rpath.pri)
QT -= gui
