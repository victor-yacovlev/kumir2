set(
    CONFIGURATION_TEMPLATE
    "Actor*,st_funct,!KumirCodeRun"
)

set(SRC main.cpp)

add_executable(kumir2-run WIN32 ${SRC})
target_link_libraries(kumir2-run ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-run APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
install(TARGETS kumir2-run RUNTIME DESTINATION bin)
