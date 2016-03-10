project(ActorKeyboard)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
    set(MOC_PARAMS "-I/usr/include/qt5" "-I/usr/include/qt5/QtCore" "-I${_qt5Core_install_prefix}/include/QtCore" "-DQT_PLUGIN")
    foreach(IDIR ${Qt5Core_INCLUDE_DIRS})
        set(MOC_PARAMS "-I${IDIR}" ${MOC_PARAMS})
    endforeach()
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

find_package(PythonInterp 3.2.0 REQUIRED)
include(../../kumir2_plugin.cmake)

set(SOURCES
    keyboardmodule.cpp
)

set(MOC_HEADERS
    keyboardmodule.h
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/keyboardmodulebase.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/keyboardmodulebase.h
        ${CMAKE_CURRENT_BINARY_DIR}/keyboardplugin.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/keyboardplugin.h
        ${CMAKE_CURRENT_BINARY_DIR}/ActorKeyboard.pluginspec
    COMMAND ${PYTHON_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
            --update
            ${CMAKE_CURRENT_SOURCE_DIR}/keyboard.json
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/keyboard.json
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(
    ActorKeyboardPluginSpec
    ALL
    ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/ActorKeyboard.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorKeyboard.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/keyboardmodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        -I${CMAKE_SOURCE_DIR}/src/shared
        ${MOC_PARAMS}
        -o${CMAKE_CURRENT_BINARY_DIR}/keyboardmodulebase.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/keyboardmodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/keyboardmodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/keyboardplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE}
        -I${CMAKE_SOURCE_DIR}/src/shared
        ${MOC_PARAMS}
        -o${CMAKE_CURRENT_BINARY_DIR}/keyboardplugin.moc.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/keyboardplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/keyboardplugin.h
)

set(SOURCES_GENERATED
    keyboardmodulebase.cpp
    keyboardplugin.cpp
)

set(MOC_SOURCES_GENERATED
    keyboardmodulebase.moc.cpp
    keyboardplugin.moc.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()

install(
    FILES ${PLUGIN_OUTPUT_PATH}/ActorKeyboard.pluginspec
    DESTINATION ${PLUGINS_DIR}
)

handleTranslation(ActorKeyboard)

add_library(
    ActorKeyboard
    SHARED
    ${MOC_SOURCES} ${SOURCES}
    ${MOC_SOURCES_GENERATED} ${SOURCES_GENERATED}
)

target_link_libraries(
    ActorKeyboard
    ${QT_LIBRARIES}
    ExtensionSystem
    Widgets
    ${STDCXX_LIB} ${STDMATH_LIB}
)

copyResources(actors/keyboard)

install(
    TARGETS ActorKeyboard
    DESTINATION ${PLUGINS_DIR}
)
