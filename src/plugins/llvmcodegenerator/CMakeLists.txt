project(LLVMCodeGenerator)
cmake_minimum_required(VERSION 3.0.0)

if(NOT DEFINED USE_QT)
    set(USE_QT 4)
endif(NOT DEFINED USE_QT)

if(${USE_QT} GREATER 4)
    # Find Qt5
    find_package(Qt5 5.3.0 COMPONENTS Core REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} BEFORE)
    set(QT_LIBRARIES ${Qt5Core_LIBRARIES})
else()
    # Find Qt4
    set(QT_USE_QTMAIN 1)
    find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
    include(${QT_USE_FILE})
endif()

find_package(Llvm COMPONENTS Linker BitReader BitWriter AsmParser REQUIRED)

add_definitions(${Llvm_DEFINITIONS})
include_directories(${Llvm_INCLUDE_DIR})
include(../../kumir2_plugin.cmake)

if(MSVC)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif(MSVC)

set(DEP_LIBS
)

if(NOT WIN32)
    set(DEP_LIBS dl pthread)
endif()

set(SOURCES
    llvmcodegeneratorplugin.cpp
    llvmgenerator.cpp
    nametranslator.cpp
    llvm_module_${Llvm_API_VERSION}.cpp
    llvm_function_${Llvm_API_VERSION}.cpp
    llvm_type_${Llvm_API_VERSION}.cpp
)

set(MOC_HEADERS
    llvmcodegeneratorplugin.h
)

set(EXLIB_SOURCES
    stdlib_c.cpp
)

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(LLVMCodeGenerator)
add_library(LLVMCodeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(LLVMCodeGenerator)
target_link_libraries(LLVMCodeGenerator
    ${Llvm_LD_FLAGS}
    ${Llvm_LIBRARIES}
    ${DEP_LIBS}
    ${QT_LIBRARIES}
    ExtensionSystem
    DataFormats
    ErrorMessages
)

if(WIN32)
#    exec_program(${MINGW_ROOT}/bin/gcc ARGS "--version" OUTPUT_VARIABLE GCC_VERSION_OUTPUT)
#    string(REGEX MATCH "[1-9].[1-9].[1-9]" GCC_VERSION "${GCC_VERSION_OUTPUT}")
#    add_custom_target(
#        stdlib_preproc.cxx
#        ALL
#        DEPENDS stdlib_c.h stdlib_c.cpp
#        COMMAND ${MINGW_ROOT}/bin/gcc -E -fno-exceptions -fno-use-cxa-atexit -std=c++0x
#            -I${CMAKE_SOURCE_DIR}/src/shared
#            -o ${CMAKE_CURRENT_BINARY_DIR}/stdlib_preproc.cxx
#            ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_c.cpp
#    )
#    add_custom_target(
#        stdlib.llvm
#        ALL
#        DEPENDS stdlib_preproc.cxx
#        COMMAND ${Clang_EXECUTABLE} -c -fno-exceptions -fno-use-cxa-atexit -std=c++0x -emit-llvm -nobuiltininc -nostdinc++
#            -fno-ms-compatibility -fno-ms-extensions
#            -o ${SHARE_PATH}/llvmcodegenerator/stdlib.bc
#            ${CMAKE_CURRENT_BINARY_DIR}/stdlib_preproc.cxx
#    )

#    exec_program(${MINGW_ROOT}/bin/gcc ARGS "--version" OUTPUT_VARIABLE GCC_VERSION_OUTPUT)
#    string(REGEX MATCH "[1-9].[1-9].[1-9]" GCC_VERSION "${GCC_VERSION_OUTPUT}")
#    add_custom_target(
#        stdlib_preproc.cxx
#        ALL
#        DEPENDS stdlib_c.h stdlib_c.cpp
#        COMMAND ${CMAKE_CXX_COMPILER}
#            -Zm300 -EHsc -GR -Y- -D_CRT_SECURE_NO_WARNINGS
#            /I ${CMAKE_SOURCE_DIR}/src/shared
#            /P /Fi${CMAKE_CURRENT_BINARY_DIR}/stdlib_preproc.cxx
#            ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_c.cpp
#    )
#    add_custom_target(
#        stdlib.llvm
#        ALL
#        DEPENDS stdlib_preproc.cxx
#        COMMAND ${Clang_EXECUTABLE} -c -std=c++0x -emit-llvm
#            -D_CRT_SECURE_NO_WARNINGS
#            -Wno-ignored-attributes
#            -fms-compatibility -fms-extensions
#            -o ${SHARE_PATH}/llvmcodegenerator/stdlib.bc
#            ${CMAKE_CURRENT_BINARY_DIR}/stdlib_preproc.cxx
#    )

    add_custom_target(
        stdlib.llvm
        ALL
        DEPENDS stdlib_c.h stdlib_c.cpp dummy_sjlj.cpp
        COMMAND ${CLANG_EXECUTABLE} -c -std=gnu++0x -fno-exceptions -emit-llvm
            -DUSE_MINGW_TOOLCHAIN
            -I${CMAKE_SOURCE_DIR}/src/shared
            -o ${SHARE_PATH}/llvmcodegenerator/stdlib.bc
            ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_c.cpp
    )
else()
    find_program(Clang_EXECUTABLE NAMES clang PATHS ${LLVM_ROOT} ${LLVM_ROOT}/bin)
    add_custom_command(
        OUTPUT ${SHARE_PATH}/llvmcodegenerator/stdlib.bc
        DEPENDS stdlib_c.h stdlib_c.cpp
        COMMAND ${Clang_EXECUTABLE} -c -std=c++0x -emit-llvm
            -I${CMAKE_SOURCE_DIR}/src/shared
            -o ${SHARE_PATH}/llvmcodegenerator/stdlib.bc
            ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_c.cpp
    )
    add_custom_target(llvm-stdlib ALL DEPENDS ${SHARE_PATH}/llvmcodegenerator/stdlib.bc)
    if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_custom_command(
        OUTPUT ${SHARE_PATH}/llvmcodegenerator/stdlib_debug.o
        DEPENDS stdlib_c.h stdlib_c.cpp
        COMMAND ${Clang_EXECUTABLE} -c -g -std=c++0x -DSTDLIB_DEBUG
            -I${CMAKE_SOURCE_DIR}/src/shared
            -o ${SHARE_PATH}/llvmcodegenerator/stdlib_debug.o
            ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_c.cpp
    )
    add_custom_target(llvm-stdlib-debug ALL DEPENDS ${SHARE_PATH}/llvmcodegenerator/stdlib_debug.o)
    endif()
endif(WIN32)

file(MAKE_DIRECTORY ${SHARE_PATH}/llvmcodegenerator)

install(TARGETS LLVMCodeGenerator DESTINATION ${PLUGINS_DIR})
install(FILES ${SHARE_PATH}/llvmcodegenerator/stdlib.bc DESTINATION ${RESOURCES_DIR}/llvmcodegenerator)
