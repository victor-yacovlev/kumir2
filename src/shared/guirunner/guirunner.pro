TEMPLATE = lib
TARGET = KumirGuiRunner
DEFINES += GUIRUNNER_LIBRARY
include(../../kumir2library.pri)
include(../../rpath.pri)
include(../stdlib/stdlib.pri)


QT += gui

HEADERS += \
    guirunner.h \
    actorwindow.h \
    userprogramthread.h

SOURCES += \
    guirunner.cpp \
    actorwindow.cpp \
    userprogramthread.cpp
