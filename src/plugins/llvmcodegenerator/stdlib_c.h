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
EXTERN void __kumir_check_call_stack();
EXTERN void __kumir_pop_call_stack_counter();
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

EXTERN void __kumir_convert_char_to_string(__kumir_scalar * result, const __kumir_scalar * source);
EXTERN void __kumir_convert_int_to_real(__kumir_scalar * result, const __kumir_scalar * source);

EXTERN __kumir_variant __kumir_copy_variant(const __kumir_variant rvalue, __kumir_scalar_type type);
EXTERN void __kumir_copy_scalar(__kumir_scalar  * lvalue_ptr, const __kumir_scalar * rvalue);
EXTERN void __kumir_move_scalar(__kumir_scalar  * lvalue, __kumir_scalar * rvalue);
EXTERN void __kumir_store_scalar(__kumir_scalar  ** lvalue, const __kumir_scalar * rvalue);
EXTERN void __kumir_modify_string(__kumir_stringref * lvalue, const __kumir_scalar * rvalue);
EXTERN void __kumir_check_value_defined(const __kumir_scalar * value);
EXTERN __kumir_bool __kumir_scalar_as_bool(const __kumir_scalar * scalar);
EXTERN void __kumir_free_scalar(__kumir_scalar * scalar);

EXTERN void __kumir_output_stdout_ii(const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const int format2);
EXTERN void __kumir_output_stdout(const char * utf8);
EXTERN void __kumir_output_stdout_is(const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const __kumir_scalar * format2);
EXTERN void __kumir_output_stdout_si(const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const int format2);
EXTERN void __kumir_output_stdout_ss(const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const __kumir_scalar * format2);

EXTERN void __kumir_output_file_ii(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const int format2);
EXTERN void __kumir_output_file_is(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const int format1, const __kumir_scalar * format2);
EXTERN void __kumir_output_file_si(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const int format2);
EXTERN void __kumir_output_file_ss(const __kumir_scalar * handle, const __kumir_scalar * value, const __kumir_scalar_type type, const __kumir_scalar * format1, const __kumir_scalar * format2);

EXTERN void __kumir_input_stdin(const __kumir_int format, __kumir_scalar ** ptr);
EXTERN void __kumir_input_file(const __kumir_scalar * handle, const __kumir_int format, __kumir_scalar ** ptr);

EXTERN void __kumir_get_scalar_argument(const char * argName, const __kumir_int format, __kumir_scalar * res);
EXTERN void __kumir_get_array_argument(const char * argName, const __kumir_int format, __kumir_array * res);
EXTERN void __kumir_set_main_arguments(int argc, char ** argv);

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

EXTERN void __kumir_cleanup_array_in_shape(__kumir_array * result);

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

EXTERN void __kumir_fill_array_i(__kumir_array * array, const char * data);
EXTERN void __kumir_fill_array_r(__kumir_array * array, const char * data);
EXTERN void __kumir_fill_array_b(__kumir_array * array, const char * data);
EXTERN void __kumir_fill_array_c(__kumir_array * array, const char * data);
EXTERN void __kumir_fill_array_s(__kumir_array * array, const char * data);

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

EXTERN void __kumir_loop_for_from_to_init_counter(const __kumir_scalar * from, const __kumir_scalar * to);
EXTERN void __kumir_loop_for_from_to_step_init_counter(const __kumir_scalar * from, const __kumir_scalar * to, const __kumir_scalar * step);

EXTERN __kumir_bool __kumir_loop_for_check_counter(__kumir_scalar * variable);

EXTERN void __kumir_loop_times_init_counter(const __kumir_scalar * from);
EXTERN __kumir_bool __kumir_loop_times_check_counter();

EXTERN void __kumir_loop_end_counter();

EXTERN void __kumir_print_scalar_variable(const char * name,
                                   const __kumir_scalar_type type,
                                   const __kumir_scalar * value);

EXTERN void __kumir_input_scalar_variable(const char * name,
                                   const __kumir_scalar_type type,
                                   __kumir_scalar * value);

EXTERN void __kumir_print_array_variable(const char * name,
                                   const __kumir_scalar_type type,
                                   const __kumir_array * value);

EXTERN void __kumir_input_array_variable(const char * name,
                                   const __kumir_scalar_type type,
                                   __kumir_array * ptr);


// "Kumir Standard Library" module

EXTERN void Kumir_Standard_Library_div(__kumir_scalar  * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void Kumir_Standard_Library_mod(__kumir_scalar  * result, const __kumir_scalar * left, const __kumir_scalar * right);
EXTERN void Kumir_Standard_Library_ln(__kumir_scalar  * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_lg(__kumir_scalar  * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_exp(__kumir_scalar  * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_rnd(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_irnd(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_rand(__kumir_scalar * result, const __kumir_scalar * a, const __kumir_scalar * b);
EXTERN void Kumir_Standard_Library_irand(__kumir_scalar * result, const __kumir_scalar * a, const __kumir_scalar * b);
EXTERN void Kumir_Standard_Library_iabs(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_abs(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_sign(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_int(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_arcsin(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_arccos(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_arctg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_arcctg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_tg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_ctg(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_sin(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_cos(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_sqrt(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_max(__kumir_scalar * result, const __kumir_scalar * value1, const __kumir_scalar * value2);
EXTERN void Kumir_Standard_Library_min(__kumir_scalar * result, const __kumir_scalar * value1, const __kumir_scalar * value2);
EXTERN void Kumir_Standard_Library_imax(__kumir_scalar * result, const __kumir_scalar * value1, const __kumir_scalar * value2);
EXTERN void Kumir_Standard_Library_imin(__kumir_scalar * result, const __kumir_scalar * value1, const __kumir_scalar * value2);

EXTERN void Kumir_Standard_Library_time(__kumir_scalar * result);
EXTERN void Kumir_Standard_Library_wait(const __kumir_scalar * value);

EXTERN void Kumir_Standard_Library_string_of_int(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_string_of_real(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_length(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_string_to_real(__kumir_scalar * result, const __kumir_scalar * value, __kumir_scalar * success);
EXTERN void Kumir_Standard_Library_string_to_int(__kumir_scalar * result, const __kumir_scalar * value, __kumir_scalar * success);
EXTERN void Kumir_Standard_Library_unicode(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_code(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_symbol(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_unisymbol(__kumir_scalar * result, const __kumir_scalar * value);
EXTERN void Kumir_Standard_Library_Integer(__kumir_scalar * result, const __kumir_scalar * lexem, const __kumir_scalar * def);
EXTERN void Kumir_Standard_Library_Real(__kumir_scalar * result, const __kumir_scalar * lexem, const __kumir_scalar * def);
EXTERN void Kumir_Standard_Library_Boolean(__kumir_scalar * result, const __kumir_scalar * lexem, const __kumir_scalar * def);


// "String Utilities" module

EXTERN void String_Utilities_find_after(__kumir_scalar * result, const __kumir_scalar * ot, const __kumir_scalar * fragment, const __kumir_scalar * thestring);
EXTERN void String_Utilities_find(__kumir_scalar * result, const __kumir_scalar * fragment, const __kumir_scalar * thestring);
EXTERN void String_Utilities_index_after(__kumir_scalar * result, const __kumir_scalar * ot, const __kumir_scalar * fragment, const __kumir_scalar * thestring);
EXTERN void String_Utilities_index(__kumir_scalar * result, const __kumir_scalar * fragment, const __kumir_scalar * thestring);
EXTERN void String_Utilities_lowercase(__kumir_scalar * result, const __kumir_scalar * thestring);
EXTERN void String_Utilities_uppercase(__kumir_scalar * result, const __kumir_scalar * thestring);
EXTERN void String_Utilities_insert(const __kumir_scalar * fragment, __kumir_scalar * thestring, const __kumir_scalar * position);
EXTERN void String_Utilities_remove(__kumir_scalar * source, const __kumir_scalar * position, const __kumir_scalar * thelength);
EXTERN void String_Utilities_replace(__kumir_scalar * source, const __kumir_scalar * before, const __kumir_scalar * after, const __kumir_scalar * each);

// "Files" module

EXTERN void Files_nonempty(__kumir_scalar  * result, const __kumir_scalar * handle);
EXTERN void Files_PROGRAM_DIR(__kumir_scalar  * result);
EXTERN void Files_CURRENT_DIR(__kumir_scalar  * result);
EXTERN void Files_read_open(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_write_open(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_append_open(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_readable(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_writable(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_close(const __kumir_scalar * handle);
EXTERN void Files_start_reading(const __kumir_scalar * handle);
EXTERN void Files_exists(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_isdir(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_mkdir(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_rm(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_rmdir(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_abspath(__kumir_scalar  * result, const __kumir_scalar * name);
EXTERN void Files_eof(__kumir_scalar  * result, const __kumir_scalar * handle);
EXTERN void Files_set_encoding(const __kumir_scalar * encoding);
EXTERN void Files_get_console(__kumir_scalar * result);
EXTERN void Files_operator_neq(__kumir_scalar * result, const __kumir_scalar * a, const __kumir_scalar * b);
EXTERN void Files_operator_eq(__kumir_scalar * result, const __kumir_scalar * a, const __kumir_scalar * b);



#endif // STDLIB_C_H
