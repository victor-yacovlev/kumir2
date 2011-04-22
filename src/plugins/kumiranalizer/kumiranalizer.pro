TEMPLATE = lib
TARGET = KumirAnalizer

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)
include(../../shared/interfaces/analizer.pri)
include(../../shared/abstractsyntaxtree/abstractsyntaxtree.pri)


HEADERS += kumiranalizerplugin.h \
    analizer.h \
    analizer_p.h \
    lexer.h \
    statement.h \
    lexer_p.h \
    lexem.h \
    error_constants.h \
    pdautomata.h \
    pdautomata_p.h

SOURCES += kumiranalizerplugin.cpp \
    analizer.cpp \
    lexer.cpp \
    statement.cpp \
    pdautomata.cpp

OTHER_FILES += KumirAnalizer.pluginspec

RESOURCES += \
    keywords.qrc
