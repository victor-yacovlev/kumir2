set(
    CONFIGURATION_TEMPLATE
    "Actor*,!KumirCompiler,KumirNativeGenerator,KumirAnalizer"
)

set(SRC main.cpp)

add_executable(kumir2-cc WIN32 ${SRC})
target_link_libraries(kumir2-cc ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-cc APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
install(TARGETS kumir2-cc RUNTIME DESTINATION bin)
