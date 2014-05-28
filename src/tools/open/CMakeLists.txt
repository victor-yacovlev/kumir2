project(kumir2-open)
cmake_minimum_required(VERSION 2.8.3)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core Widgets REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES})
    if(WIN32)
        set(QT_LIBRARIES ${QT_LIBRARIES} ${Qt5Core_QTMAIN_LIBRARIES})
    endif()
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
    include(${QT_USE_FILE})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(USED_LIBS ${QT_LIBRARIES})

set(SOURCES 
    main.cpp
    messager.cpp
    dialog.cpp
    appmanager.cpp
    procmanager.cpp
    settings.cpp
)

set(HEADERS
    messager.h
    dialog.h
    appmanager.h
    procmanager.h
    settings.h
)

set(FORMS
    dialog.ui
)

set(MOC_HEADERS
    dialog.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt5_wrap_ui(UI_SOURCES ${FORMS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
    qt4_wrap_ui(UI_SOURCES ${FORMS})
endif()

if(NOT WIN32 AND NOT APPLE)
    set(SOURCES ${SOURCES} messager_unix.cpp procmanager_linux.cpp)
    set(HEADERS ${HEADERS} messager_unix.h procmanager_linux.h)
endif()

if(WIN32)
    add_definitions(-DPSAPI_VERSION=1)
    set(SOURCES ${SOURCES} procmanager_winapi.cpp messager_shm.cpp)
    set(HEADERS ${HEADERS} procmanager_winapi.h messager_shm.h)
    set(USED_LIBS ${USED_LIBS} psapi kernel32)
endif()

add_executable(kumir2-open WIN32 ${UI_SOURCES} ${MOC_SOURCES} ${SOURCES})
target_link_libraries(kumir2-open ${USED_LIBS} ${STDCXX_LIB})
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-open PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-open DESTINATION ${EXEC_DIR})
