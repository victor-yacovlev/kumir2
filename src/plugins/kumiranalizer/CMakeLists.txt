set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
find_package(PythonInterp)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)

set(SOURCES
    kumiranalizerplugin.cpp
    analizer.cpp
    lexer.cpp
    statement.cpp
    pdautomata.cpp
    syntaxanalizer.cpp
)

set(MOC_HEADERS
    kumiranalizerplugin.h
    analizer.h
    lexer.h
    pdautomata.h
    syntaxanalizer.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirAnalizer)
add_library(KumirAnalizer SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirAnalizer)
target_link_libraries(KumirAnalizer ${QT_LIBRARIES} ExtensionSystem ErrorMessages DataFormats)

file(MAKE_DIRECTORY ${SHARE_PATH}/kumiranalizer)

add_custom_target(
    KumirAnalizer_messages ALL
    ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/shared/errormessages/scanmessages.py --db=${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer/messages.csv --out=${SHARE_PATH}/kumiranalizer/messages.csv ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer
    DEPENDS ${SOURCERS} ${CMAKE_SOURCE_DIR}/src/shared/errormessages/scanmessages.py ${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer/*.rules ${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer/messages.csv
)

install(FILES ${SHARE_PATH}/kumiranalizer/messages.csv DESTINATION share/kumir2/kumiranalizer)

copyResourcesExcl(kumiranalizer .csv)
install(TARGETS KumirAnalizer DESTINATION ${PLUGINS_DIR})
