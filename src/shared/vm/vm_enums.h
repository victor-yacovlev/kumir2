#ifndef VM_ENUMS_H
#define VM_ENUMS_H

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
    EL_TESTING  = 0x08,      // Testing function
    EL_BELOWMAIN= 0x09      // Function evaluated below main function
};

enum ValueType {
    VT_void     = 0x00,
    VT_int      = 0x01,
    VT_real    = 0x02,
    VT_char     = 0x03,
    VT_bool     = 0x04,
    VT_string   = 0x05,
    VT_user     = 0xFF
};

enum ValueKind {
    VK_Plain    = 0x00,
    VK_In       = 0x01,
    VK_InOut    = 0x02,
    VK_Out      = 0x03
};

}

#endif
