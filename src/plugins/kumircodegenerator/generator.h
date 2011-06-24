#ifndef KUMIRCODEGENERATOR_GENERATOR_H
#define KUMIRCODEGENERATOR_GENERATOR_H

#include <QtCore>

#include "abstractsyntaxtree/ast.h"
#include "abstractsyntaxtree/ast_variable.h"
#include "abstractsyntaxtree/ast_algorhitm.h"
#include "abstractsyntaxtree/lexem.h"
#include "errormessages/errormessages.h"
#include "bytecode/data.h"

namespace KumirCodeGenerator {

class Generator : public QObject
{
    Q_OBJECT
public:
    explicit Generator(QObject *parent = 0);
    void reset(const AST::Data * ast, Bytecode::Data * bc);
    void addModule(const AST::Module * mod);
    void generateConstantTable();
private:
    quint16 constantValue(Bytecode::ValueType type, const QVariant & value);
    void addKumirModule(int id, const AST::Module * mod);
    void addFunction(int id, int moduleId, Bytecode::ElemType type, const AST::Algorhitm * alg);

    QList<Bytecode::Instruction> instructions(
        int modId, int algId, int level,
        const QList<AST::Statement*> & statements);

    static void shiftInstructions(QList<Bytecode::Instruction> &instrs, int offset);

    void ERROR(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void ASSIGN(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void ASSERT(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void INIT(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void CALL_SPECIAL(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void LOOP(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void IFTHENELSE(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void SWITCHCASEELSE(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);
    void BREAK(int modId, int algId, int level, const AST::Statement * st, QList<Bytecode::Instruction> & result);

    QList<Bytecode::Instruction> calculate(int modId, int algId, int level, const AST::Expression* st);

    void findVariable(int modId, int algId, const AST::Variable * var, Bytecode::VariableScope & scope, quint16 & id) const;
    static const AST::Variable * returnValue(const AST::Algorhitm * alg);
    void findFunction(const AST::Algorhitm * alg, quint8 & module, quint16 & id) ;

    static Bytecode::ValueType valueType(AST::VariableBaseType t);
    static Bytecode::ValueKind valueKind(AST::VariableAccessType t);
    static Bytecode::InstructionType operation(AST::ExpressionOperator op);

    const AST::Data * m_ast;
    Bytecode::Data * m_bc;
    QList< QPair<Bytecode::ValueType, QVariant> > l_constants;
    QList< QPair<quint8,quint16> > l_externs;
    QList< QPair<quint8,quint16> > l_loopBreaks;
    QList< quint16 > l_funcBreaks;
};

} // namespace KumirCodeGenerator

#endif // KUMIRCODEGENERATOR_GENERATOR_H
