set(
    CONFIGURATION_TEMPLATE
    "Actor*,st_funct,!KumirCodeRun"
)

set(SRC ../main.cpp)

add_executable(kumir2-run ${SRC})
target_link_libraries(kumir2-run ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-run APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-run PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-run DESTINATION ${EXEC_DIR})
