include(../../kumir2_plugin.cmake)

set(SOURCES
    kumirllvmgeneratorplugin.cpp
    llvmnameprovider.cpp
)

set(MOC_HEADERS
    kumirllvmgeneratorplugin.h
    llvmnameprovider.h
)

set(REQ_LLVM_LIBRARIES
    LLVMTableGen
    LLVMMCJIT
    LLVMRuntimeDyld
    LLVMObject
    LLVMMCDisassembler
    LLVMLinker
    LLVMipo
    LLVMInterpreter
    LLVMInstrumentation
    LLVMJIT
    LLVMExecutionEngine
    LLVMDebugInfo
    LLVMBitWriter
    LLVMX86Disassembler
    LLVMX86AsmParser
    LLVMX86CodeGen
    LLVMX86Desc
    LLVMX86AsmPrinter
    LLVMX86Utils
    LLVMX86Info
    LLVMAsmParser
    LLVMARMDisassembler
    LLVMARMAsmParser
    LLVMARMCodeGen
    LLVMSelectionDAG
    LLVMAsmPrinter
    LLVMMCParser
    LLVMCodeGen
    LLVMScalarOpts
    LLVMInstCombine
    LLVMTransformUtils
    LLVMipa
    LLVMARMDesc
    LLVMARMAsmPrinter
    LLVMARMInfo
    LLVMArchive
    LLVMBitReader
    LLVMAnalysis
    LLVMTarget
    LLVMMC
    LLVMCore
    LLVMSupport
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(KumirLLVMGenerator SHARED ${MOC_SOURCES} ${SOURCES})
add_library(KumirLLVMGenerator SHARED ${MOC_SOURCES} ${SOURCES})
handleTranslation(KumirLLVMGenerator)
target_link_libraries(KumirLLVMGenerator ${QT_LIBRARIES} ${REQ_LLVM_LIBRARIES} ExtensionSystem DataFormats)
copyResources(kumirllvmgenerator)

install(TARGETS KumirLLVMGenerator DESTINATION ${PLUGINS_DIR})
