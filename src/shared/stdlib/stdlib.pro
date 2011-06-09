TEMPLATE = lib
TARGET = KumirStdLib
DEFINES += STDLIB_LIBRARY
HEADERS = \
    kumstdlib.h genericinputoutput.h connector.h
SOURCES = \
    kumstdlib.cpp genericinputoutput.cpp connector.cpp
include(../../kumir2library.pri)
include(../../rpath.pri)
QT -= gui
