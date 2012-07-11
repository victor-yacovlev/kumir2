set(CONFIGURATION_TEMPLATE
    "Editor,Browser,Actor*,KumirAnalizer,KumirCodeGenerator,KumirCodeRun,!CoreGUI"
)

set(SPLASHSCREEN
    SPLASHSCREEN=coregui/splashscreens/professional.png
)

set(SRC ../main.cpp)
if(WIN32)
    list(APPEND SRC kumir2.rc)
endif(WIN32)

add_executable(kumir2-ide WIN32 ${SRC})
target_link_libraries(kumir2-ide ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-ide APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET kumir2-ide APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-ide PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-ide DESTINATION ${EXEC_DIR})
