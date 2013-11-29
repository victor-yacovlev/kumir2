set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore QtGui REQUIRED)
include (${QT_USE_FILE})

set(
    CONFIGURATION_TEMPLATE
    "Actor*,!KumirCompilerTool,LLVMCodeGenerator,KumirAnalizer\(teacher,preload=Files,preload=Strings\)"
)

set(SRC ../main.cpp)

add_executable(kumir2-llvmc ${SRC})
target_link_libraries(kumir2-llvmc ${QT_LIBRARIES} ExtensionSystem ${STDCXX_LIB})
set_property(TARGET kumir2-llvmc APPEND PROPERTY COMPILE_DEFINITIONS CONFIGURATION_TEMPLATE="${CONFIGURATION_TEMPLATE}")
if (XCODE OR MSVC_IDE)
    set_target_properties (kumir2-llvmc PROPERTIES PREFIX "../")
endif(XCODE OR MSVC_IDE)
install(TARGETS kumir2-llvmc DESTINATION ${EXEC_DIR})
