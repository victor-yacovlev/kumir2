project(KumirAnalizer)
cmake_minimum_required(VERSION 2.8.3)
find_package(PythonInterp 3.2.0 REQUIRED)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
    include(${QT_USE_FILE})
endif()
include(../../kumir2_plugin.cmake)

set(FORMS
    quickreferencewidget.ui
)

set(SOURCES
    kumiranalizerplugin.cpp
    analizer.cpp
    lexer.cpp
    statement.cpp
    pdautomata.cpp
    syntaxanalizer.cpp
    kumfilehandler.cpp
    quickreferencewidget.cpp
)

set(MOC_HEADERS
    kumiranalizerplugin.h
    analizer.h
    lexer.h
    pdautomata.h
    syntaxanalizer.h
    kumfilehandler.h
    quickreferencewidget.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_ui(UI_SOURCES ${FORMS})
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_ui(UI_SOURCES ${FORMS})
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(KumirAnalizer)
add_library(KumirAnalizer SHARED ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirAnalizer)
target_link_libraries(KumirAnalizer ${QT_LIBRARIES} ExtensionSystem ErrorMessages DataFormats Widgets ${STDCXX_LIB} ${STDMATH_LIB})

file(MAKE_DIRECTORY ${SHARE_PATH}/kumiranalizer)

add_custom_target(
    KumirAnalizer_messages ALL
    ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/shared/errormessages/scanmessages.py --db=${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer/messages.csv --out=${SHARE_PATH}/kumiranalizer/messages.csv ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer
    DEPENDS ${SOURCERS} ${CMAKE_SOURCE_DIR}/src/shared/errormessages/scanmessages.py ${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer/*.rules ${CMAKE_SOURCE_DIR}/share/kumir2/kumiranalizer/messages.csv
)

install(FILES ${SHARE_PATH}/kumiranalizer/messages.csv DESTINATION share/kumir2/kumiranalizer)

copyResourcesExcl(kumiranalizer .csv)
install(TARGETS KumirAnalizer DESTINATION ${PLUGINS_DIR})
