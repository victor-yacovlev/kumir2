include(../../kumir2.pri)

TEMPLATE = app
TARGET = $$IDE_APP_TARGET
DESTDIR = $$IDE_APP_PATH

SOURCES += main.cpp
QT += script

include(../rpath.pri)

win32 {
    CONFIG(debug, debug|release):LIBS *= -lExtensionSystemd
    else:LIBS *= -lExtensionSystem

    RC_FILE = kumir2.rc
    target.path = /bin
    INSTALLS += target
} else:macx {
    CONFIG(debug, debug|release):LIBS *= -lExtensionSystem_debug
    else:LIBS *= -lExtensionSystem
    LIBS += -framework CoreFoundation
    ICON = kumir2.icns
    QMAKE_INFO_PLIST = Info.plist
} else {
    LIBS *= -lExtensionSystem

    target.path  = /bin
    INSTALLS    += target
}

OTHER_FILES += kumir2.rc Info.plist
