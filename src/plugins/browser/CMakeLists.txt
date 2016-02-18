project(Browser)
cmake_minimum_required(VERSION 2.8.9)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.5.0 COMPONENTS Core Widgets WebEngineWidgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5WebEngineWidgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5WebEngineWidgets_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg QtNetwork QtWebkit REQUIRED)
    include(${QT_USE_FILE})
endif()

include(../../kumir2_plugin.cmake)


set(SOURCES
    component.cpp
    plugin.cpp
    webpage.cpp
    dir.cpp
    localhostserver.cpp
    networkaccessmanager.cpp
)

set(MOC_HEADERS
    component.h
    plugin.h
    webpage.h
    dir.h
    localhostserver.h
    networkaccessmanager.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(Browser)
add_library(Browser SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(Browser)
target_link_libraries(Browser ${QT_LIBRARIES} ExtensionSystem)

install(TARGETS Browser DESTINATION ${PLUGINS_DIR})
