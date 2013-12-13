#ifndef KUMTYPES_H
#define KUMTYPES_H

#ifdef __cplusplus
extern "C" {
#include <stdint.h>
#include <wchar.h>
#include <stdbool.h>

}
#endif

/* External types */

typedef struct array_t {
    int         left1;
    int         right1;
    void *      data;
} array_t;

typedef struct const_array_t {
    int         left1;
    int         right1;
    const void *data;
} const_array_t;

typedef struct array2_t {
    int         left1;
    int         right1;
    int         left2;
    int         right2;
    void *      data;
} array2_t;

typedef struct const_array2_t {
    int         left1;
    int         right1;
    int         left2;
    int         right2;
    const void *data;
} const_array2_t;

typedef struct array3_t {
    int         left1;
    int         right1;
    int         left2;
    int         right2;
    int         left3;
    int         right3;
    void *      data;
} array3_t;

typedef struct const_array3_t {
    int         left1;
    int         right1;
    int         left2;
    int         right2;
    int         left3;
    int         right3;
    const void *data;
} const_array3_t;


/* Internal types -- prefixed with __ */

typedef int32_t         __kumir_int;
typedef double          __kumir_real;
typedef bool            __kumir_bool;
typedef wchar_t         __kumir_char;
typedef wchar_t*        __kumir_string;

typedef enum __kumir_scalar_type {
    __KUMIR_INT     = 0x01,
    __KUMIR_REAL    = 0x02,
    __KUMIR_BOOL    = 0x03,
    __KUMIR_CHAR    = 0x04,
    __KUMIR_STRING  = 0x05,
    __KUMIR_RECORD  = 0x06
}
__kumir_scalar_type;

typedef struct __kumir_record {
    size_t                  nfields;
    union __kumir_variant*  fields;
    __kumir_scalar_type*    types;
}
__kumir_record;

typedef union __kumir_variant {
    __kumir_int     i;
    __kumir_real    r;
    __kumir_bool    b;
    __kumir_char    c;
    __kumir_string  s;
    __kumir_record  u;
}
__kumir_variant;

typedef struct __kumir_scalar {
    __kumir_bool        defined;
    __kumir_scalar_type type;
    __kumir_variant     data;
}
__kumir_scalar;

typedef enum __kumir_stringref_operation {
    __KUMIR_STRINGREF_REPLACE   = 0x00,
    __KUMIR_STRINGREF_PREPEND   = 0x01,
    __KUMIR_STRINGREF_APPEND    = 0x02,
    __KUMIR_STRINGREF_INSERT    = 0x03
}
__kumir_stringref_operation;

typedef struct __kumir_stringref {
    __kumir_scalar*             ref;
    __kumir_stringref_operation op;
    size_t                      from;
    size_t                      length;
}
__kumir_stringref;

typedef struct __kumir_array {
    unsigned char       dim;
    int32_t             size_left[3];
    int32_t             size_right[3];
    int32_t             shape_left[3];
    int32_t             shape_right[3];
    __kumir_scalar*     data;
} __kumir_array;

#endif // KUMTYPES_H
