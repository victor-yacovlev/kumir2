project(DataFormats)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)
kumir2_use_qt(Core)

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

kumir2_add_library(
    NAME        DataFormats
    SOURCES     ${SOURCES}
    LIBRARIES   ${QT_LIBRARIES}
)
