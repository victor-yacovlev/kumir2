TEMPLATE = lib
TARGET = Browser
CONFIG += plugin
QT += script webkit
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)

OTHER_FILES += Browser.pluginspec

HEADERS += \
    component.h \
    plugin.h \
    webpage.h \
    dir.h

SOURCES += \
    component.cpp \
    plugin.cpp \
    webpage.cpp \
    dir.cpp

FORMS += \
    component.ui
