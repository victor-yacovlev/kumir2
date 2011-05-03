TEMPLATE = lib
TARGET = ErrorMessages
DEFINES += ERRORMESSAGES_LIBRARY
include(../../kumir2library.pri)
include(../../rpath.pri)
ERRORMESSAGES_PRO = true
QT -= gui
include(errormessages.pri)
SOURCES += errormessages.cpp
OTHER_FILES += scanmessages.py
