TEMPLATE = lib
TARGET = ActorPainterC
include(../../kumir2library.pri)
include(../../rpath.pri)
include(../../shared/stdlib/stdlib.pri)

QT += gui

HEADERS += painterwindow.h painterview.h painterruler.h paintertools.h painternewimagedialog.h \
    painterworker.h \
    painter_c.h
SOURCES += painterwindow.cpp paintertools.cpp painterruler.cpp painterview.cpp painternewimagedialog.cpp \
    painterworker.cpp \
    painter_c.cpp
FORMS += painterwindow.ui painternewimagedialog.ui
