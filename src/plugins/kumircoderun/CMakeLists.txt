project(KumirCodeRun)
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
    find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
    include (${QT_USE_FILE})
endif()


include(../../kumir2_plugin.cmake)


set(SOURCES
    runplugin.cpp
    run.cpp
    consolerun.cpp
    commonrun.cpp
    util.cpp
    guirun.cpp
    kumvariablesmodel.cpp
)

set(HEADERS
    ../../shared/vm/vm.hpp
)

set(MOC_HEADERS
    runplugin.h
    run.h
    commonrun.h
    guirun.h
    kumvariablesmodel.h
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(KumirCodeRun)
add_library(KumirCodeRun SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirCodeRun)
target_link_libraries(KumirCodeRun ${QT_LIBRARIES} ExtensionSystem  ${STDCXX_LIB} ${STDMATH_LIB})

install(TARGETS KumirCodeRun DESTINATION ${PLUGINS_DIR})
