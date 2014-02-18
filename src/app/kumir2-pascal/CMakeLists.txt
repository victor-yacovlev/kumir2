set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})

set(CONFIGURATION_TEMPLATE
    "CourseManager,Editor,Browser,FPCAnalizer,GDBRun\(subsystem=fpc,io=cp866\),!CoreGUI\(icon=pascal\)"
)
set(SPLASHSCREEN
    "coregui/splashscreens/pascal.png"
)

set(SRC ../main.cpp)
if(WIN32)
    list(APPEND SRC kumir2-pascal.rc)
endif(WIN32)

#include(${QT_USE_FILE})

add_executable(kumir2-pascal WIN32 ${SRC})
target_link_libraries(kumir2-pascal ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-pascal APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET kumir2-pascal APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-pascal PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-pascal DESTINATION ${EXEC_DIR})
