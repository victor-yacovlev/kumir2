TEMPLATE = lib
CONFIG += plugin
TARGET = CoreGUI
QT += script
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)

QT += gui

OTHER_FILES += CoreGUI.pluginspec

HEADERS += \
    mainwindow.h \
    plugin.h \
    switchworkspacedialog.h \
    tabwidget.h

SOURCES += \
    mainwindow.cpp \
    plugin.cpp \
    switchworkspacedialog.cpp \
    tabwidget.cpp

FORMS += \
    mainwindow.ui \
    switchworkspacedialog.ui
