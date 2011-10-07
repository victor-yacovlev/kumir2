set(
    CONFIGURATION_TEMPLATE
    "Editor\(teacher\),Browser,Actor*,KumirAnalizer\(teacher\),KumirNativeGenerator,KumirCodeGenerator,KumirCodeRun,!CoreGUI\(icon=teacher\)"
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
target_link_libraries(kumir2-teacher ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-teacher APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET kumir2-teacher APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")
install(TARGETS kumir2-teacher DESTINATION ${EXEC_DIR})
