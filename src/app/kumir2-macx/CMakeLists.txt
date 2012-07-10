set(CONFIGURATION_TEMPLATE
    "Editor,Browser,Actor*,KumirAnalizer,KumirCodeGenerator,KumirCodeRun,!CoreGUI"
)

set(SPLASHSCREEN
    SPLASHSCREEN=coregui/splashscreens/professional.png
)

set(SRC ../main.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(MACOSX_BUNDLE_ICON_FILE kumir2.icns)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Kumir.app/Contents/Resources)
file(COPY ${CMAKE_SOURCE_DIR}/app_icons/mac/kumir2.icns DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Kumir.app/Contents/Resources)

add_executable(Kumir MACOSX_BUNDLE ${SRC})
target_link_libraries(Kumir ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET Kumir APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET Kumir APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")
install(TARGETS Kumir DESTINATION  ./)

