project(Widgets)
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

include(../../kumir2_library.cmake)


include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    actionproxy.cpp
    groupbox.cpp
    cyrillicmenu.cpp
    secondarywindow.cpp
    dockwindowplace.cpp
    dockwindowplace_impl.cpp
    dockwindowplace_container.cpp
    multipagedialog.cpp
    multipagedialog_impl.cpp
    declarativesettingspage.cpp
    declarativesettingspage_impl.cpp
    secondarywindow_generic.cpp
    utf8textbrowser.cpp
)

set(MOC_HEADERS
    actionproxy.h
    groupbox.h
    cyrillicmenu.h
    secondarywindow.h
    dockwindowplace.h
    dockwindowplace_container.h
    dockwindowplace_impl.h
    multipagedialog.h
    multipagedialog_impl.h
    declarativesettingspage.h
    declarativesettingspage_impl.h
    secondarywindow_generic.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()

add_library(Widgets SHARED  ${SOURCES} ${MOC_SOURCES})
target_link_libraries(Widgets ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET Widgets APPEND PROPERTY COMPILE_DEFINITIONS WIDGETS_LIBRARY)

handleTranslation(Widgets)
copyResources(widgets)
install(TARGETS Widgets DESTINATION ${LIBS_DIR})
