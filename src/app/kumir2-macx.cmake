set(CONFIGURATION_TEMPLATE
    "Editor,Browser,Actor*,KumirAnalizer,PascalAnalizer,KumirNativeGenerator,KumirCodeGenerator,KumirCodeRun,!CoreGUI"
)

set(SPLASHSCREEN
    SPLASHSCREEN=coregui/splashscreens/professional.png
)

set(SRC main.cpp)

add_executable(Kumir MACOSX_BUNDLE ${SRC})
target_link_libraries(Kumir ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET Kumir APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET Kumir APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")
install(TARGETS Kumir DESTINATION ${EXEC_DIR})
