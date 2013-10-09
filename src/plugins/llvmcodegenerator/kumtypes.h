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
    __KUMIR_INT,
    __KUMIR_REAL,
    __KUMIR_BOOL,
    __KUMIR_CHAR,
    __KUMIR_STRING
}
__kumir_scalar_type;

typedef struct { // TODO replace struct with union
    __kumir_int i;
    __kumir_real r;
    __kumir_bool b;
    __kumir_char c;
    __kumir_string s;
}
__kumir_variant;

typedef struct {
    __kumir_bool        defined;
    __kumir_scalar_type type;
    __kumir_variant     data;
}
__kumir_scalar;

typedef struct {
    int32_t         dim;
    int32_t         left_1;
    int32_t         right_1;
    int32_t         left_2;
    int32_t         right_2;
    int32_t         left_3;
    int32_t         right_3;
    __kumir_scalar* data;
} __kumir_array;

#endif // KUMTYPES_H
