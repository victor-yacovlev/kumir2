#ifndef KUMTYPES_H
#define KUMTYPES_H

#include <stdint.h>
#include <wchar.h>

typedef int32_t         __kumir_int;
typedef double          __kumir_real;
typedef bool            __kumir_bool;
typedef wchar_t         __kumir_char;
typedef wchar_t*        __kumir_string;

typedef enum {
    __KUMIR_INT     = 0x01,
    __KUMIR_REAL    = 0x02,
    __KUMIR_BOOL    = 0x03,
    __KUMIR_CHAR    = 0x04,
    __KUMIR_STRING  = 0x05,
    __KUMIR_RECORD  = 0x06
}
__kumir_scalar_type;

typedef struct {
    size_t                  nfields;
    void*                   fields; // of type __kumir_variant
    __kumir_scalar_type*    types;
}
__kumir_record;


typedef struct { // TODO replace struct with union
    __kumir_int     i;
    __kumir_real    r;
    __kumir_bool    b;
    __kumir_char    c;
    __kumir_string  s;
    __kumir_record  u;
}
__kumir_variant;

typedef struct {
    __kumir_bool        defined;
    __kumir_scalar_type type;
    __kumir_variant     data;
}
__kumir_scalar;

typedef struct {
    unsigned char       dim;
    int32_t             left;
    int32_t             right;
    void*               data;
} __kumir_array;

#endif // KUMTYPES_H
