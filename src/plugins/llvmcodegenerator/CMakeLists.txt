project(LLVMCodeGenerator)
cmake_minimum_required(VERSION 2.8.3)

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

if(${USE_QT} GREATER 4)
    qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
else()
    qt4_wrap_cpp(MOC_SOURCES ${MOC_HEADERS})
endif()
copySpecFile(LLVMCodeGenerator)
add_library(LLVMCodeGenerator SHARED ${MOC_SOURCES} ${SOURCES})
#handleTranslation(LLVMCodeGenerator)
target_link_libraries(LLVMCodeGenerator
    ${Llvm_LIBRARIES}
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


