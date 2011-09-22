include(../../kumir2.pri)

TEMPLATE = app
!macx {
	TARGET = kumir2-classic
}
macx:TARGET = Kumir

DESTDIR = $$IDE_APP_PATH

SOURCES += main.cpp
QT += script

include(../rpath.pri)

# need to avoid using previously compiled main.o from other project
OBJECTS_DIR = $$IDE_BUILD_TREE/src/app/$$TARGET
# defines default template for this build
DEFINES += CONFIGURATION_TEMPLATE=\\\"Editor,Browser,Actor*,KumirAnalizer,KumirCodeGenerator,KumirCodeRun,!CoreGUI\\(notabs,icon=classic,nostartpage,nosessions\\)\\\"
DEFINES += SPLASHSCREEN=\\\"coregui/splashscreens/classic.png\\\"

win32 {
    CONFIG(debug, debug|release):LIBS *= -lExtensionSystemd
    else:LIBS *= -lExtensionSystem

    RC_FILE = kumir2-classic.rc
    target.path = /bin
    INSTALLS += target
} else:macx {
    CONFIG(debug, debug|release):LIBS *= -lExtensionSystem_debug
    else:LIBS *= -lExtensionSystem
    LIBS += -framework CoreFoundation
    ICON = ../../app_icons/mac/kumir2-classic.icns
    QMAKE_INFO_PLIST = Info.plist
} else {
    LIBS *= -lExtensionSystem

    target.path  = /bin
    INSTALLS    += target
}

OTHER_FILES += kumir2-classic.rc Info.plist
