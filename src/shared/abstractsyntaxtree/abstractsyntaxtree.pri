HEADERS += ../../shared/abstractsyntaxtree/ast.h \
    ../../shared/abstractsyntaxtree/ast_module.h \
    ../../shared/abstractsyntaxtree/ast_algorhitm.h \
    ../../shared/abstractsyntaxtree/ast_statement.h \
    ../../shared/abstractsyntaxtree/ast_expression.h \
    ../../shared/abstractsyntaxtree/ast_variable.h \
    ../../shared/abstractsyntaxtree/ast_variabletype.h \
    ../../shared/abstractsyntaxtree/lexem.h
isEmpty(ABSTRACTSYNTAXTREE_PRO) {
    LIBS *= -l$$qtLibraryTarget(AbstractSyntaxTree)
}

