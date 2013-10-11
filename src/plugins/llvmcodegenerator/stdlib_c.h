#ifndef STDLIB_C_H
#define STDLIB_C_H

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

#include "kumtypes.h"

EXTERN void __kumir_init_stdlib();
static void __kumir_handle_abort();
EXTERN __kumir_scalar __kumir_create_undefined_scalar();
EXTERN __kumir_scalar __kumir_create_defined_scalar(const __kumir_scalar value);
EXTERN __kumir_scalar __kumir_create_int(__kumir_int value);
EXTERN __kumir_scalar __kumir_create_real(__kumir_real value);
EXTERN __kumir_scalar __kumir_create_bool(__kumir_bool value);
EXTERN __kumir_scalar __kumir_create_char(const char * utf8);
EXTERN __kumir_scalar __kumir_create_string(const char * utf8);
EXTERN __kumir_variant __kumir_copy_variant(const __kumir_variant rvalue, __kumir_scalar_type type);
EXTERN __kumir_scalar __kumir_copy_scalar(const __kumir_scalar rvalue);
EXTERN void __kumir_check_value_defined(const __kumir_scalar value);
EXTERN void __kumir_free_scalar(__kumir_scalar scalar);

EXTERN void __kumir_output_stdout_ii(const __kumir_scalar value, const int format1, const int format2);
EXTERN void __kumir_output_stdout_is(const __kumir_scalar value, const int format1, const __kumir_scalar format2);
EXTERN void __kumir_output_stdout_si(const __kumir_scalar value, const __kumir_scalar format1, const int format2);
EXTERN void __kumir_output_stdout_ss(const __kumir_scalar value, const __kumir_scalar format1, const __kumir_scalar format2);

EXTERN void __kumir_output_file_ii(const __kumir_scalar handle, const __kumir_scalar value, const int format1, const int format2);
EXTERN void __kumir_output_file_is(const __kumir_scalar handle, const __kumir_scalar value, const int format1, const __kumir_scalar format2);
EXTERN void __kumir_output_file_si(const __kumir_scalar handle, const __kumir_scalar value, const __kumir_scalar format1, const int format2);
EXTERN void __kumir_output_file_ss(const __kumir_scalar handle, const __kumir_scalar value, const __kumir_scalar format1, const __kumir_scalar format2);

EXTERN void __kumir_assert(const __kumir_scalar assumption);

EXTERN __kumir_scalar __kumir_operator_eq(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_ls(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_gt(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_lq(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_gq(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_neq(const __kumir_scalar left, const __kumir_scalar right);

EXTERN __kumir_scalar __kumir_operator_sum(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_sub(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_mul(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_div(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_pow(const __kumir_scalar left, const __kumir_scalar right);

EXTERN __kumir_scalar __kumir_operator_neg(const __kumir_scalar left);
EXTERN __kumir_scalar __kumir_operator_and(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir_operator_or(const __kumir_scalar left, const __kumir_scalar right);

EXTERN __kumir_array __kumir_create_array_1(
                                 const __kumir_scalar left_1,
                                 const __kumir_scalar right_1
                                 );
EXTERN __kumir_array __kumir_create_array_2(
                                 const __kumir_scalar left_1,
                                 const __kumir_scalar right_1,
                                 const __kumir_scalar left_2,
                                 const __kumir_scalar right_2
                                 );
EXTERN __kumir_array __kumir_create_array_3(
                                 const __kumir_scalar left_1,
                                 const __kumir_scalar right_1,
                                 const __kumir_scalar left_2,
                                 const __kumir_scalar right_2,
                                 const __kumir_scalar left_3,
                                 const __kumir_scalar right_3
                                 );

EXTERN void __kumir_fill_array_i(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_r(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_b(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_c(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_s(__kumir_array array, const char * data);

EXTERN __kumir_scalar __kumir_get_array_1_element(__kumir_array array, const __kumir_scalar x);
EXTERN __kumir_scalar __kumir_get_array_2_element(__kumir_array array, const __kumir_scalar y, const __kumir_scalar x);
EXTERN __kumir_scalar __kumir_get_array_3_element(__kumir_array array, const __kumir_scalar z, const __kumir_scalar y, const __kumir_scalar x);

EXTERN void __kumir_set_array_1_element(__kumir_array array, const __kumir_scalar x, const __kumir_scalar value);
EXTERN void __kumir_set_array_2_element(__kumir_array array, const __kumir_scalar y, const __kumir_scalar x, const __kumir_scalar value);
EXTERN void __kumir_set_array_3_element(__kumir_array array, const __kumir_scalar z, const __kumir_scalar y, const __kumir_scalar x, const __kumir_scalar value);

EXTERN void __kumir_loop_for_from_to_init_counter(const __kumir_scalar from);
EXTERN void __kumir_loop_for_from_to_step_init_counter(const __kumir_scalar from, const __kumir_scalar step);

EXTERN __kumir_bool __kumir_loop_for_from_to_check_counter(__kumir_scalar * variable, const __kumir_scalar from, const __kumir_scalar to);
EXTERN __kumir_bool __kumir_loop_for_from_to_step_check_counter(__kumir_scalar * variable, const __kumir_scalar from, const __kumir_scalar to, const __kumir_scalar step);

EXTERN void __kumir_loop_times_init_counter(const __kumir_scalar from);
EXTERN __kumir_bool __kumir_loop_times_check_counter();

EXTERN void __kumir_loop_end_counter();


// Math

EXTERN __kumir_scalar __kumir__stdlib__div(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir__stdlib__mod(const __kumir_scalar left, const __kumir_scalar right);
EXTERN __kumir_scalar __kumir__stdlib__ln(const __kumir_scalar value);
EXTERN __kumir_scalar __kumir__stdlib__lg(const __kumir_scalar value);
EXTERN __kumir_scalar __kumir__stdlib__exp(const __kumir_scalar value);

// Files
EXTERN __kumir_scalar __kumir__stdlib__KATALOG_PROGRAMMYi();
EXTERN __kumir_scalar __kumir__stdlib__otkryit_na_chtenie(const __kumir_scalar name);
EXTERN __kumir_scalar __kumir__stdlib__otkryit_na_zapis(const __kumir_scalar name);
EXTERN void __kumir__stdlib__zakryit(const __kumir_scalar handle);

EXTERN void test_123();
EXTERN void test_456(__kumir_scalar arg);
EXTERN __kumir_scalar test_890();

#endif // STDLIB_C_H
