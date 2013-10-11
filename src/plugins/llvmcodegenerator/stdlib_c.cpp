#include "stdlib_c.h"
#include "stdlib/kumirstdlib.hpp"
#include <memory.h>
#include <string.h>
#include <wchar.h>
#include <stack>

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

EXTERN __kumir_scalar __kumir_create_undefined_scalar()
{
    __kumir_scalar result;
    result.defined = false;
    return result;
}

EXTERN __kumir_scalar __kumir_create_defined_scalar(const __kumir_scalar value)
{
    return __kumir_copy_scalar(value);
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
    std::wstring wstr;
    try {
        wstr = Kumir::Core::fromUtf8(utf8string);
    }
    catch (Kumir::EncodingError) {
        Kumir::Core::abort(Kumir::Core::fromAscii("Encoding error"));
        __kumir_handle_abort();
    }
    __kumir_scalar result;
    result.defined = true;
    result.type = __KUMIR_CHAR;
    result.data.c = wstr[0];
    return result;
}

EXTERN __kumir_scalar __kumir_create_string(const char * utf8)
{
    std::string utf8string(utf8);    
    std::wstring wstr;
    try {
        wstr = Kumir::Core::fromUtf8(utf8string);
    }
    catch (Kumir::EncodingError) {
        Kumir::Core::abort(Kumir::Core::fromAscii("Encoding error"));
        __kumir_handle_abort();
    }
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

static __kumir_scalar __kumir_create_string(const std::wstring & wstr)
{
    __kumir_scalar result;
    result.type = __KUMIR_STRING;
    result.defined = true;
    result.data.s = reinterpret_cast<wchar_t*>(
                calloc(wstr.length() + 1, sizeof(wchar_t))
                );
    result.data.s[wstr.length()] = L'\0';
    wcsncpy(result.data.s, wstr.c_str(), wstr.length());
    return result;
}

// Math

EXTERN __kumir_scalar __kumir__stdlib__div(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);
    return __kumir_create_int(Kumir::Math::div(left.data.i, right.data.i));
}

EXTERN __kumir_scalar __kumir__stdlib__mod(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);
    return __kumir_create_int(Kumir::Math::mod(left.data.i, right.data.i));
}

EXTERN __kumir_scalar __kumir__stdlib__ln(const __kumir_scalar value)
{
    __kumir_check_value_defined(value);
    return __kumir_create_real(Kumir::Math::ln(value.data.r));
}

EXTERN __kumir_scalar __kumir__stdlib__lg(const __kumir_scalar value)
{
    __kumir_check_value_defined(value);
    return __kumir_create_real(Kumir::Math::lg(value.data.r));
}

EXTERN __kumir_scalar __kumir__stdlib__exp(const __kumir_scalar value)
{
    __kumir_check_value_defined(value);
    return __kumir_create_real(Kumir::Math::exp(value.data.r));
}

// Files
EXTERN __kumir_scalar __kumir__stdlib__KATALOG_PROGRAMMYi()
{
    const std::wstring res = Kumir::Files::CurrentWorkingDirectory();
    return __kumir_create_string(res);
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

EXTERN __kumir_scalar __kumir_operator_sum(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;

    if (__KUMIR_INT == left.type && __KUMIR_INT == right.type) {
        if (!Kumir::Math::checkSumm(left.data.i, right.data.i)) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Целочисленное переполнение"));
            __kumir_handle_abort();
        }
        result.data.i = left.data.i + right.data.i;
        result.type = __KUMIR_INT;
    }
    else if (__KUMIR_REAL == left.type || __KUMIR_REAL == right.type) {
        const __kumir_real l = __kumir_scalar_as_real(left);
        const __kumir_real r = __kumir_scalar_as_real(right);
        if (!Kumir::Math::checkSumm(left.data.i, right.data.i)) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Вещественное переполнение"));
            __kumir_handle_abort();
        }
        result.data.r = l + r;
        result.type = __KUMIR_REAL;
    }
    else if (__KUMIR_STRING == left.type || __KUMIR_CHAR == left.type) {
        const std::wstring l = __kumir_scalar_as_wstring(left);
        const std::wstring r = __kumir_scalar_as_wstring(right);
        const std::wstring res = l + r;
        result.data.s = reinterpret_cast<wchar_t*>(calloc(res.length()+1, sizeof(wchar_t)));
        wcsncpy(result.data.s, res.c_str(), res.length());
        result.data.s[res.length()] = L'\0';
        result.type = __KUMIR_STRING;
    }

    return result;
}

EXTERN __kumir_scalar __kumir_operator_sub(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;

    if (__KUMIR_INT == left.type && __KUMIR_INT == right.type) {
        if (!Kumir::Math::checkDiff(left.data.i, right.data.i)) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Целочисленное переполнение"));
            __kumir_handle_abort();
        }
        result.data.i = left.data.i - right.data.i;
        result.type = __KUMIR_INT;
    }
    else if (__KUMIR_REAL == left.type || __KUMIR_REAL == right.type) {
        const __kumir_real l = __kumir_scalar_as_real(left);
        const __kumir_real r = __kumir_scalar_as_real(right);
        if (!Kumir::Math::checkDiff(left.data.i, right.data.i)) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Вещественное переполнение"));
            __kumir_handle_abort();
        }
        result.data.r = l - r;
        result.type = __KUMIR_REAL;
    }

    return result;
}

EXTERN __kumir_scalar __kumir_operator_mul(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;

    if (__KUMIR_INT == left.type && __KUMIR_INT == right.type) {
        if (!Kumir::Math::checkProd(left.data.i, right.data.i)) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Целочисленное переполнение"));
            __kumir_handle_abort();
        }
        result.data.i = left.data.i * right.data.i;
        result.type = __KUMIR_INT;
    }
    else if (__KUMIR_REAL == left.type || __KUMIR_REAL == right.type) {
        const __kumir_real l = __kumir_scalar_as_real(left);
        const __kumir_real r = __kumir_scalar_as_real(right);
        result.data.r = l * r;
        if (!Kumir::Math::isCorrectReal(result.data.r)) {
            Kumir::Core::abort(Kumir::Core::fromUtf8("Вещественное переполнение"));
            __kumir_handle_abort();
        }
        result.type = __KUMIR_REAL;
    }

    return result;
}

EXTERN __kumir_scalar __kumir_operator_div(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;

    if ( (__KUMIR_INT == right.type && 0 == right.data.i) || (__KUMIR_REAL == right.type && 0. == right.data.r) ) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Деление на ноль"));
        __kumir_handle_abort();
    }


    const __kumir_real l = __kumir_scalar_as_real(left);
    const __kumir_real r = __kumir_scalar_as_real(right);
    result.data.r = l / r;
    if (!Kumir::Math::isCorrectReal(result.data.r)) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Вещественное переполнение"));
        __kumir_handle_abort();
    }
    result.type = __KUMIR_REAL;

    return result;
}

EXTERN __kumir_scalar __kumir_operator_pow(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.defined = true;

    if (__KUMIR_INT == left.type && __KUMIR_INT == right.type) {
        result.type = __KUMIR_INT;
        result.data.i = Kumir::Math::ipow(left.data.i, right.data.i);
    }
    else {
        result.type = __KUMIR_REAL;
        const __kumir_real l = __kumir_scalar_as_real(left);
        const __kumir_real r = __kumir_scalar_as_real(right);
        result.data.i = Kumir::Math::pow(l, r);
    }

    return result;
}

EXTERN __kumir_scalar __kumir_operator_neg(const __kumir_scalar left)
{
    __kumir_check_value_defined(left);
    __kumir_scalar result;
    result.defined = true;
    result.type = left.type;
    if (__KUMIR_BOOL == left.type) {
        result.data.b = ! left.data.b;
    }
    else if (__KUMIR_INT == left.type) {
        result.data.i = - left.data.i;
    }
    else if (__KUMIR_REAL == left.type) {
        result.data.r = 0.0 - left.data.r;
    }
    return result;

}

EXTERN __kumir_scalar __kumir_operator_and(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.type = __KUMIR_BOOL;
    result.data.b = left.data.b && right.data.b;
    result.defined = true;
    return result;
}

EXTERN __kumir_scalar __kumir_operator_or(const __kumir_scalar left, const __kumir_scalar right)
{
    __kumir_check_value_defined(left);
    __kumir_check_value_defined(right);

    __kumir_scalar result;
    result.type = __KUMIR_BOOL;
    result.data.b = left.data.b || right.data.b;
    result.defined = true;
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

EXTERN __kumir_array __kumir_create_array_1(
                                 const __kumir_scalar left_1,
                                 const __kumir_scalar right_1
                                 )
{
    __kumir_check_value_defined(left_1);
    __kumir_check_value_defined(right_1);
    __kumir_array result;
    result.dim = 1u;
    result.left = left_1.data.i;
    result.right = right_1.data.i;
    int isz = 1 + right_1.data.i - left_1.data.i;
    if (isz < 0) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t sz = static_cast<size_t>(isz);
        if (sz) {
            __kumir_scalar * scalars = reinterpret_cast<__kumir_scalar*>(
                    calloc(sz, sizeof(__kumir_scalar))
                    );
            result.data = scalars;
            for (size_t i=0; i<sz; i++) {
                scalars[i] = __kumir_create_undefined_scalar();
            }
        }
    }
    return result;
}

EXTERN __kumir_array __kumir_create_array_2(
                                 const __kumir_scalar left_1,
                                 const __kumir_scalar right_1,
                                 const __kumir_scalar left_2,
                                 const __kumir_scalar right_2
                                 )
{
    __kumir_check_value_defined(left_1);
    __kumir_check_value_defined(right_1);
    __kumir_check_value_defined(left_2);
    __kumir_check_value_defined(right_2);
    __kumir_array result;
    result.dim = 2u;
    result.left = left_1.data.i;
    result.right = right_1.data.i;
    int isz = 1 + right_1.data.i - left_1.data.i;
    if (isz < 0) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t sz = static_cast<size_t>(isz);
        if (sz) {
            __kumir_array * subarrays = reinterpret_cast<__kumir_array*>(
                    calloc(sz, sizeof(__kumir_array))
                    );
            result.data = subarrays;
            for (size_t i=0; i<sz; i++) {
                subarrays[i] = __kumir_create_array_1(left_2, right_2);
            }
        }
    }
    return result;
}

EXTERN __kumir_array __kumir_create_array_3(
                                 const __kumir_scalar left_1,
                                 const __kumir_scalar right_1,
                                 const __kumir_scalar left_2,
                                 const __kumir_scalar right_2,
                                 const __kumir_scalar left_3,
                                 const __kumir_scalar right_3
                                 )
{
    __kumir_check_value_defined(left_1);
    __kumir_check_value_defined(right_1);
    __kumir_check_value_defined(left_2);
    __kumir_check_value_defined(right_2);
    __kumir_check_value_defined(left_3);
    __kumir_check_value_defined(right_3);
    __kumir_array result;
    result.dim = 3u;
    result.left = left_1.data.i;
    result.right = right_1.data.i;
    int isz = 1 + right_1.data.i - left_1.data.i;
    if (isz < 0) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Неверный размер таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t sz = static_cast<size_t>(isz);
        if (sz) {
            __kumir_array * subarrays = reinterpret_cast<__kumir_array*>(
                    calloc(sz, sizeof(__kumir_array))
                    );
            result.data = subarrays;
            for (size_t i=0; i<sz; i++) {
                subarrays[i] = __kumir_create_array_2(left_2, right_2, left_3, right_3);
            }
        }
    }
    return result;
}

template<typename T>
static T __eat(const char ** pdata)
{
    const char * data = *pdata;
    const T * idata = reinterpret_cast<const T*> (data);
    T val = idata[0];
    idata++;
    data = reinterpret_cast<const char*>(idata);
    *pdata = data;
    return val;
}

static void __kumir_fill_array_1(__kumir_array array, const char *data, const __kumir_scalar_type type)
{
    const char * p = data;
    __kumir_int isz = __eat<__kumir_int>(&p);
    size_t index = 0u;
    __kumir_scalar * adata = reinterpret_cast<__kumir_scalar*>(array.data);
    while (isz) {
        adata[index].defined = __eat<__kumir_bool>(&p);
        adata[index].type = type;
        if (__KUMIR_INT == type) {
            adata[index].data.i = __eat<__kumir_int>(&p);
        }
        else if (__KUMIR_REAL == type) {
            adata[index].data.r = __eat<__kumir_real>(&p);
        }
        else if (__KUMIR_BOOL == type) {
            adata[index].data.b = __eat<__kumir_bool>(&p);
        }
        isz--;
        index++;
    }
}

static void __kumir_fill_array_2(__kumir_array array, const char *data, const __kumir_scalar_type type)
{

}

static void __kumir_fill_array_3(__kumir_array array, const char *data, const __kumir_scalar_type type)
{

}

EXTERN void __kumir_fill_array_i(__kumir_array array, const char * data)
{
    if      (1u == array.dim) __kumir_fill_array_1(array, data, __KUMIR_INT);
    else if (2u == array.dim) __kumir_fill_array_2(array, data, __KUMIR_INT);
    else if (3u == array.dim) __kumir_fill_array_3(array, data, __KUMIR_INT);
}

EXTERN void __kumir_fill_array_r(__kumir_array array, const char * data)
{
    if      (1u == array.dim) __kumir_fill_array_1(array, data, __KUMIR_REAL);
    else if (2u == array.dim) __kumir_fill_array_2(array, data, __KUMIR_REAL);
    else if (3u == array.dim) __kumir_fill_array_3(array, data, __KUMIR_REAL);
}

EXTERN void __kumir_fill_array_b(__kumir_array array, const char * data)
{
    if      (1u == array.dim) __kumir_fill_array_1(array, data, __KUMIR_BOOL);
    else if (2u == array.dim) __kumir_fill_array_2(array, data, __KUMIR_BOOL);
    else if (3u == array.dim) __kumir_fill_array_3(array, data, __KUMIR_BOOL);
}

EXTERN void __kumir_fill_array_c(__kumir_array array, const char * data)
{
    if      (1u == array.dim) __kumir_fill_array_1(array, data, __KUMIR_CHAR);
    else if (2u == array.dim) __kumir_fill_array_2(array, data, __KUMIR_CHAR);
    else if (3u == array.dim) __kumir_fill_array_3(array, data, __KUMIR_CHAR);
}

EXTERN void __kumir_fill_array_s(__kumir_array array, const char * data)
{
    if      (1u == array.dim) __kumir_fill_array_1(array, data, __KUMIR_STRING);
    else if (2u == array.dim) __kumir_fill_array_2(array, data, __KUMIR_STRING);
    else if (3u == array.dim) __kumir_fill_array_3(array, data, __KUMIR_STRING);
}

EXTERN __kumir_scalar __kumir_get_array_1_element(__kumir_array array, const __kumir_scalar x)
{
    __kumir_scalar * data = reinterpret_cast<__kumir_scalar*>(array.data);
    __kumir_check_value_defined(x);
    __kumir_int xx = x.data.i;
    __kumir_scalar result;
    if (xx < array.left || xx > array.right) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t index = static_cast<size_t>(xx - array.left);
        result = data[index];
    }
    return result;
}

EXTERN __kumir_scalar __kumir_get_array_2_element(__kumir_array array, const __kumir_scalar y, const __kumir_scalar x)
{
    __kumir_check_value_defined(x);
    __kumir_check_value_defined(y);
    __kumir_int yy = y.data.i;
    __kumir_array * data = reinterpret_cast<__kumir_array*>(array.data);
    __kumir_scalar result;
    if (yy < array.left || yy > array.right) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t index = static_cast<size_t>(yy - array.left);
        result = __kumir_get_array_1_element(data[index], x);
    }
    return result;
}

EXTERN __kumir_scalar __kumir_get_array_3_element(__kumir_array array, const __kumir_scalar z, const __kumir_scalar y, const __kumir_scalar x)
{
    __kumir_check_value_defined(x);
    __kumir_check_value_defined(y);
    __kumir_check_value_defined(z);
    __kumir_int zz = z.data.i;
    __kumir_array * data = reinterpret_cast<__kumir_array*>(array.data);
    __kumir_scalar result;
    if (zz < array.left || zz > array.right) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t index = static_cast<size_t>(zz - array.left);
        result = __kumir_get_array_2_element(data[index], y, x);
    }
    return result;
}

EXTERN void __kumir_set_array_1_element(__kumir_array array, const __kumir_scalar x, const __kumir_scalar value)
{
    __kumir_scalar * data = reinterpret_cast<__kumir_scalar*>(array.data);
    __kumir_check_value_defined(x);
    __kumir_int xx = x.data.i;
    if (xx < array.left || xx > array.right) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t index = static_cast<size_t>(xx - array.left);
        data[index] = __kumir_copy_scalar(value);
    }
}

EXTERN void __kumir_set_array_2_element(__kumir_array array, const __kumir_scalar y, const __kumir_scalar x, const __kumir_scalar value)
{
    __kumir_check_value_defined(x);
    __kumir_check_value_defined(y);
    __kumir_int yy = y.data.i;
    __kumir_array * data = reinterpret_cast<__kumir_array*>(array.data);
    if (yy < array.left || yy > array.right) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t index = static_cast<size_t>(yy - array.left);
        __kumir_set_array_1_element(data[index], x, value);
    }
}

EXTERN void __kumir_set_array_3_element(__kumir_array array, const __kumir_scalar z, const __kumir_scalar y, const __kumir_scalar x, const __kumir_scalar value)
{
    __kumir_check_value_defined(x);
    __kumir_check_value_defined(y);
    __kumir_check_value_defined(z);
    __kumir_int zz = z.data.i;
    __kumir_array * data = reinterpret_cast<__kumir_array*>(array.data);
    if (zz < array.left || zz > array.right) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Выход за границу таблицы"));
        __kumir_handle_abort();
    }
    else {
        size_t index = static_cast<size_t>(zz - array.left);
        __kumir_set_array_2_element(data[index], y, x, value);
    }
}

static std::stack<int32_t> for_counters;

EXTERN void __kumir_loop_for_from_to_init_counter(const __kumir_scalar from)
{
    int32_t val = from.data.i - 1;
    for_counters.push(val);
}

EXTERN void __kumir_loop_for_from_to_step_init_counter(const __kumir_scalar from, const __kumir_scalar step)
{
    int32_t val = from.data.i - step.data.i;
    for_counters.push(val);
}

EXTERN __kumir_bool __kumir_loop_for_from_to_check_counter(__kumir_scalar * variable, const __kumir_scalar from, const __kumir_scalar to)
{
    int32_t & i = for_counters.top();
    i += 1;
    int32_t f = from.data.i;
    int32_t t = to.data.i;
    bool result = f <= i && i <= t;
    if (result) {
        variable->data.i = i;
        variable->defined = true;
        variable->type = __KUMIR_INT;
    }
    return result;
}

EXTERN __kumir_bool __kumir_loop_for_from_to_step_check_counter(__kumir_scalar * variable, const __kumir_scalar from, const __kumir_scalar to, const __kumir_scalar step)
{
    int32_t & i = for_counters.top();
    int32_t f = from.data.i;
    int32_t t = to.data.i;
    int32_t s = step.data.i;
    i += s;
    bool result = s >= 0
            ? f <= i && i <= t
            : t >= i && i >= f;
    if (result) {
        variable->data.i = i;
        variable->defined = true;
        variable->type = __KUMIR_INT;
    }
    return result;
}

EXTERN void __kumir_loop_times_init_counter(const __kumir_scalar from)
{
    int32_t val = from.data.i;
    for_counters.push(val);
}

EXTERN __kumir_bool __kumir_loop_times_check_counter()
{
    int32_t & times = for_counters.top();
    if (times > 0) {
        times --;
        return true;
    }
    else {
        return false;
    }
}

EXTERN void __kumir_loop_end_counter()
{
    for_counters.pop();
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
