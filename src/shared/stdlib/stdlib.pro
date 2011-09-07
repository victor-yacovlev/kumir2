

TEMPLATE = lib
TARGET = KumirStdLib
DEFINES += STDLIB_LIBRARY
include(../../kumir2library.pri)
include(../../rpath.pri)
HEADERS = \
    kumstdlib.h genericinputoutput.h connector.h guirunner.h actorwindow.h userprogramthread.h \
    integeroverflowchecker.h \
    doubleoverflowchecker.h \
    connector_backend.h

SOURCES = \
    kumstdlib.cpp genericinputoutput.cpp connector.cpp guirunner.cpp actorwindow.cpp userprogramthread.cpp \
    integeroverflowchecker.cpp \
    doubleoverflowchecker.cpp

QT += gui


#    message("Using shared memory interprocess method for connector")
#    HEADERS += connector_shm.h
#    SOURCES += connector_shm.cpp
#    DEFINES += SHM_METHOD

    message("Using pipes interprocess method for connector")
    HEADERS += connector_pipe.h
    SOURCES += connector_pipe.cpp
    DEFINES += PIPE_METHOD



