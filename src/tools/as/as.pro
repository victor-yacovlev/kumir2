
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
    CONFIG(debug, debug|release):LIBS *= -lDataFormatsd
    else:LIBS *= -lDataFormats
    target.path = /bin
    INSTALLS += target
} else:macx {
    CONFIG(debug, debug|release):LIBS *= -lDataFormats_debug
    else:LIBS *= -lDataFormats
    LIBS += -framework CoreFoundation
} else {
    LIBS *= -lDataFormats

    target.path  = /bin
    INSTALLS    += target
}



SOURCES = main.cpp
