project(kumir2-classic)
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
    include_directories(${QT_INCLUDE_DIR})
    include(${QT_USE_FILE})
endif()

set(CONFIGURATION_TEMPLATE
    "CourseManager,Editor,Browser,Actor*,KumirAnalizer,KumirCodeGenerator,KumirCodeRun,!CoreGUI\(notabs,icon=classic,nostartpage,nosessions\)"
)
set(SPLASHSCREEN
    "coregui/splashscreens/classic.png"
)

set(SRC ../main.cpp)
if(WIN32)
    list(APPEND SRC kumir2-classic.rc)
endif(WIN32)

#include(${QT_USE_FILE})

add_executable(kumir2-classic WIN32 ${SRC})
target_link_libraries(kumir2-classic ${QT_LIBRARIES} ExtensionSystem ${STDCXX_LIB})
set_property(TARGET kumir2-classic APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET kumir2-classic APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-classic PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-classic DESTINATION ${EXEC_DIR})
