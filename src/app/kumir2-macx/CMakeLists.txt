set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})
set(CMAKE_MACOSX_RPATH 1)

set(CONFIGURATION_TEMPLATE
    "CourseManager,Editor,Browser,Actor*,KumirAnalizer,KumirCodeGenerator,KumirCodeRun,!CoreGUI"
)

set(SPLASHSCREEN
    SPLASHSCREEN=coregui/splashscreens/professional.png
)

set(SRC ../main.cpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(MACOSX_BUNDLE_ICON_FILE kumir2.icns)

if(XCODE)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Debug/Kumir.app/Contents/Resources)
    file(COPY ${CMAKE_SOURCE_DIR}/app_icons/mac/kumir2.icns DESTINATION ${CMAKE_BINARY_DIR}/Debug/Kumir.app/Contents/Resources)
else()
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Kumir.app/Contents/Resources)
    file(COPY ${CMAKE_SOURCE_DIR}/app_icons/mac/kumir2.icns DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/Kumir.app/Contents/Resources)
endif(XCODE)

add_executable(Kumir MACOSX_BUNDLE ${SRC})
set_target_properties(Kumir PROPERTIES RPATH "@loader_path/../lib")
target_link_libraries(Kumir ${QT_LIBRARIES} ExtensionSystem)

set_property(TARGET Kumir APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
set_property(TARGET Kumir APPEND PROPERTY COMPILE_DEFINITIONS SPLASHSCREEN="${SPLASHSCREEN}")

install(TARGETS Kumir DESTINATION  ./)

