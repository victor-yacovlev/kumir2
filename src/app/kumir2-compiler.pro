include(../../kumir2.pri)

TEMPLATE = app
TARGET = kumir2-cc
DESTDIR = $$IDE_APP_PATH

SOURCES += main.cpp
DEFINES += HAS_CONFIGUTATION_TEMPLATE
QT += script


include(../rpath.pri)

configuration.target = configuration_template.h
configuration.commands = 'echo static const char \\* CONFIGURATION_TEMPLATE=\\"\\&KumirCompiler,KumirCppGenerator,KumirAnalizer\\";' > configuration_template.h;
QMAKE_EXTRA_TARGETS += configuration
PRE_TARGETDEPS += configuration_template.h

win32 {
    CONFIG(debug, debug|release):LIBS *= -lExtensionSystemd
    else:LIBS *= -lExtensionSystem
    target.path = /bin
    INSTALLS += target
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
