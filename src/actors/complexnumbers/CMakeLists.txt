project(ActorComplexNumbers)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
    set(MOC_PARAMS "-I${_qt5Core_install_prefix}/include/QtCore")
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

find_package(PythonInterp 2.6 REQUIRED)
include(../../kumir2_plugin.cmake)

set(SOURCES
    complexnumbersmodule.cpp
)

set(MOC_HEADERS
    complexnumbersmodule.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersmodulebase.cpp ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersmodulebase.h ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersplugin.cpp ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersplugin.h ${CMAKE_CURRENT_BINARY_DIR}/ActorComplexNumbers.pluginspec
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py --update ${CMAKE_CURRENT_SOURCE_DIR}/complexnumbers.json
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/complexnumbers.json ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/gen_actor_source.py
)

add_custom_target(ActorComplexNumbersPluginSpec ALL ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/ActorComplexNumbers.pluginspec ${PLUGIN_OUTPUT_PATH}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ActorComplexNumbers.pluginspec
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersmodulebase.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_PARAMS} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/complexnumbersmodulebase.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersmodulebase.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersmodulebase.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersplugin.moc.cpp
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_PARAMS} -I${CMAKE_SOURCE_DIR}/src/shared -o${CMAKE_CURRENT_BINARY_DIR}/complexnumbersplugin.moc.cpp ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersplugin.h
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/complexnumbersplugin.h
)

set(SOURCES2
    complexnumbersmodulebase.cpp
    complexnumbersplugin.cpp
)

set(MOC_SOURCES2
    complexnumbersmodulebase.moc.cpp
    complexnumbersplugin.moc.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()

install(FILES ${PLUGIN_OUTPUT_PATH}/ActorComplexNumbers.pluginspec DESTINATION ${PLUGINS_DIR})
handleTranslation(ActorComplexNumbers)
add_library(ActorComplexNumbers SHARED ${MOC_SOURCES} ${SOURCES} ${MOC_SOURCES2} ${SOURCES2})
target_link_libraries(ActorComplexNumbers ${QT_LIBRARIES} ExtensionSystem Widgets  ${STDCXX_LIB} ${STDMATH_LIB})

install(TARGETS ActorComplexNumbers DESTINATION ${PLUGINS_DIR})

