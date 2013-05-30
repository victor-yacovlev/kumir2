set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_library.cmake)

find_library(QCA_LIBRARY NAMES qca HINTS ${QCA_LIBDIR})
find_path(QCA_INCLUDE_DIR QtCrypto/qca.h ${QCA_INCDIR})

set(EXTRA_LIBS
)

if(QCA_LIBRARY AND QCA_INCLUDE_DIR)
    message(STATUS "Found QCA Library: ${QCA_LIBRARY}")
    message(STATUS "Found QCA header: ${QCA_INCLUDE_DIR}/QtCrypto/qca.h")
    add_definitions(-DHAS_QCA)
    include_directories(${QCA_INCLUDE_DIR})
    set(EXTRA_LIBS ${QCA_LIBRARY})
else()
    message(WARNING "QCA library not found! Signing and verifying programs will disabled!")
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
target_link_libraries(DataFormats ${QT_LIBRARIES} ${EXTRA_LIBS})
set_property(TARGET DataFormats APPEND PROPERTY COMPILE_DEFINITIONS DATAFORMATS_LIBRARY)

handleTranslation(DataFormats)

install(TARGETS DataFormats DESTINATION ${LIBS_DIR})
