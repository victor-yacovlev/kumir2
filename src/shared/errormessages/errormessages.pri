HEADERS += ../../shared/errormessages/errormessages.h


isEmpty(ERRORMESSAGES_PRO) {
    contains(CONFIG,generate_messages) {
        pr.target = $${IDE_DATA_PATH}/$${TARGET}.messages.csv
        pr.commands = python $${_PRO_FILE_PWD_}/../../shared/errormessages/scanmessages.py $${IDE_SOURCE_TREE}/share/kumir2/$${TARGET}.messages.csv $${IDE_DATA_PATH}/$${TARGET}.messages.csv $${_PRO_FILE_PWD_}
        QMAKE_EXTRA_TARGETS += pr
        PRE_TARGETDEPS += $${IDE_DATA_PATH}/$${TARGET}.messages.csv
    }
    LIBS *= -l$$qtLibraryTarget(ErrorMessages)
}
