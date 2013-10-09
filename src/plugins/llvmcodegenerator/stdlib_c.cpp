#include "stdlib_c.h"
#include "stdlib/kumirstdlib.hpp"
#include <memory.h>
#include <string.h>
#include <wchar.h>

static Kumir::FileType __kumir_scalar_to_file_type(const __kumir_scalar & scalar);

EXTERN void __use_all_types()
{
    __kumir_bool        b;
    __kumir_int         i;
    __kumir_real        r;
    __kumir_char        c;
    __kumir_string      s;
    __kumir_scalar      scalar;
    __kumir_array       array;
}

EXTERN void __kumir_init_scalar(__kumir_scalar * scalar)
{
    scalar->defined = false;
}

EXTERN __kumir_scalar __kumir_create_int(__kumir_int value)
{
    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_INT;
    result.data.i = value;
    return result;
}

EXTERN __kumir_scalar __kumir_create_real(__kumir_real value)
{
    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_REAL;
    result.data.r = value;
    return result;
}

EXTERN __kumir_scalar __kumir_create_bool(__kumir_bool value)
{
    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_BOOL;
    result.data.b = value;
    return result;
}

EXTERN __kumir_scalar __kumir_create_char(const char * utf8)
{
    std::string utf8string(utf8);
    std::wstring wstr = Kumir::Core::fromUtf8(utf8string);
    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_CHAR;
    result.data.c = wstr[0];
    return result;
}

EXTERN __kumir_scalar __kumir_create_string(const char * utf8)
{
    std::string utf8string(utf8);
    std::wstring wstr = Kumir::Core::fromUtf8(utf8string);
    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_STRING;
    result.data.s = reinterpret_cast<wchar_t*>(calloc(wstr.length() + 1, sizeof(wchar_t)));
    memcpy(result.data.s, wstr.c_str(), wstr.length() * sizeof(wchar_t));
    result.data.s[wstr.length()] = L'\0';
    return result;
}

EXTERN __kumir_variant __kumir_copy_variant(const __kumir_variant rvalue, __kumir_scalar_type type)
{
    __kumir_variant lvalue;
    switch (type) {
    case __KUMIR_INT:
        lvalue.i = rvalue.i;        
        break;
    case __KUMIR_REAL:
        lvalue.r = rvalue.r;
        break;
    case __KUMIR_BOOL:
        lvalue.b = rvalue.b;
        break;
    case __KUMIR_CHAR:
        lvalue.c = rvalue.c;
        break;
    case __KUMIR_STRING: {
        size_t sz = wcslen(rvalue.s);
        lvalue.s = reinterpret_cast<wchar_t*>(calloc(sz+1u, sizeof(wchar_t)));
        wcsncpy(lvalue.s, rvalue.s, sz);
        lvalue.s[sz] = L'\0';
        break;
    }
    case __KUMIR_RECORD: {
        lvalue.u.nfields = rvalue.u.nfields;
        lvalue.u.types = reinterpret_cast<__kumir_scalar_type*>(calloc(lvalue.u.nfields, sizeof(__kumir_scalar_type)));
        lvalue.u.fields = reinterpret_cast<__kumir_variant*>(calloc(lvalue.u.nfields, sizeof(__kumir_variant)));
        __kumir_variant * lfields = reinterpret_cast<__kumir_variant*>(lvalue.u.fields);
        __kumir_variant * rfields = reinterpret_cast<__kumir_variant*>(rvalue.u.fields);
        for (size_t i=0u; i<rvalue.u.nfields; i++) {
            __kumir_scalar_type type = lvalue.u.types[i] = rvalue.u.types[i];
            lfields[i] = __kumir_copy_variant(rfields[i], type);
        }
        break;
    }
    }
    return lvalue;
}

EXTERN __kumir_scalar __kumir_copy_scalar(const __kumir_scalar rvalue)
{
    __kumir_scalar lvalue;
    lvalue.defined = rvalue.defined;
    lvalue.type = rvalue.type;
    __kumir_check_value_defined(rvalue);
    if (lvalue.defined) {
        lvalue.data = __kumir_copy_variant(rvalue.data, rvalue.type);
    }
    return lvalue;
}

static void __kumir_handle_abort()
{
    const std::wstring message = Kumir::Core::fromUtf8("ОШИБКА ВЫПОЛНЕНИЯ: ") +
            Kumir::Core::getError();
    Kumir::Encoding enc = Kumir::UTF8;
#if defined(WIN32) || defined(_WIN32)
    enc = Kumir::CP866;
#endif
    const std::string loc_message = Kumir::Coder::encode(enc, message);
    std::cerr << loc_message << std::endl;
    exit(1);
}

EXTERN void __kumir_check_value_defined(const __kumir_scalar value)
{
    if (!value.defined) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение величины не определено"));
        __kumir_handle_abort();
    }
}

EXTERN void __kumir_free_scalar(__kumir_scalar scalar)
{
    if (scalar.defined && __KUMIR_STRING == scalar.type) {
        free(scalar.data.s);
    }
    else if (scalar.defined && __KUMIR_RECORD == scalar.type) {
        __kumir_scalar_type * types = scalar.data.u.types;
        __kumir_variant * fields = reinterpret_cast<__kumir_variant*>(scalar.data.u.fields);
        for (size_t i=0; i<scalar.data.u.nfields; i++) {
            if (__KUMIR_STRING == types[i]) {
                free(fields[i].s);
            }
        }
        free(types);
        free(fields);
    }
}

EXTERN void __kumir_output_stdout_ii(const __kumir_scalar value, const int format1, const int format2)
{
    __kumir_check_value_defined(value);
    switch (value.type) {
    case __KUMIR_INT:
        Kumir::IO::writeInteger(format2, value.data.i);
        break;
    case __KUMIR_REAL:
        Kumir::IO::writeReal(format2, format1, value.data.r);
        break;
    case __KUMIR_BOOL:
        Kumir::IO::writeBool(format2, value.data.b);
        break;
    case __KUMIR_CHAR:
        Kumir::IO::writeChar(format2, value.data.c);
        break;
    case __KUMIR_STRING:
        Kumir::IO::writeString(format2, std::wstring(value.data.s));
        break;
    default:
        break;
    }
}

EXTERN void __kumir_output_stdout_is(const __kumir_scalar value, const int format1, const __kumir_scalar format2)
{
    __kumir_check_value_defined(format2);
    __kumir_output_stdout_ii(value, format1, format2.data.i);
}

EXTERN void __kumir_output_stdout_si(const __kumir_scalar value, const __kumir_scalar format1, const int format2)
{
    __kumir_check_value_defined(format1);
    __kumir_output_stdout_ii(value, format1.data.i, format2);
}

EXTERN void __kumir_output_stdout_ss(const __kumir_scalar value, const __kumir_scalar format1, const __kumir_scalar format2)
{
    __kumir_check_value_defined(format1);
    __kumir_check_value_defined(format2);
    __kumir_output_stdout_ii(value, format1.data.i, format2.data.i);
}

EXTERN void __kumir_output_file_ii(const __kumir_scalar handle, const __kumir_scalar value, const int format1, const int format2)
{
    __kumir_check_value_defined(handle);
    __kumir_check_value_defined(value);
    Kumir::FileType f = __kumir_scalar_to_file_type(handle);
    switch (value.type) {
    case __KUMIR_INT:
        Kumir::IO::writeInteger(format2, value.data.i, f, false);
        break;
    case __KUMIR_REAL:
        Kumir::IO::writeReal(format2, format1, value.data.r, f, false);
        break;
    case __KUMIR_BOOL:
        Kumir::IO::writeBool(format2, value.data.b, f, false);
        break;
    case __KUMIR_CHAR:
        Kumir::IO::writeChar(format2, value.data.c, f, false);
        break;
    case __KUMIR_STRING:
        Kumir::IO::writeString(format2, std::wstring(value.data.s), f, false);
        break;
    default:
        break;
    }
}

EXTERN void __kumir_output_file_is(const __kumir_scalar handle, const __kumir_scalar value, const int format1, const __kumir_scalar format2)
{
    __kumir_check_value_defined(format2);
    __kumir_output_file_ii(handle, value, format1, format2.data.i);
}

EXTERN void __kumir_output_file_si(const __kumir_scalar handle, const __kumir_scalar value, const __kumir_scalar format1, const int format2)
{
    __kumir_check_value_defined(format1);
    __kumir_output_file_ii(handle, value, format1.data.i, format2);
}

EXTERN void __kumir_output_file_ss(const __kumir_scalar handle, const __kumir_scalar value, const __kumir_scalar format1, const __kumir_scalar format2)
{
    __kumir_check_value_defined(format1);
    __kumir_check_value_defined(format2);
    __kumir_output_file_ii(handle, value, format1.data.i, format2.data.i);
}

static __kumir_scalar __kumir_file_type_to_scalar(const Kumir::FileType &f)
{
    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_RECORD;
    result.data.u.nfields = 4u;
    __kumir_scalar_type * types = reinterpret_cast<__kumir_scalar_type*>(
                calloc(result.data.u.nfields, sizeof(__kumir_scalar_type))
                );
    __kumir_variant * fields = reinterpret_cast<__kumir_variant*>(
                calloc(result.data.u.nfields, sizeof(__kumir_variant))
                );

    types[0] = __KUMIR_STRING;
    types[1] = __KUMIR_INT;
    types[2] = __KUMIR_INT;
    types[3] = __KUMIR_BOOL;

    fields[0].s = reinterpret_cast<wchar_t*>(calloc(f.fullPath.length() + 1u, sizeof(wchar_t)));
    wcsncpy(fields[0].s, f.fullPath.c_str(), f.fullPath.length());
    fields[0].s[f.fullPath.length()] = L'\0';

    fields[1].i = f.mode;
    fields[2].i = f.type;
    fields[3].b = f.valid;

    result.data.u.fields = fields;
    result.data.u.types = types;

    return result;
}

static Kumir::FileType __kumir_scalar_to_file_type(const __kumir_scalar & scalar)
{
    Kumir::FileType f;
    __kumir_variant * fields = reinterpret_cast<__kumir_variant*>(scalar.data.u.fields);
    f.fullPath = std::wstring(fields[0].s);
    f.mode = fields[1].i;
    f.type = fields[2].i;
    f.valid = fields[3].b;
    return f;
}

EXTERN __kumir_scalar __kumir__stdlib__otkryit_na_chtenie(const __kumir_scalar name)
{
    __kumir_check_value_defined(name);
    std::wstring wsname(name.data.s);
    Kumir::FileType f = Kumir::Files::open(wsname, Kumir::FileType::Read);
    return __kumir_file_type_to_scalar(f);
}

EXTERN __kumir_scalar __kumir__stdlib__otkryit_na_zapis(const __kumir_scalar name)
{
    __kumir_check_value_defined(name);
    std::wstring wsname(name.data.s);
    Kumir::FileType f = Kumir::Files::open(wsname, Kumir::FileType::Write);
    return __kumir_file_type_to_scalar(f);
}

EXTERN void __kumir__stdlib__zakryit(const __kumir_scalar handle)
{
    __kumir_check_value_defined(handle);
    Kumir::FileType f = __kumir_scalar_to_file_type(handle);
    Kumir::Files::close(f);
}

static __kumir_real __kumir_scalar_as_real(const __kumir_scalar scalar)
{
    __kumir_real result = 0.;
    if (__KUMIR_REAL == scalar.type) {
        result = scalar.data.r;
    }
    else if (__KUMIR_INT == scalar.type) {
        result = static_cast<__kumir_real>(scalar.data.i);
    }
    return result;
}

static std::wstring __kumir_scalar_as_wstring(const __kumir_scalar scalar)
{
    std::wstring result;
    if (__KUMIR_CHAR == scalar.type) {
        result.push_back(scalar.data.c);
    }
    else if (__KUMIR_STRING == scalar.type) {
        result = std::wstring(scalar.data.s);
    }
    return result;
}

static signed char __kumir_compare_scalars(const __kumir_scalar left, const __kumir_scalar right)
{
    signed char result = 0;
    if (__KUMIR_INT == left.type && __KUMIR_INT == right.type) {
        const __kumir_int l = left.data.i;
        const __kumir_int r = right.data.i;
        if (l < r) result = 1;
        else if (l > r) result = -1;
        else if (l == r) result = 0;
    }
    else if (__KUMIR_REAL == left.type || __KUMIR_REAL == right.type) {
        const __kumir_real l = __kumir_scalar_as_real(left);
        const __kumir_real r = __kumir_scalar_as_real(right);
        if (l < r) result = 1;
        else if (l > r) result = -1;
        else if (l == r) result = 0;
    }
    else if (__KUMIR_BOOL == left.type && __KUMIR_BOOL == right.type) {
        const __kumir_bool l = left.data.b;
        const __kumir_bool r = right.data.b;
        if (l == r) result = 0;
        else result = 1;
    }
    else if (__KUMIR_STRING == left.type || __KUMIR_STRING == right.type) {
        const std::wstring l = __kumir_scalar_as_wstring(left);
        const std::wstring r = __kumir_scalar_as_wstring(right);
        if (l < r) result = 1;
        else if (l > r) result = -1;
        else if (l == r) result = 0;
    }
    else if (__KUMIR_CHAR == left.type && __KUMIR_CHAR == right.type) {
        const __kumir_char l = left.data.c;
        const __kumir_char r = right.data.c;
        if (l < r) result = 1;
        else if (l > r) result = -1;
        else if (l == r) result = 0;
    }
    return result;
}

EXTERN __kumir_scalar __kumir_operator_eq(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_BOOL;
    signed char v = __kumir_compare_scalars(left, right);
    result.data.b = v == 0;

    return result;
}

EXTERN __kumir_scalar __kumir_operator_ls(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_BOOL;
    result.data.b = __kumir_compare_scalars(left, right) == 1;

    return result;
}

EXTERN __kumir_scalar __kumir_operator_gt(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_BOOL;
    result.data.b = __kumir_compare_scalars(left, right) == -1;

    return result;
}

EXTERN __kumir_scalar __kumir_operator_lq(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_BOOL;
    result.data.b = __kumir_compare_scalars(left, right) >= 0;

    return result;
}

EXTERN __kumir_scalar __kumir_operator_gq(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_BOOL;
    result.data.b = __kumir_compare_scalars(left, right) <= 0;

    return result;
}

EXTERN __kumir_scalar __kumir_operator_neq(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_scalar result = __kumir_operator_eq(left, right);
    result.data.b = !result.data.b;
    return result;
}

EXTERN void __kumir_assert(const __kumir_scalar assumption)
{
    __kumir_check_value_defined(assumption);
    bool value = assumption.data.b;
    if (!value) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Утверждение ложно"));
        __kumir_handle_abort();
    }
}

EXTERN void __kumir_init_stdlib()
{
    Kumir::initStandardLibrary();
}

extern void __kumir_init_array_1(__kumir_array * array,
                                 __kumir_int left_1,
                                 __kumir_int right_1
                                 )
{

}

EXTERN void __kumir_stdlib_init()
{
    Kumir::initStandardLibrary();
}

EXTERN void test_456(__kumir_scalar abc)
{
    std::cout << abc.data.i;
}

EXTERN void test_123()
{    
    __kumir_scalar aaa = test_890();
//    test_456(aaa);
    __kumir_scalar bbb = __kumir_create_string("Превед, Медвед!");

}

EXTERN __kumir_scalar test_890()
{
    __kumir_scalar aaa;
    aaa.data.i = 12345678;
    return aaa;
}
