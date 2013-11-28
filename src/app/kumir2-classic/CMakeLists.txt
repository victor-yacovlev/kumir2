set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})

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
