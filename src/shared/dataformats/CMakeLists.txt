set(QT_USE_QTMAIN 1)
find_package(Qt4 4.7.0 COMPONENTS QtCore REQUIRED)
include (${QT_USE_FILE})

include(../../kumir2_library.cmake)


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
target_link_libraries(DataFormats ${QT_LIBRARIES})
set_property(TARGET DataFormats APPEND PROPERTY COMPILE_DEFINITIONS DATAFORMATS_LIBRARY)

handleTranslation(DataFormats)

install(TARGETS DataFormats DESTINATION ${LIBS_DIR})
