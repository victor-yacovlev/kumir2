TEMPLATE = lib
TARGET = AbstractSyntaxTree
DEFINES += ABSTRACTSYNTAXTREE_LIBRARY
include(../../kumir2library.pri)
include(../../rpath.pri)
ABSTRACTSYNTAXTREE_PRO = true
include(abstractsyntaxtree.pri)
SOURCES += ast.cpp \
    ast_module.cpp \
    ast_algorhitm.cpp \
    ast_expression.cpp \
    ast_variable.cpp \
    ast_statement.cpp
