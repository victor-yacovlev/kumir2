project(DocBookViewer)
cmake_minimum_required(VERSION 2.8.3)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets Svg Xml PrintSupport REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${Qt5Svg_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5Svg_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml QtSvg REQUIRED)
    include(${QT_USE_FILE})
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../kumir2_library.cmake")
    message(STATUS "DocBookViewer will be build as a part of Kumir")
    include(../../kumir2_library.cmake)
    set(BUILD_AS_PART_OF_KUMIR 1)
    add_definitions(-DBUILD_AS_PART_OF_KUMIR)
else()
    set(BUILD_AS_PART_OF_KUMIR 0)
endif()

if(NOT MSVC)
    if(APPLE)
        set(CMAKE_CXX_FLAGS "-stdlib=libc++ -DAPPLE ${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
    endif()
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
    endif()
endif()


set(MOC_HEADERS
    docbookview.h
    docbookview_impl.h
    contentview.h
    printdialog.h
    sidepanel.h
)

set(SOURCES
    docbookview.cpp
    docbookview_impl.cpp
    docbookmodel.cpp
    docbookfactory.cpp
    document.cpp
    contentview.cpp
    printdialog.cpp
    printrenderer.cpp
    sidepanel.cpp
    mathmlrenderer.cpp
)

set(FORMS
    printdialog.ui
    sidepanel.ui
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()

add_library(DocBookViewer SHARED  ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES})
target_link_libraries(DocBookViewer ${QT_LIBRARIES} ExtensionSystem Widgets)
set_property(TARGET DocBookViewer APPEND PROPERTY COMPILE_DEFINITIONS LIBRARY_BUILD)

if(${BUILD_AS_PART_OF_KUMIR})
    handleTranslation(DocBookViewer)
    copyResources(docbookviewer)
    install(TARGETS DocBookViewer DESTINATION ${LIBS_DIR})
else()
    set(APP_MOC_HEADERS
        mainwindow.h
    )

    set(APP_FORMS
        mainwindow.ui
    )

    set(APP_SOURCES
        mainwindow.cpp
        main.cpp
    )
    qt4_wrap_cpp(APP_MOC_SOURCES ${APP_MOC_HEADERS})
    qt4_wrap_ui(APP_UI_SOURCES ${APP_FORMS})

    add_executable(DocBookViewerApp ${APP_SOURCES} ${APP_MOC_SOURCES} ${APP_UI_SOURCES})
    target_link_libraries(DocBookViewerApp ${QT_LIBRARIES} DocBookViewer)
endif()
