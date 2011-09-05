

TEMPLATE = lib
TARGET = KumirStdLib
DEFINES += STDLIB_LIBRARY
include(../../kumir2library.pri)
include(../../rpath.pri)
HEADERS = \
    kumstdlib.h genericinputoutput.h connector.h guirunner.h actorwindow.h userprogramthread.h \
    integeroverflowchecker.h \
    doubleoverflowchecker.h

SOURCES = \
    kumstdlib.cpp genericinputoutput.cpp connector.cpp guirunner.cpp actorwindow.cpp userprogramthread.cpp \
    integeroverflowchecker.cpp \
    doubleoverflowchecker.cpp

QT += gui

!macx {
    message("Using shared memory interprocess method for connector")
    HEADERS += connector_shm.h
    SOURCES += connector_shm.cpp
    DEFINES += SHM_METHOD
} else {
    message("Using pipes interprocess method for connector")
    HEADERS += connector_pipe.h
    SOURCES += connector_pipe.cpp
    DEFINES += PIPE_METHOD
}

#macx {
#    message("Using MacOS-native interprocess method for connector")
#    HEADERS += connector_mac.h
#    SOURCES += connector_mac.cpp connector_mac.m
#    DEFINES += MAC_METHOD
#} else {
#    message("Using shared memory interprocess method for connector")
#    HEADERS += connector_shm.h
#    SOURCES += connector_shm.cpp
#    DEFINES += SHM_METHOD
#
#}
