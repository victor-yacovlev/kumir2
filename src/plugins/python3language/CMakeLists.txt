project(Python3Language)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core Gui)

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
    tokenizerinstance.cpp
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
    tokenizerinstance.h
)

kumir2_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})

kumir2_add_plugin(
  NAME Python3Language
  SOURCES SHARED ${MOC_SOURCES} ${SOURCES}
  LIBRARIES ${QT_LIBRARIES} ${PYTHON_LIBRARIES} DataFormats ExtensionSystem
)
