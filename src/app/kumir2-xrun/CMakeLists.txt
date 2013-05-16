set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})

set(
    CONFIGURATION_TEMPLATE
    "!KumirCodeRun"
)

set(SRC ../main.cpp)

add_executable(kumir2-xrun ${SRC})
target_link_libraries(kumir2-xrun ${QT_LIBRARIES} ExtensionSystem)
set_property(TARGET kumir2-xrun APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-xrun PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-xrun DESTINATION ${EXEC_DIR})
