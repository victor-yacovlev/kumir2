project(ActorUmki)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)


endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets SerialPort REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5SerialPort_INCLUDE_DIRS})
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5SerialPort_LIBRARIES})
    set(MOC_PARAMS "-I/usr/include/qt5/QtCore" "-I${_qt5Core_install_prefix}/include/QtCore")
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)

    set(QT_QTSERIALPORT_INCLUDE_DIR /usr/qt4/QtSerialPort)
    set(QT_QTSERIALPORT_LIBRARY QtSerialPort)

#    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtSvg QtSerialPort REQUIRED)
#    find_package(Qt4 4.7.0 COMPONENTS QtCore QtXml QtGui QtSvg QtSerialPort REQUIRED)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)

    
    find_package(Qt4 4.7.0 COMPONENTS QtSerialPort REQUIRED)

#    FIND_LIBRARY(QT_QTSERIALPORT_LIBRARY
#	NAMES
#	    SerialPort
#	PATH_SUFFIXES
#	    qt4
#    )

    include(${QT_USE_FILE})
    
endif()

add_definitions(-DTRUE=true -DFALSE=false)

find_package(PythonInterp 3.2.0 REQUIRED)
include(../../kumir2_plugin.cmake)

set(SOURCES
    umkimodule.cpp
    umkibase.cpp
    pult.cpp
)

set(MOC_HEADERS
    umkimodule.h
    umkibase.h
    pult.h
)

set(FORMS
    pult.ui
    )
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/umkimodulebase.cpp ${CMAKE_CURRENT_BINARY_DIR}/umkimodulebase.h ${CMAKE_CURRENT_BINARY_DIR}/umkiplugin.cpp ${CMAKE_CURRENT_BINARY_DIR}/umkiplugin.h ${CMAKE_CURRENT_BINARY_DIR}/ActorUmki.pluginspec
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py --update ${CMAKE_CURRENT_SOURCE_DIR}/umki.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/umki.json ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(ActorUmkiPluginSpec ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/ActorUmki.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorUmki.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/umkimodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_PARAMS} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/umkimodulebase.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/umkimodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/umkimodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/umkiplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_PARAMS} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/umkiplugin.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/umkiplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/umkiplugin.h
)

set(SOURCES2
    umkimodulebase.cpp
    umkiplugin.cpp
)

set(MOC_SOURCES2
    umkimodulebase.moc.cpp
    umkiplugin.moc.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()

install(FILES ${PLUGIN_OUTPUT_PATH}/ActorUmki.pluginspec DESTINATION ${PLUGINS_DIR})
handleTranslation(ActorUmki)
add_library(ActorUmki SHARED ${MOC_SOURCES} ${SOURCES} ${MOC_SOURCES2} ${SOURCES2} ${UI_SOURCES})
#target_link_libraries(ActorUmki ${QT_LIBRARIES} ExtensionSystem Widgets ${STDCXX_LIB} ${STDMATH_LIB})
target_link_libraries(ActorUmki ${QT_LIBRARIES} ExtensionSystem Widgets ${STDCXX_LIB} ${STDMATH_LIB})
copyResources(actors/umki)
install(TARGETS ActorUmki DESTINATION ${PLUGINS_DIR})

