project(kumir2-courseeditor)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets Xml Svg REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} ${Qt5Svg_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5Svg_LIBRARIES})
    if(WIN32)
        set(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5Core_QTMAIN_LIBRARIES})
    endif()
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(USED_LIBS ${QT_LIBRARIES})
set(HEADERS 
    course_model.h
    csInterface.h
    editdialog.h
    interface.h
    kumfiledialog.h
    mainwindow.h
    newkursdialog.h
    taskControlInterface.h
)
set(MOC_HEADERS
    course_model.h
    editdialog.h
    interface.h
    kumfiledialog.h
    mainwindow.h
    newkursdialog.h
)
set(SOURCES
    course_model.cpp
    csInterface.cpp
    editdialog.cpp
    interface.cpp
    kumfiledialog.cpp
    main.cpp
    mainwindow.cpp
    newkursdialog.cpp
)
set(FORMS
    editdialog.ui
    mainwindow.ui
    newkursdialog.ui
)

set(RESOURCES
    marks.qrc
)

if(${USE_QT} GREATER 4)
    qt5_wrap_ui(UI_SOURCES ${FORMS})
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_add_resources(QRC_SOURCES ${RESOURCES})
else()
    qt4_wrap_ui(UI_SOURCES ${FORMS})
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_add_resources(QRC_SOURCES ${RESOURCES})
endif()

add_executable(kumir2-courseeditor WIN32 ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
target_link_libraries(kumir2-courseeditor ${USED_LIBS} ${STDCXX_LIB})
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-courseeditor PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-courseeditor DESTINATION ${EXEC_DIR})
