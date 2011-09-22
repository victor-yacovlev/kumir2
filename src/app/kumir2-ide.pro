include(../../kumir2.pri)

TEMPLATE = app
!macx {
	TARGET = kumir2-ide
}
macx:TARGET = Kumir
DESTDIR = $$IDE_APP_PATH

SOURCES += main.cpp
QT += script

win32 {
#    CONFIG += console
}

include(../rpath.pri)

# need to avoid using previously compiled main.o from other project
OBJECTS_DIR = $$IDE_BUILD_TREE/src/app/$$TARGET
# defines default template for this build
DEFINES += CONFIGURATION_TEMPLATE=\\\"Editor,Browser,Actor*,KumirAnalizer,KumirNativeGenerator,KumirCodeGenerator,KumirCodeRun,!CoreGUI\\\"


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
    ICON = ../../app_icons/mac/kumir2.icns
    QMAKE_INFO_PLIST = Info.plist
} else {
    LIBS *= -lExtensionSystem

    target.path  = /bin
    INSTALLS    += target
}

OTHER_FILES += kumir2.rc Info.plist
