TEMPLATE = lib
TARGET = Bytecode
DEFINES += BYTECODE_LIBRARY
include(../../kumir2library.pri)
include(../../rpath.pri)
HEADERS += instruction.h \
    tableelem.h \
    data.h
SOURCES += instruction.cpp \
    tableelem.cpp \
    data.cpp
