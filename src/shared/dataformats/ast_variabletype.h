#ifndef AST_VARIABLETYPE_H
#define AST_VARIABLETYPE_H

namespace AST {

/** Defines base (non-array) type for variables,
  * expressions and algorhitm return values */
enum VariableBaseType {

    /** None type */
    TypeNone = 0x0000,

    /** System-dependent (32/64 bit) integer type */
    TypeInteger = 0x0001,

    /** System dependent (precision: single on ARM or double on others)
      * floating-point type */
    TypeReal = 0x0002,

    /** Unicode character type */
    TypeCharect = 0x0003,

    /** Unicode string type */
    TypeString = 0x0004,

    /** Boolean type */
    TypeBoolean = 0x0005,

    /** Custom type mask for 0xFF00..0xFFFF types, defined
      * in typedef table (for future use) */
    TypeUser = 0xFF00
};

/** Defines, what variable related to */
enum VariableAccessType {

    /** Regular variable for read/write access */
    AccessRegular,

    /** Algorhitm in-argument (russian: arg) */
    AccessArgumentIn,

    /** Algorhitm out-reference argument (russian: rez) */
    AccessArgumentOut,

    /** Algorhitm in/out-reference argument (russian: argrez) */
    AccessArgumentInOut

};

#include <QString>

inline QString dump(const enum VariableBaseType & type)
{
    if (type==TypeNone)
        return "\"void\"";
    else if (type==TypeInteger)
        return "\"int\"";
    else if (type==TypeReal)
        return "\"float\"";
    else if (type==TypeCharect)
        return "\"char\"";
    else if (type==TypeString)
        return "\"string\"";
    else if (type==TypeBoolean)
        return "\"bool\"";
    else {
        int id = type & 0x00FF;
        return QString("\"custom_%1\"").arg(id);
    }
}

inline QString dump(const enum VariableAccessType & type)
{
    if (type==AccessArgumentIn)
        return "in-argument";
    else if (type==AccessArgumentOut)
        return "out-argument";
    else if (type==AccessArgumentInOut)
        return "in/out-argument";
    else
        return "regular";
}

}

#endif // AST_VARIABLETYPE_H
