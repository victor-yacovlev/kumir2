project(DataFormats)
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
    find_library(QCA_LIBRARY NAMES qca HINTS ${QCA_LIBDIR})
    find_path(QCA_INCLUDE_DIR QtCrypto/qca.h ${QCA_INCDIR})
endif()




set(EXTRA_LIBS
)

include(../../kumir2_library.cmake)

if(QCA_LIBRARY AND QCA_INCLUDE_DIR)
    message(STATUS "Found QCA Library: ${QCA_LIBRARY}")
    message(STATUS "Found QCA header: ${QCA_INCLUDE_DIR}/QtCrypto/qca.h")
    add_definitions(-DHAS_QCA)
    include_directories(${QCA_INCLUDE_DIR})
    set(EXTRA_LIBS ${QCA_LIBRARY})
else()
    message(STATUS "QCA library not found! Signing and verifying programs will disabled!")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    ast.cpp
    ast_module.cpp
    ast_type.cpp
    ast_algorhitm.cpp
    ast_expression.cpp
    ast_variable.cpp
    ast_statement.cpp
    kumfile.cpp
)

add_library(DataFormats SHARED  ${SOURCES})
target_link_libraries(DataFormats ${QT_LIBRARIES} ${EXTRA_LIBS} ${STDCXX_LIB})
set_property(TARGET DataFormats APPEND PROPERTY COMPILE_DEFINITIONS DATAFORMATS_LIBRARY)

handleTranslation(DataFormats)

install(TARGETS DataFormats DESTINATION ${LIBS_DIR})
