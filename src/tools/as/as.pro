
TEMPLATE = app
TARGET = kumir2-as
include(../../../kumir2.pri)
include(../../rpath.pri)

DESTDIR = $$IDE_APP_PATH
macx{
DESTDIR = $$IDE_APP_PATH/Kumir.app/Contents/MacOS/
}


CONFIG += console
macx: CONFIG -= app_bundle
QT -= gui

INCLUDEPATH += ../../

win32 {
    CONFIG(debug, debug|release):LIBS *= -lBytecoded
    else:LIBS *= -lBytecode
    target.path = /bin
    INSTALLS += target
} else:macx {
    CONFIG(debug, debug|release):LIBS *= -lBytecode_debug
    else:LIBS *= -lBytecode
    LIBS += -framework CoreFoundation
} else {
    LIBS *= -lBytecode

    target.path  = /bin
    INSTALLS    += target
}



SOURCES = main.cpp
