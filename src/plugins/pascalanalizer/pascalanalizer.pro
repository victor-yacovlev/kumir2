TEMPLATE = lib
TARGET = PascalAnalizer

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)

HEADERS += fpc-sourcetree/compiler/freepascal.h \
    pasplugin.h

OTHER_FILES = \
    PascalAnalizer.pluginspec \
    fpc-sourcetree/compiler/freepascal.pas \
    fpc-sourcetree/compiler/build-lib.sh

fp.target = $${IDE_LIBRARY_PATH}/libfreepascal.so
fp.commands = $${_PRO_FILE_PWD_}/fpc-sourcetree/compiler/build-lib.sh $${IDE_LIBRARY_PATH}/libfreepascal.so
QMAKE_EXTRA_TARGETS += fp
PRE_TARGETDEPS += $${IDE_LIBRARY_PATH}/libfreepascal.so

LIBS += -lfreepascal

SOURCES += \
    pasplugin.cpp


