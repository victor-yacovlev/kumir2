# Based on Qt Creator pro file

#version check qt
contains(QT_VERSION, ^4\\.[0-5]\\..*) {
    message("Cannot build Kumir with Qt version $${QT_VERSION}.")
    error("Use at least Qt 4.6.")
}

include(kumir2.pri)
#include(doc/doc.pri)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS = src share
