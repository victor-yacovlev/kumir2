#include "stdlib_c.h"
#include "stdlib/kumirstdlib.hpp"
#include <memory.h>
#include <string.h>
#include <wchar.h>

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

EXTERN __kumir_scalar __kumir_copy_scalar(const __kumir_scalar rvalue)
{
    __kumir_scalar lvalue;
    lvalue.defined = rvalue.defined;
    lvalue.type = rvalue.type;
    __kumir_check_value_defined(rvalue);
    if (lvalue.defined) {
        switch (lvalue.type) {
        case __KUMIR_INT:
            lvalue.data.i = rvalue.data.i;
            break;
        case __KUMIR_REAL:
            lvalue.data.r = rvalue.data.r;
            break;
        case __KUMIR_BOOL:
            lvalue.data.b = rvalue.data.b;
            break;
        case __KUMIR_CHAR:
            lvalue.data.c = rvalue.data.c;
            break;
        case __KUMIR_STRING:
            size_t sz = wcslen(rvalue.data.s);
            lvalue.data.s = reinterpret_cast<wchar_t*>(calloc(sz+1u, sizeof(wchar_t)));
            wcsncpy(lvalue.data.s, rvalue.data.s, sz);
            lvalue.data.s[sz] = L'\0';
            break;
        }
    }
    return lvalue;
}

EXTERN void __kumir_check_value_defined(const __kumir_scalar value)
{
    if (!value.defined) {
        Kumir::Core::abort(Kumir::Core::fromUtf8("Значение величины не определено"));
    }
}

EXTERN void __kumir_free_scalar(__kumir_scalar scalar)
{
    if (scalar.defined && __KUMIR_STRING == scalar.type) {
        free(scalar.data.s);
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
