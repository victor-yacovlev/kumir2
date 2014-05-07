project(kumir2-xrun)
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

set(
    CONFIGURATION_TEMPLATE
    "!KumirCodeRun"
)

set(SRC ../main.cpp)

add_executable(kumir2-xrun ${SRC})
target_link_libraries(kumir2-xrun ${QT_LIBRARIES} ExtensionSystem ${STDCXX_LIB})
set_property(TARGET kumir2-xrun APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-xrun PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-xrun DESTINATION ${EXEC_DIR})
