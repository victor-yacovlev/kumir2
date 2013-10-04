set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)


set(SOURCES
    llvmcodegeneratorplugin.cpp
    llvmgenerator.cpp
)

set(MOC_HEADERS
    llvmcodegeneratorplugin.h
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(LLVMCodeGenerator)
add_library(LLVMCodeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(LLVMCodeGenerator)
target_link_libraries(LLVMCodeGenerator ${QT_LIBRARIES} ExtensionSystem DataFormats ErrorMessages)

install(TARGETS LLVMCodeGenerator DESTINATION ${PLUGINS_DIR})

