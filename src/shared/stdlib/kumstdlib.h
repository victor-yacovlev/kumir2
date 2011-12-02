#ifndef STDLIB_H
#define STDLIB_H

#ifdef STDLIB_LIBRARY
#define STDLIB_EXPORT Q_DECL_EXPORT
#else
#define STDLIB_EXPORT Q_DECL_IMPORT
#endif

#include <QtCore>

struct __array__ {
    int dim;
    int lefts[3];
    int rights[3];
    int sizes[3];
    void * data;
    char type;
};

extern STDLIB_EXPORT QString __get_error__st_funct();

extern "C" STDLIB_EXPORT void __allocate_array__(struct __array__ * arr);
extern "C" STDLIB_EXPORT void* __element__(struct __array__ * arr, int i0, int i1, int i2);

extern "C" STDLIB_EXPORT wchar_t* __string_from_constant__(const wchar_t * str);
extern "C" STDLIB_EXPORT wchar_t* __concatenate_ss__(const wchar_t * s1, const wchar_t * s2);
extern "C" STDLIB_EXPORT wchar_t* __concatenate_sc__(const wchar_t * s, wchar_t ch);
extern "C" STDLIB_EXPORT wchar_t* __concatenate_cs__(wchar_t ch, const wchar_t * s);
extern "C" STDLIB_EXPORT wchar_t* __concatenate_cc__(wchar_t ch1, wchar_t ch2);

extern "C" STDLIB_EXPORT wchar_t __get_char_at__(const wchar_t * s, int index);
extern "C" STDLIB_EXPORT void __set_char_at__(wchar_t * s, int index, wchar_t ch);

extern "C" STDLIB_EXPORT wchar_t* __get_slice__(const wchar_t * s, int start, int end);
extern "C" STDLIB_EXPORT void __set_slice_s__(wchar_t ** s, int start, int end, const wchar_t * ss);
extern "C" STDLIB_EXPORT void __set_slice_c__(wchar_t ** s, int start, int end, wchar_t ch);

extern "C" STDLIB_EXPORT wchar_t* __string_of_char__(wchar_t ch);

extern "C" STDLIB_EXPORT void __abort__(const wchar_t * message, int lineNo);
extern "C" STDLIB_EXPORT void __halt__(int lineNo) ;

extern "C" STDLIB_EXPORT void __init_garbage_collector__();
extern "C" STDLIB_EXPORT void __free_garbage_collector__();

extern "C" STDLIB_EXPORT void __garbage_collector_begin_algorhitm__();
extern "C" STDLIB_EXPORT void __garbage_collector_register_string__(wchar_t * s);
extern "C" STDLIB_EXPORT void __garbage_collector_register_array__(struct __array__ * a);
extern "C" STDLIB_EXPORT void __garbage_collector_set_return_value__(wchar_t * s);
extern "C" STDLIB_EXPORT void __garbage_collector_end_algorhitm__();

extern "C" STDLIB_EXPORT double __safe_div_rr__(double a, double b);
extern "C" STDLIB_EXPORT double __safe_div_ri__(double a, int b);
extern "C" STDLIB_EXPORT double __safe_div_ir__(int a, double b);
extern "C" STDLIB_EXPORT double __safe_div_ii__(int a, int b);
extern "C" STDLIB_EXPORT double __real_power__(double a, double b);
extern "C" STDLIB_EXPORT int __integer_power__(int a, int b);

extern "C" STDLIB_EXPORT void __init__st_funct();
extern "C" STDLIB_EXPORT double abs__st_funct(double x);
extern "C" STDLIB_EXPORT double arccos__st_funct(double x);
extern "C" STDLIB_EXPORT double arcctg__st_funct(double x);
extern "C" STDLIB_EXPORT double arcsin__st_funct(double x);
extern "C" STDLIB_EXPORT double arctg__st_funct(double x);
extern "C" STDLIB_EXPORT double cos__st_funct(double x);
extern "C" STDLIB_EXPORT double ctg__st_funct(double x);
extern "C" STDLIB_EXPORT void delay__st_funct(int s);
extern "C" STDLIB_EXPORT int div__st_funct(int x, int y);
extern "C" STDLIB_EXPORT double exp__st_funct(double x);
extern "C" STDLIB_EXPORT int iabs__st_funct(int x);
extern "C" STDLIB_EXPORT int imax__st_funct(int x, int y);
extern "C" STDLIB_EXPORT int imin__st_funct(int x, int y);
extern "C" STDLIB_EXPORT int int__st_funct(double x);
extern "C" STDLIB_EXPORT int irand__st_funct(int a, int b);
extern "C" STDLIB_EXPORT int irnd__st_funct(int x);
extern "C" STDLIB_EXPORT double lg__st_funct(double x);
extern "C" STDLIB_EXPORT double ln__st_funct(double x);
extern "C" STDLIB_EXPORT double max__st_funct(double x, double y);
extern "C" STDLIB_EXPORT double min__st_funct(double x, double y);
extern "C" STDLIB_EXPORT int mod__st_funct(int x, int y);
extern "C" STDLIB_EXPORT double rand__st_funct(double a, double b);
extern "C" STDLIB_EXPORT double rnd__st_funct(double x);
extern "C" STDLIB_EXPORT int sign__st_funct(double x);
extern "C" STDLIB_EXPORT double sin__st_funct(double x);
extern "C" STDLIB_EXPORT double sqrt__st_funct(double x);
extern "C" STDLIB_EXPORT double tg__st_funct(double x);
extern "C" STDLIB_EXPORT double MAXREAL__st_funct();
extern "C" STDLIB_EXPORT int MAXINT__st_funct();
extern "C" STDLIB_EXPORT wchar_t* real_to_string__st_funct(double x);
extern "C" STDLIB_EXPORT int time__st_funct();
extern "C" STDLIB_EXPORT int length__st_funct(wchar_t * s);
extern "C" STDLIB_EXPORT int code__st_funct(wchar_t c);
extern "C" STDLIB_EXPORT double string_to_real__st_funct(wchar_t * s, unsigned char *success);
extern "C" STDLIB_EXPORT int string_to_int__st_funct(wchar_t * s, unsigned char *success);
extern "C" STDLIB_EXPORT wchar_t symbol__st_funct(int n);
extern "C" STDLIB_EXPORT wchar_t symbol2__st_funct(int n);
extern "C" STDLIB_EXPORT wchar_t * int_to_string__st_funct(int x);
extern "C" STDLIB_EXPORT int unicode__st_funct(wchar_t c);

extern "C" STDLIB_EXPORT unsigned char is_file_exists__st_funct(wchar_t * file_name);
extern "C" STDLIB_EXPORT int remove_file__st_funct(wchar_t * file_name);
extern "C" STDLIB_EXPORT void set_file_encoding__st_funct(wchar_t * encoding);
extern "C" STDLIB_EXPORT int open_file_r__st_funct(wchar_t * file_name);
extern "C" STDLIB_EXPORT int open_file_w__st_funct(wchar_t * file_name);
extern "C" STDLIB_EXPORT int open_file_a__st_funct(wchar_t * file_name);
extern "C" STDLIB_EXPORT void close_file__st_funct(int file_handle);
extern "C" STDLIB_EXPORT void reset_file__st_funct(int file_handle);
extern "C" STDLIB_EXPORT unsigned char is_file_at_end__st_funct(int file_handle);
extern "C" STDLIB_EXPORT void __foutput2__st_funct(int file_handle, wchar_t * data);





#endif
