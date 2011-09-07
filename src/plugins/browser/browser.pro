TEMPLATE = lib
TARGET = Browser
CONFIG += plugin
QT += script network webkit
include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)

OTHER_FILES += Browser.pluginspec

HEADERS += \
    component.h \
    plugin.h \
    webpage.h \
    dir.h \
    localhostserver.h \
    networkaccessmanager.h

SOURCES += \
    component.cpp \
    plugin.cpp \
    webpage.cpp \
    dir.cpp \
    localhostserver.cpp \
    networkaccessmanager.cpp

FORMS +=

