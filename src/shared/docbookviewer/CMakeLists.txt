project(docbookviewer)
cmake_minimum_required(VERSION 2.8)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../kumir2_library.cmake")
    message(STATUS "DocBookViewer will be build as a part of Kumir")
    include(../../kumir2_library.cmake)
    set(BUILD_AS_PART_OF_KUMIR 1)
else()
    set(BUILD_AS_PART_OF_KUMIR 0)
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
include (${QT_USE_FILE})

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
    endif()
endif()


set(MOC_HEADERS
    docbookview.h
    docbookview_impl.h
    contentview.h
    printdialog.h
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
)

set(FORMS
    printdialog.ui
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
qt4_wrap_ui(UI_SOURCES ${FORMS})

add_library(DocBookViewer SHARED  ${SOURCES} ${MOC_SOURCES} ${UI_SOURCES})
target_link_libraries(DocBookViewer ${QT_LIBRARIES})
set_property(TARGET DocBookViewer APPEND PROPERTY COMPILE_DEFINITIONS DOCBOOKVIEWER_LIBRARY)

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