#ifndef BYTECODE_TABLEELEM_H
#define BYTECODE_TABLEELEM_H

#include <QtCore>
#include "instruction.h"

#ifdef BYTECODE_LIBRARY
#   define BYTECODE_EXPORT Q_DECL_EXPORT
#else
#   define BYTECODE_EXPORT Q_DECL_IMPORT
#endif

namespace Bytecode {

enum ElemType {
    EL_NONE     = 0x00,     // stream delimeter
    EL_LOCAL    = 0x01,       // Local variable
    EL_GLOBAL   = 0x02,      // Global variable
    EL_CONST    = 0x03,       // Constant
    EL_FUNCTION = 0x04,    // Local defined function
    EL_EXTERN   = 0x05,      // External module name
    EL_INIT     = 0x06,     // Local defined module initializer
    EL_MAIN     = 0x07,     // Main (usually - first) function
    EL_TESTING  = 0x08      // Testing function
};

enum ValueType {
    VT_void     = 0x00,
    VT_int      = 0x01,
    VT_float    = 0x02,
    VT_char     = 0x03,
    VT_bool     = 0x04,
    VT_string   = 0x05
};

enum ValueKind {
    VK_Plain    = 0x00,
    VK_In       = 0x01,
    VK_InOut    = 0x02,
    VK_Out      = 0x03
};

struct TableElem {
    ElemType type; // Element type
    ValueType vtype; // Value type
    quint8 dimension; // 0 for regular, [1..3] for arrays
    ValueKind refvalue; // 1 for in-argument,
                     // 2 for in/out-argument,
                     // 3 for out-argument
                     // 0 for regular variable
    quint8 module;  // module id
    quint16 algId; // algorhitm id
    quint16 id; // element id

    QString name; // variable or function name

    QString moduleName; // external module name
    QVariant constantValue; // constant value
    QVector<Instruction> instructions; // for local defined function

};

extern BYTECODE_EXPORT QTextStream& operator<<(QTextStream&, const TableElem&);
extern BYTECODE_EXPORT QTextStream& operator>>(QTextStream&, TableElem&);

extern BYTECODE_EXPORT QDataStream& operator<<(QDataStream&, const TableElem&);
extern BYTECODE_EXPORT QDataStream& operator>>(QDataStream&, TableElem&);

} // namespace Bytecode

#endif // BYTECODE_TABLEELEM_H
