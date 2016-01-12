#ifndef KUMIRCODEGENERATOR_GENERATOR_H
#define KUMIRCODEGENERATOR_GENERATOR_H

#include <QtCore>

#include "dataformats/ast.h"
#include "dataformats/ast_variable.h"
#include "dataformats/ast_algorhitm.h"
#include "dataformats/lexem.h"
#include "errormessages/errormessages.h"
#include "interfaces/generatorinterface.h"
#include "vm/vm_enums.h"
#include "vm/vm_instruction.hpp"



namespace Bytecode {
class Data;
}

namespace AST {
struct Lexem;
}

namespace KumirCodeGenerator {

typedef Shared::GeneratorInterface::DebugLevel DebugLevel;
struct ConstValue {
    QVariant value;
    QList<Bytecode::ValueType> baseType;
    QString recordModuleName;
    QString recordClassLocalizedName;
    QByteArray recordClassAsciiName;
    quint8 dimension;
    inline bool operator==(const ConstValue & other) {
        return
                baseType == other.baseType &&
                dimension == other.dimension &&
                recordModuleName == other.recordModuleName &&
                recordClassAsciiName == other.recordClassAsciiName &&
                value == other.value;
    }
    inline ConstValue() {
        baseType.push_back(Bytecode::VT_void);
        dimension = 0;
    }
};

class Generator : public QObject
{

    Q_OBJECT
public:
    explicit Generator(QObject *parent = 0);
    void reset(const AST::DataPtr ast, Bytecode::Data * bc);
    void addModule(const AST::ModulePtr  mod);
    void generateConstantTable();
    void generateExternTable();
    void setDebugLevel(DebugLevel debugLevel);
private:
    QList<Bytecode::Instruction> makeLineInstructions(const QList<AST::LexemPtr> & lexems) const;
    quint16 constantValue(Bytecode::ValueType type, quint8 dimension, const QVariant & value,
                          const QString & recordModule, const QString & recordClass
                          );
    quint16 constantValue(const QList<Bytecode::ValueType> & type, quint8 dimension, const QVariant & value,
                          const QString & recordModule, const QString & recordClass
                          );
    void addKumirModule(int id, const AST::ModulePtr  mod);
    void addFunction(int id, int moduleId, Bytecode::ElemType type, const AST::ModulePtr  mod, const AST::AlgorithmPtr  alg);
    void addInputArgumentsMainAlgorhitm(int moduleId, int algorhitmId, const AST::ModulePtr  mod, const AST::AlgorithmPtr  alg);

    QList<Bytecode::Instruction> instructions(
        int modId, int algId, int level,
        const QList<AST::StatementPtr> & statements);

    static void shiftInstructions(QList<Bytecode::Instruction> &instrs, int offset);
    static void setBreakAddress(QList<Bytecode::Instruction> &instrs, int level, int address);

    void ERRORR(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void ASSIGN(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void ASSERT(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void PAUSE_STOP(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void INIT(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void CALL_SPECIAL(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void LOOP(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void IFTHENELSE(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void SWITCHCASEELSE(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);
    void BREAK(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result);

    QList<Bytecode::Instruction> calculate(int modId, int algId, int level, const AST::ExpressionPtr st);

    void findVariable(int modId, int algId, const AST::VariablePtr  var, Bytecode::VariableScope & scope, quint16 & id) const;
    static const AST::VariablePtr  returnValue(const AST::AlgorithmPtr  alg);
    void findFunction(const AST::AlgorithmPtr  alg, quint8 & module, quint16 & id) ;


    static QList<Bytecode::ValueType> valueType(const AST::Type & t);
    static Bytecode::ValueKind valueKind(AST::VariableAccessType t);
    static Bytecode::InstructionType operation(AST::ExpressionOperator op);

    AST::DataPtr ast_;
    Bytecode::Data * byteCode_;
    QList< ConstValue > constants_;
    QList< QPair<quint8,quint16> > externs_;
    DebugLevel debugLevel_;

};

} // namespace KumirCodeGenerator

#endif // KUMIRCODEGENERATOR_GENERATOR_H
