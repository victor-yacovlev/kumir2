#ifndef STDLIB_C_H
#define STDLIB_C_H

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif

#include "kumtypes.h"

EXTERN void __kumir_internal_debug(int32_t code);
EXTERN void __kumir_init_stdlib();
static void __kumir_handle_abort();
EXTERN void __kumir_halt();
EXTERN void __kumir_create_undefined_scalar(__kumir_scalar * result);
EXTERN void __kumir_create_undefined_array(__kumir_array * result);
EXTERN void __kumir_create_defined_scalar(__kumir_scalar * lvalue, const __kumir_scalar * value);
EXTERN void __kumir_create_int(__kumir_scalar * result, const __kumir_int value);
EXTERN void __kumir_create_real(__kumir_scalar * result, const __kumir_real value);
EXTERN void __kumir_create_bool(__kumir_scalar * result, const __kumir_bool value);
EXTERN void __kumir_create_char(__kumir_scalar * result, const char * utf8);
EXTERN void __kumir_create_string(__kumir_scalar * result, const char * utf8);
EXTERN __kumir_variant __kumir_copy_variant(const __kumir_variant rvalue, __kumir_scalar_type type);
EXTERN void __kumir_copy_scalar(__kumir_scalar  * lvalue_ptr, const __kumir_scalar * rvalue);
EXTERN void __kumir_move_scalar(__kumir_scalar  * lvalue, __kumir_scalar * rvalue);
EXTERN void __kumir_store_scalar(__kumir_scalar  ** lvalue, const __kumir_scalar * rvalue);
EXTERN void __kumir_modify_string(__kumir_stringref * lvalue, const __kumir_scalar * rvalue);
EXTERN void __kumir_check_value_defined(const __kumir_scalar * value);
EXTERN __kumir_bool __kumir_scalar_as_bool(const __kumir_scalar * scalar);
EXTERN void __kumir_free_scalar(__kumir_scalar * scalar);

EXTERN void __kumir_output_stdout_ii(const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const int format2);
EXTERN void __kumir_output_stdout_is(const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const __kumir_scalar * format2);
EXTERN void __kumir_output_stdout_si(const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const int format2);
EXTERN void __kumir_output_stdout_ss(const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const __kumir_scalar * format2);

EXTERN void __kumir_output_file_ii(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const int format2);
EXTERN void __kumir_output_file_is(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const __kumir_scalar * format2);
EXTERN void __kumir_output_file_si(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const int format2);
EXTERN void __kumir_output_file_ss(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const __kumir_scalar * format2);

EXTERN void __kumir_input_stdin(const __kumir_int format, __kumir_scalar ** ptr);
EXTERN void __kumir_input_file(const __kumir_scalar * handle, const __kumir_int format, __kumir_scalar ** ptr);

EXTERN void __kumir_set_current_line_number(const int32_t line_no);
EXTERN void __kumir_assert(const __kumir_scalar * assumption);
EXTERN void __kumir_abort_on_error(const char * message);

EXTERN void __kumir_operator_eq(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_ls(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_gt(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_lq(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_gq(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_neq(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);

EXTERN void __kumir_operator_sum(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_sub(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_mul(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_div(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir_operator_pow(__kumir_scalar * result, const __kumir_scalar * left, const __kumir_scalar * right);

EXTERN void __kumir_operator_neg(__kumir_scalar * result, const __kumir_scalar * left);

EXTERN void __kumir_create_array_1(__kumir_array * result,
                                   const __kumir_scalar * left_1,
                                   const __kumir_scalar * right_1
                                   );

EXTERN void __kumir_create_array_ref_1(__kumir_array * result,
                                       const __kumir_scalar * left_1,
                                       const __kumir_scalar * right_1
                                       );

EXTERN void __kumir_create_array_copy_1(__kumir_array * result,
                                        const __kumir_scalar * left_1,
                                        const __kumir_scalar * right_1
                                        );

EXTERN void __kumir_create_array_2(__kumir_array * result,
                                   const __kumir_scalar * left_1,
                                   const __kumir_scalar * right_1,
                                   const __kumir_scalar * left_2,
                                   const __kumir_scalar * right_2
                                   );

EXTERN void __kumir_create_array_ref_2(__kumir_array * result,
                                       const __kumir_scalar * left_1,
                                       const __kumir_scalar * right_1,
                                       const __kumir_scalar * left_2,
                                       const __kumir_scalar * right_2
                                       );

EXTERN void __kumir_create_array_copy_2(__kumir_array * result,
                                        const __kumir_scalar * left_1,
                                        const __kumir_scalar * right_1,
                                        const __kumir_scalar * left_2,
                                        const __kumir_scalar * right_2
                                        );

EXTERN void __kumir_create_array_3(__kumir_array * result,
                                   const __kumir_scalar * left_1,
                                   const __kumir_scalar * right_1,
                                   const __kumir_scalar * left_2,
                                   const __kumir_scalar * right_2,
                                   const __kumir_scalar * left_3,
                                   const __kumir_scalar * right_3
                                   );

EXTERN void __kumir_create_array_ref_3(__kumir_array * result,
                                       const __kumir_scalar * left_1,
                                       const __kumir_scalar * right_1,
                                       const __kumir_scalar * left_2,
                                       const __kumir_scalar * right_2,
                                       const __kumir_scalar * left_3,
                                       const __kumir_scalar * right_3
                                       );

EXTERN void __kumir_create_array_copy_3(__kumir_array * result,
                                        const __kumir_scalar * left_1,
                                        const __kumir_scalar * right_1,
                                        const __kumir_scalar * left_2,
                                        const __kumir_scalar * right_2,
                                        const __kumir_scalar * left_3,
                                        const __kumir_scalar * right_3
                                        );


EXTERN void __kumir_link_array(__kumir_array * result, const __kumir_array * from);
EXTERN void __kumir_free_array(__kumir_array * array);

EXTERN void __kumir_fill_array_i(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_r(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_b(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_c(__kumir_array array, const char * data);
EXTERN void __kumir_fill_array_s(__kumir_array array, const char * data);

EXTERN void __kumir_get_array_1_element(__kumir_scalar ** result,
                                        bool value_expected,
                                        __kumir_array * array,
                                        const __kumir_scalar * x);

EXTERN void __kumir_get_array_2_element(__kumir_scalar ** result,
                                        bool value_expected,
                                        __kumir_array * array,
                                        const __kumir_scalar * y,
                                        const __kumir_scalar * x);

EXTERN void __kumir_get_array_3_element(__kumir_scalar ** result,
                                        bool value_expected,
                                        __kumir_array * array,
                                        const __kumir_scalar * z,
                                        const __kumir_scalar * y,
                                        const __kumir_scalar * x);

EXTERN void __kumir_get_string_slice_ref(__kumir_stringref * result,
                                     __kumir_scalar ** sptr,
                                     const __kumir_scalar * from,
                                     const __kumir_scalar * to);

EXTERN void __kumir_get_string_element_ref(__kumir_stringref * result,
                                     __kumir_scalar ** sptr,
                                     const __kumir_scalar * at);

EXTERN void __kumir_get_string_slice(__kumir_scalar * result,
                                     const __kumir_scalar ** sptr,
                                     const __kumir_scalar * from,
                                     const __kumir_scalar * to);

EXTERN void __kumir_get_string_element(__kumir_scalar * result,
                                       const __kumir_scalar ** sptr,
                                       const __kumir_scalar * at);

EXTERN void __kumir_loop_for_from_to_init_counter(const __kumir_scalar * from);
EXTERN void __kumir_loop_for_from_to_step_init_counter(const __kumir_scalar * from, const __kumir_scalar * step);

EXTERN __kumir_bool __kumir_loop_for_from_to_check_counter(__kumir_scalar * variable, const __kumir_scalar * from, const __kumir_scalar * to);
EXTERN __kumir_bool __kumir_loop_for_from_to_step_check_counter(__kumir_scalar * variable, const __kumir_scalar * from, const __kumir_scalar * to, const __kumir_scalar * step);

EXTERN void __kumir_loop_times_init_counter(const __kumir_scalar * from);
EXTERN __kumir_bool __kumir_loop_times_check_counter();

EXTERN void __kumir_loop_end_counter();


// Math

EXTERN void __kumir__stdlib__div(__kumir_scalar  * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir__stdlib__mod(__kumir_scalar  * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void __kumir__stdlib__ln(__kumir_scalar  * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__lg(__kumir_scalar  * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__exp(__kumir_scalar  * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__rnd(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__iabs(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__abs(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__sign(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__int(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__arcsin(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__arccos(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__arctg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__arcctg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__tg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__ctg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__sin(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__cos(__kumir_scalar * result, const __kumir_scalar * value);

// String
EXTERN void __kumir__stdlib__tsel_v_lit(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__vesch_v_lit(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__dlin(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__lit_v_vesch(__kumir_scalar * result, const __kumir_scalar * value, __kumir_scalar * success);
EXTERN void __kumir__stdlib__lit_v_tsel(__kumir_scalar * result, const __kumir_scalar * value, __kumir_scalar * success);
EXTERN void __kumir__stdlib__yunikod(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__kod(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__simvol(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void __kumir__stdlib__simvol2(__kumir_scalar * result, const __kumir_scalar * value);

// Files
EXTERN void __kumir__stdlib__est_dannyie(__kumir_scalar  * result, const __kumir_scalar * handle);
EXTERN void __kumir__stdlib__KATALOG_PROGRAMMYi(__kumir_scalar  * result);
EXTERN void __kumir__stdlib__otkryit_na_chtenie(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void __kumir__stdlib__otkryit_na_zapis(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void __kumir__stdlib__zakryit(const __kumir_scalar * handle);

// System
EXTERN void __kumir__stdlib__vremya(__kumir_scalar * result);
EXTERN void __kumir__stdlib__zhdat(const __kumir_scalar * value);



#endif // STDLIB_C_H
