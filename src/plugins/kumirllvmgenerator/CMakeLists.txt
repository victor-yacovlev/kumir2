include(../../kumir2_plugin.cmake)

set(SOURCES
    kumirllvmgeneratorplugin.cpp
)

set(MOC_HEADERS
    kumirllvmgeneratorplugin.h
)


qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirLLVMGenerator SHARED ${MOC_SOURCES} ${SOURCES})
add_library(KumirLLVMGenerator SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirLLVMGenerator)
target_link_libraries(KumirLLVMGenerator ${QT_LIBRARIES} ExtensionSystem DataFormats)
copyResources(kumirllvmgenerator)

install(TARGETS KumirLLVMGenerator DESTINATION ${PLUGINS_DIR})
