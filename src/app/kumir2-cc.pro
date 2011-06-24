include(../../kumir2.pri)

TEMPLATE = app
TARGET = kumir2-cc
DESTDIR = $$IDE_APP_PATH
macx{
DESTDIR = $$IDE_APP_PATH/Kumir.app/Contents/MacOS/
}

SOURCES += main.cpp

QT += script


include(../rpath.pri)

# need to avoid using previously compiled main.o from other project
OBJECTS_DIR = $$OBJECTS_DIR/$$TARGET
# defines default template for this build
DEFINES += CONFIGURATION_TEMPLATE=\\\"Actor*,!KumirCompiler,KumirCppGenerator,KumirAnalizer\\\"

win32 {
    CONFIG(debug, debug|release):LIBS *= -lExtensionSystemd
    else:LIBS *= -lExtensionSystem
    target.path = /bin
    INSTALLS += target
    CONFIG += console
} else:macx {
    CONFIG -= app_bundle
    CONFIG(debug, debug|release):LIBS *= -lExtensionSystem_debug
    else:LIBS *= -lExtensionSystem
    LIBS += -framework CoreFoundation

} else {
    LIBS *= -lExtensionSystem
    target.path  = /bin
    INSTALLS    += target
}
