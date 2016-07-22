project(kumir2-teacher)
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


    set(
        CONFIGURATION_TEMPLATE
        "CourseManager,Editor\(teacher\),Actor*,KumirAnalizer\(teacher\),KumirCodeGenerator,KumirCodeRun,!CoreGUI\(icon=teacher\)"
    )


set(
    SPLASHSCREEN
    "coregui/splashscreens/teacher.png"
)

set(SRC ../main.cpp)
if(WIN32)
    list(APPEND SRC kumir2-teacher.rc)
endif(WIN32)

add_executable(kumir2-teacher WIN32 ${SRC})
target_link_libraries(kumir2-teacher ${QT_LIBRARIES} ExtensionSystem ${STDCXX_LIB})
set_property(TARGET kumir2-teacher APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET kumir2-teacher APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-teacher PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-teacher DESTINATION ${EXEC_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/kumir2-teacher.desktop DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications)
install(FILES ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/share/applications/kumir2-teacher.desktop DESTINATION share/applications)
