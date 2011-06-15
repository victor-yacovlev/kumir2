TEMPLATE = lib
TARGET = KumirStdLib
DEFINES += STDLIB_LIBRARY
HEADERS = \
    kumstdlib.h genericinputoutput.h connector.h guirunner.h actorwindow.h userprogramthread.h
SOURCES = \
    kumstdlib.cpp genericinputoutput.cpp connector.cpp guirunner.cpp actorwindow.cpp userprogramthread.cpp
include(../../kumir2library.pri)
include(../../rpath.pri)
QT += gui