HEADERS += ../../shared/errormessages/errormessages.h


isEmpty(ERRORMESSAGES_PRO) {
    contains(CONFIG,generate_messages) {
        pr.target = $${IDE_DATA_PATH}/$${MSG_FILE}
        pr.commands = python $${_PRO_FILE_PWD_}/../../shared/errormessages/scanmessages.py --db=$${IDE_SOURCE_TREE}/share/kumir2/$${MSG_FILE} --out=$${IDE_DATA_PATH}/$${MSG_FILE} $${_PRO_FILE_PWD_} $${EXTRA_MESSAGES_DIRS}
        QMAKE_EXTRA_TARGETS += pr
        PRE_TARGETDEPS += $${IDE_DATA_PATH}/$${MSG_FILE}
    }
    LIBS *= -l$$qtLibraryTarget(ErrorMessages)
}
