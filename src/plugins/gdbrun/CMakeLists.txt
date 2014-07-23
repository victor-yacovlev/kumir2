project(GDBRun)
cmake_minimum_required(VERSION 2.8.3)
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


set(SOURCES
    gdbrunplugin.cpp
    pascalvariablesmodel.cpp
)

set(HEADERS
)

set(MOC_HEADERS
    gdbrunplugin.h
    pascalvariablesmodel.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(GDBRun)
add_library(GDBRun SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(KumirCodeRun)
target_link_libraries(GDBRun ${QT_LIBRARIES} ExtensionSystem  ${STDCXX_LIB} ${STDMATH_LIB})

install(TARGETS GDBRun DESTINATION ${PLUGINS_DIR})
