include(../kumir2.pri)

win32 {
    DLLDESTDIR = $$IDE_APP_PATH
}

DESTDIR = $$IDE_LIBRARY_PATH

include(rpath.pri)

TRANSLATIONS += ../../../share/kumir2/translations/$${TARGET}_ru.ts

TARGET = $$qtLibraryTarget($$TARGET)



INCLUDEPATH += $$IDE_SOURCE_TREE/src/shared
#contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols

!macx {
    win32 {
        target.path = /bin
        target.files = $$DESTDIR/$${TARGET}.dll
    } else {
        target.path = /$$IDE_LIBRARY_BASENAME/kumir2
    }
    INSTALLS += target
}

QT -= gui
