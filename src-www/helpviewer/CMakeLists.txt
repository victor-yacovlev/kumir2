set(GWT_MAIN_MODULE ru.niisi.kumir2.helpviewer.viewer)
set(SOURCE
    src/ru/niisi/kumir2/helpviewer/viewer.gwt.xml
    src/ru/niisi/kumir2/helpviewer/client/DocBookModel.java
    src/ru/niisi/kumir2/helpviewer/client/DocBookModelType.java
    src/ru/niisi/kumir2/helpviewer/client/DocBookType.java
    src/ru/niisi/kumir2/helpviewer/client/IndexPanel.java
    src/ru/niisi/kumir2/helpviewer/client/OpenNodeEvent.java
    src/ru/niisi/kumir2/helpviewer/client/OpenNodeHandler.java
    src/ru/niisi/kumir2/helpviewer/client/TableOfContentsPanel.java
    src/ru/niisi/kumir2/helpviewer/client/ViewContentPanel.java
    src/ru/niisi/kumir2/helpviewer/client/ViewerRoot.java
    src/ru/niisi/kumir2/helpviewer/client/XrefDatabase.java
)
if (WIN32)
	set(GWTC_COMMAND ${CMAKE_SOURCE_DIR}/gwt/gwtc.cmd ${GWT_DEV_JAR} ${GWT_USER_JAR} ${CMAKE_CURRENT_SOURCE_DIR}/src ${SHARE_PATH}/webapps/helpviewer ${GWT_MAIN_MODULE})
else()
	set(GWTC ${JAVA_RUNTIME} -cp ${GWT_DEV_JAR}:${GWT_USER_JAR}:${CMAKE_CURRENT_SOURCE_DIR}/src com.google.gwt.dev.Compiler )
	set(GWTC_COMMAND ${GWTC} ${GWT_MAIN_MODULE} -war ${SHARE_PATH}/webapps/helpviewer)
endif()



add_custom_command(
    OUTPUT ${SHARE_PATH}/webapps/helpviewer/${GWT_MAIN_MODULE}/${GWT_MAIN_MODULE}.nocache.js
    COMMAND ${GWTC_COMMAND}
    DEPENDS ${SOURCE}
)

add_custom_target(
        helpviewer_gwt
        ALL
        DEPENDS ${SHARE_PATH}/webapps/helpviewer/${GWT_MAIN_MODULE}/${GWT_MAIN_MODULE}.nocache.js
)

file(MAKE_DIRECTORY ${SHARE_PATH}/webapps/helpviewer/images)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/images/thumb_vertical.png DESTINATION ${SHARE_PATH}/webapps/helpviewer/images)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/images/thumb_vertical.png DESTINATION ${SHARE_PATH}/webapps/helpviewer/images)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/_print.html DESTINATION ${SHARE_PATH}/webapps/helpviewer)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/docbook-print-article.css DESTINATION ${SHARE_PATH}/webapps/helpviewer)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/docbook-print-book.css DESTINATION ${SHARE_PATH}/webapps/helpviewer)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/docbook-print.css DESTINATION ${SHARE_PATH}/webapps/helpviewer)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/docbook-screen.css DESTINATION ${SHARE_PATH}/webapps/helpviewer)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/index.html DESTINATION ${SHARE_PATH}/webapps/helpviewer)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/war/viewer.css DESTINATION ${SHARE_PATH}/webapps/helpviewer)

file(MAKE_DIRECTORY ${SHARE_PATH}/webapps/helpviewer/data/russian)
file(COPY ${CMAKE_SOURCE_DIR}/userdocs/language.xml DESTINATION ${SHARE_PATH}/webapps/helpviewer/data/russian)
file(COPY ${CMAKE_SOURCE_DIR}/userdocs/system.xml DESTINATION ${SHARE_PATH}/webapps/helpviewer/data/russian)

