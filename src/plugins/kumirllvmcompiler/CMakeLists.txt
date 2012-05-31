include(../../kumir2_plugin.cmake)

set(SOURCES
    kumirllvmcompilerplugin.cpp
)

set(MOC_HEADERS
    kumirllvmcompilerplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirLLVMCompiler)
add_library(KumirLLVMCompiler SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirLLVMCompiler)
target_link_libraries(KumirLLVMCompiler ${QT_LIBRARIES} ExtensionSystem DataFormats)

install(TARGETS KumirLLVMCompiler DESTINATION ${PLUGINS_DIR})
