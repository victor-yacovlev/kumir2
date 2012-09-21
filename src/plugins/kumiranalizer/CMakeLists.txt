include(../../kumir2_plugin.cmake)
find_package(PythonInterp)


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
    pdautomata_p.h
    syntaxanalizer.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirAnalizer)
add_library(KumirAnalizer SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirAnalizer)
target_link_libraries(KumirAnalizer ${QT_LIBRARIES} KumirStdLib ExtensionSystem ErrorMessages DataFormats)

file(MAKE_DIRECTORY ${SHARE_PATH}/kumiranalizer)

add_custom_target(
    KumirAnalizer_messages ALL
    ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/shared/errormessages/scanmessages.py --db=${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer/messages.csv --out=${SHARE_PATH}/kumiranalizer/messages.csv ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer
)

install(FILES ${SHARE_PATH}/kumiranalizer/messages.csv DESTINATION share/kumir2/kumiranalizer)

copyResourcesExcl(kumiranalizer .csv)
install(TARGETS KumirAnalizer DESTINATION ${PLUGINS_DIR})
