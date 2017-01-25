project(Python3Language)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
    include(${QT_USE_FILE})
endif()

include(../../kumir2_plugin.cmake)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DPYTHON_SCRIPT_DEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

unset(PYTHON_LIBRARY)
find_package(PythonLibs 3.4)
include_directories(${PYTHON_INCLUDE_DIRS})
if(MSVC)
    string(REGEX REPLACE
        "libpython([0-9][0-9])\\.a"
        "python\\1.lib"
        PYTHON_LIBRARIES
        "${PYTHON_LIBRARIES}"
    )
endif()

set(SOURCES
    python3languageplugin.cpp
    analizerinstance.cpp
    pyutils.cpp
    interpretercallback.cpp
    pythonrunthread.cpp
    actorshandler.cpp
    pyfilehandler.cpp
    variablesmodel.cpp
    sandboxwidget.cpp
    syntaxchecksettingspage.cpp
    pyinterpreterprocess.cpp
)

set(MOC_HEADERS
    python3languageplugin.h
    analizerinstance.h
    interpretercallback.h
    pythonrunthread.h
    actorshandler.h
    pyfilehandler.h
    variablesmodel.h
    sandboxwidget.h
    syntaxchecksettingspage.h
    pyinterpreterprocess.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(Python3Language)
add_library(Python3Language SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(Python3Language)
target_link_libraries(Python3Language ${QT_LIBRARIES} ${PYTHON_LIBRARIES} DataFormats ExtensionSystem ${STDCXX_LIB} ${STDMATH_LIB})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/kumir2/python3language)
copyResources(python3language)
install(TARGETS Python3Language DESTINATION ${PLUGINS_DIR})
