TEMPLATE = lib
TARGET = DataFormats
DEFINES += DATAFORMATS_LIBRARY
include(../../kumir2library.pri)
include(../../rpath.pri)
DATAFORMATS_PRO = true
include(dataformats.pri)
SOURCES += ast.cpp \
    ast_module.cpp \
    ast_algorhitm.cpp \
    ast_expression.cpp \
    ast_variable.cpp \
    ast_statement.cpp \
    bytecode.cpp \
    bc_tableelem.cpp \
    bc_instruction.cpp \
    kumfile.cpp
