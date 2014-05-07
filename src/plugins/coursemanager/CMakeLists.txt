project(CourseManager)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets Xml WebKitWidgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} ${Qt5WebKitWidgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5WebKitWidgets_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include (${QT_USE_FILE})
endif()
include(../../kumir2_plugin.cmake)


set(SOURCES
    coursemanager_plugin.cpp
    task/mainwindow.cpp
    course_model.cpp
)

set(FORMS
    task/editdialog.ui
    task/mainwindow.ui
    task/newkursdialog.ui
)

set(MOC_HEADERS
    coursemanager_plugin.h
    task/mainwindow.h
    course_model.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()
copySpecFile(CourseManager)
add_library(CourseManager SHARED ${MOC_SOURCES} ${SOURCES} ${UI_SOURCES})
handleTranslation(CourseManager)
target_link_libraries(CourseManager ${QT_LIBRARIES} ExtensionSystem DataFormats Widgets)
copyResources(coursemanager)
install(TARGETS CourseManager DESTINATION ${PLUGINS_DIR})
