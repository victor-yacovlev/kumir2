set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})
include(../../kumir2_plugin.cmake)

find_package(PythonLibs 3.2)
include_directories(${PYTHON_INCLUDE_DIRS})
if(MSVC)
    string(REGEX REPLACE
        "libpython([0-9][0-9])\\.a"
        "python\\1.lib"
        PYTHON_LIBRARIES
        "${PYTHON_LIBRARIES}"
    )
endif()
add_definitions(-DQT_NO_KEYWORDS)

set(SOURCES
    python3languageplugin.cpp
)

set(MOC_HEADERS
    python3languageplugin.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(Python3Language)
add_library(Python3Language SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(Python3Language)
target_link_libraries(Python3Language ${QT_LIBRARIES} ${PYTHON_LIBRARIES} ExtensionSystem)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/kumir2/python3language)
copyResources(python3language)
install(TARGETS Python3Language DESTINATION ${PLUGINS_DIR})
