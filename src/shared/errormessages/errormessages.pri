HEADERS += ../../shared/errormessages/errormessages.h


isEmpty(ERRORMESSAGES_PRO) {
    pr.target = $${IDE_DATA_PATH}/$${TARGET}.messages.csv
    pr.commands = python $${_PRO_FILE_PWD_}/../../shared/errormessages/scanmessages.py $${_PRO_FILE_PWD_} $${IDE_SOURCE_TREE}/share/kumir2/$${TARGET}.messages.csv && $${QMAKE_COPY} $${IDE_SOURCE_TREE}/share/kumir2/$${TARGET}.messages.csv $${IDE_DATA_PATH}/$${TARGET}.messages.csv
    QMAKE_EXTRA_TARGETS += pr
    PRE_TARGETDEPS += $${IDE_DATA_PATH}/$${TARGET}.messages.csv
    LIBS *= -l$$qtLibraryTarget(ErrorMessages)
}
