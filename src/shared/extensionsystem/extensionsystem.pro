TEMPLATE = lib
TARGET = ExtensionSystem
DEFINES += EXTENSIONSYSTEM_LIBRARY
include(../../kumir2library.pri)
QT += gui script

unix:!macx:!freebsd*:LIBS += -ldl

QMAKE_PRL_LIBS = -framework Qt<ModuleDep>

DEFINES += IDE_TEST_DIR=\\\"$$IDE_SOURCE_TREE\\\"

HEADERS += \
    kplugin.h \
    pluginspec.h \
    pluginmanager.h \
    settingsdialog.h \
    switchworkspacedialog.h

SOURCES += \
    kplugin.cpp \
    pluginspec.cpp \
    pluginmanager.cpp \
    settingsdialog.cpp \
    switchworkspacedialog.cpp

FORMS += \
    settingsdialog.ui \
    switchworkspacedialog.ui
