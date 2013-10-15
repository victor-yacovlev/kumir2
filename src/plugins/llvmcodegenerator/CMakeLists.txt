set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)

include (${QT_USE_FILE})

include(../../kumir2_plugin.cmake)

find_library(LLVM_Core_LIBRARY NAMES LLVMCore)
find_library(LLVM_Support_LIBRARY NAMES LLVMSupport)
find_library(LLVM_CodeGen_LIBRARY NAMES LLVMCodeGen)
find_library(LLVM_BitReader_LIBRARY NAMES LLVMBitReader)
find_library(LLVM_BitWriter_LIBRARY NAMES LLVMBitWriter)
find_library(LLVM_Linker_LIBRARY NAMES LLVMLinker)
find_library(LLVM_AsmParser_LIBRARY NAMES LLVMAsmParser)
find_library(LLVM_TransformUtils_LIBRARY NAMES LLVMTransformUtils)
find_library(LLVM_ipa_LIBRARY NAMES LLVMipa)
find_library(LLVM_Analysis_LIBRARY NAMES LLVMAnalysis)
find_library(LLVM_Target_LIBRARY NAMES LLVMTarget)
find_library(LLVM_MC_LIBRARY NAMES LLVMMC)
find_library(LLVM_Object_LIBRARY NAMES LLVMObject)
find_library(LLVM_Archive_LIBRARY NAMES LLVMArchive)
find_library(LLVM_X86AsmParser_LIBRARY NAMES LLVMX86AsmParser)
find_library(LLVM_X86CodeGen_LIBRARY NAMES LLVMX86CodeGen)
find_library(LLVM_SelectionDAG_LIBRARY NAMES LLVMSelectionDAG)
find_library(LLVM_AsmPrinter_LIBRARY NAMES LLVMAsmPrinter)
find_library(LLVM_MCParser_LIBRARY NAMES LLVMMCParser)
find_library(LLVM_CodeGen_LIBRARY NAMES LLVMCodeGen)
find_library(LLVM_ScalarOpts_LIBRARY NAMES LLVMScalarOpts)
find_library(LLVM_InstCombine_LIBRARY NAMES LLVMInstCombine)
find_library(LLVM_X86Disassembler_LIBRARY NAMES LLVMX86Disassembler)
find_library(LLVM_X86Desc_LIBRARY NAMES LLVMX86Desc)
find_library(LLVM_X86Info_LIBRARY NAMES LLVMX86Info)
find_library(LLVM_X86AsmPrinter_LIBRARY NAMES LLVMX86AsmPrinter)
find_library(LLVM_X86Utils_LIBRARY NAMES LLVMX86Utils)

if(NOT MSVC)
    add_definitions(-D__STDC_LIMIT_MACROS=100 -D__STDC_CONSTANT_MACROS=100)
endif()

set(DEP_LIBS
)

if(NOT WIN32)
    set(DEP_LIBS dl pthread)
endif()

set(SOURCES
    llvmcodegeneratorplugin.cpp
    llvmgenerator.cpp
    nametranslator.cpp
)

set(MOC_HEADERS
    llvmcodegeneratorplugin.h
)

set(EXLIB_SOURCES
    stdlib_c.cpp
)

qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
copySpecFile(LLVMCodeGenerator)
add_library(LLVMCodeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(LLVMCodeGenerator)
target_link_libraries(LLVMCodeGenerator
#    ${LLVM_X86AsmParser_LIBRARY}
#    ${LLVM_X86CodeGen_LIBRARY}
#    ${LLVM_SelectionDAG_LIBRARY}
#    ${LLVM_AsmPrinter_LIBRARY}
#    ${LLVM_MCParser_LIBRARY}
    ${LLVM_CodeGen_LIBRARY}
#    ${LLVM_ScalarOpts_LIBRARY}
#    ${LLVM_InstCombine_LIBRARY}
    ${LLVM_Linker_LIBRARY}
    ${LLVM_TransformUtils_LIBRARY}
#    ${LLVM_X86Disassembler_LIBRARY}
#    ${LLVM_X86Desc_LIBRARY}
#    ${LLVM_X86Info_LIBRARY}
#    ${LLVM_X86AsmPrinter_LIBRARY}
    ${LLVM_ipa_LIBRARY}
    ${LLVM_Analysis_LIBRARY}
    ${LLVM_Target_LIBRARY}
#    ${LLVM_MC_LIBRARY}
    ${LLVM_Object_LIBRARY}
    ${LLVM_Archive_LIBRARY}
    ${LLVM_BitReader_LIBRARY}
    ${LLVM_BitWriter_LIBRARY}
    ${LLVM_AsmParser_LIBRARY}
#    ${LLVM_X86Utils_LIBRARY}
    ${LLVM_Core_LIBRARY}
    ${LLVM_Support_LIBRARY}
    ${DEP_LIBS}
    ${QT_LIBRARIES}
    ExtensionSystem
    DataFormats
    ErrorMessages
)

add_custom_target(
    stdlib.llvm
    ALL
    DEPENDS stdlib_c.h stdlib_c.cpp
    COMMAND clang -c -std=c++0x -emit-llvm
        -I${CMAKE_SOURCE_DIR}/src/shared
        -o ${SHARE_PATH}/llvmcodegenerator/stdlib.bc
        ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_c.cpp
)

file(MAKE_DIRECTORY ${SHARE_PATH}/llvmcodegenerator)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_c.h DESTINATION ${SHARE_PATH}/llvmcodegenerator)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/kumtypes.h DESTINATION ${SHARE_PATH}/llvmcodegenerator)

install(TARGETS LLVMCodeGenerator DESTINATION ${PLUGINS_DIR})


