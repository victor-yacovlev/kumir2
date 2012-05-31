set(
    CONFIGURATION_TEMPLATE
    "Actor*,!KumirLLVMCompiler,KumirLLVMGenerator,KumirAnalizer"
)

set(SRC ../main.cpp)

add_executable(kumir2-lc ${SRC})
target_link_libraries(kumir2-lc ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-lc APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
install(TARGETS kumir2-lc DESTINATION ${EXEC_DIR})
