set(
    CONFIGURATION_TEMPLATE
    "Actor*,!KumirBCompiler,KumirCodeGenerator,KumirAnalizer"
)

set(SRC main.cpp)

add_executable(kumir2-bc WIN32 ${SRC})
target_link_libraries(kumir2-bc ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-bc APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
install(TARGETS kumir2-bc RUNTIME DESTINATION bin)
