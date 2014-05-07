project(ExtensionSystem)
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
    include (${QT_USE_FILE})
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

include(../../kumir2_library.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MOC_HEADERS
    kplugin.h
    pluginmanager.h
    commandlineparameter.h
)

set(SOURCES
    logger.cpp
    kplugin.cpp
    pluginspec.cpp
    pluginmanager.cpp
    pluginmanager_impl.cpp
    settings.cpp
    commandlineparameter.cpp
)

set(HEADERS
    kplugin.h
    pluginmanager.h
    switchworkspacedialog.h
    pluginspec.h
    settings.h
    commandlineparameter.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()

add_library(ExtensionSystem SHARED ${SOURCES} ${MOC_SOURCES})
target_link_libraries(ExtensionSystem ${QT_LIBRARIES} ${STDCXX_LIB})

set_property(TARGET ExtensionSystem APPEND PROPERTY COMPILE_DEFINITIONS EXTENSIONSYSTEM_LIBRARY)

handleTranslation(ExtensionSystem)

install(TARGETS ExtensionSystem DESTINATION ${LIBS_DIR})
