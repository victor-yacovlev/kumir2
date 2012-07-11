set(
    CONFIGURATION_TEMPLATE
    "Actor*,!KumirCompiler,KumirNativeGenerator,KumirAnalizer"
)

set(SRC ../main.cpp)

add_executable(kumir2-cc ${SRC})
target_link_libraries(kumir2-cc ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-cc APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-cc PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-cc DESTINATION ${EXEC_DIR})
