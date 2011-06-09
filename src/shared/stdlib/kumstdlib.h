#ifndef STDLIB_H
#define STDLIB_H

extern QString __get_error__st_funct();

extern "C" void __init_garbage_collector__();
extern "C" void __free_garbage_collector__();

extern "C" void __garbage_collector_begin_algorhitm__();
extern "C" void __garbage_collector_register_string__(wchar_t * s);
extern "C" void __garbage_collector_register_array__(struct __array__ * a);
extern "C" void __garbage_collector_set_return_value__(wchar_t * s);
extern "C" void __garbage_collector_end_algorhitm__();

extern "C" double __safe_div_rr__(double a, double b);
extern "C" double __safe_div_ri__(double a, int b);
extern "C" double __safe_div_ir__(int a, double b);
extern "C" double __safe_div_ii__(int a, int b);

extern "C" void __init__st_funct();
extern "C" double abs__st_funct(double x);
extern "C" double arccos__st_funct(double x);
extern "C" double arcctg__st_funct(double x);
extern "C" double arcsin__st_funct(double x);
extern "C" double arctg__st_funct(double x);
extern "C" double cos__st_funct(double x);
extern "C" double ctg__st_funct(double x);
extern "C" void delay__st_funct(int s);
extern "C" int div__st_funct(int x, int y);
extern "C" double exp__st_funct(double x);
extern "C" int iabs__st_funct(int x);
extern "C" int imax__st_funct(int x, int y);
extern "C" int imin__st_funct(int x, int y);
extern "C" int int__st_funct(double x);
extern "C" int irand__st_funct(int a, int b);
extern "C" int irnd__st_funct(int x);
extern "C" double lg__st_funct(double x);
extern "C" double ln__st_funct(double x);
extern "C" double max__st_funct(double x, double y);
extern "C" double min__st_funct(double x, double y);
extern "C" int mod__st_funct(int x, int y);
extern "C" double rand__st_funct(double a, double b);
extern "C" double rnd__st_funct(double x);
extern "C" int sign__st_funct(double x);
extern "C" double sin__st_funct(double x);
extern "C" double sqrt__st_funct(double x);
extern "C" double tg__st_funct(double x);
extern "C" double MAXREAL__st_funct();
extern "C" int MAXINT__st_funct();
extern "C" wchar_t* real_to_string__st_funct(double x);
extern "C" int time__st_funct();
extern "C" int length__st_funct(wchar_t * s);
extern "C" int code__st_funct(wchar_t c);
extern "C" double string_to_real__st_funct(wchar_t * s, unsigned char *success);
extern "C" int string_to_int__st_funct(wchar_t * s, unsigned char *success);
extern "C" wchar_t symbol__st_funct(int n);
extern "C" wchar_t symbol2__st_funct(int n);
extern "C" wchar_t * int_to_string__st_funct(int x);
extern "C" int unicode__st_funct(wchar_t c);



#endif
