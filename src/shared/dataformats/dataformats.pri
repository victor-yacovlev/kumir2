HEADERS += ../../shared/dataformats/ast.h \
    ../../shared/dataformats/ast_module.h \
    ../../shared/dataformats/ast_algorhitm.h \
    ../../shared/dataformats/ast_statement.h \
    ../../shared/dataformats/ast_expression.h \
    ../../shared/dataformats/ast_variable.h \
    ../../shared/dataformats/ast_variabletype.h \
    ../../shared/dataformats/lexem.h \
    ../../shared/dataformats/bc_instruction.h \
    ../../shared/dataformats/bytecode.h \
    ../../shared/dataformats/bc_tableelem.h \
    ../../shared/dataformats/kumfile.h
isEmpty(DATAFORMATS_PRO) {
    LIBS *= -l$$qtLibraryTarget(DataFormats)
}

