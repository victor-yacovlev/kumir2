#ifndef KUMIR_MATH_H
#define KUMIR_MATH_H

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

EXTERN double _abs(double x);
EXTERN double _arccos(double x);
EXTERN double _arcsin(double x);
EXTERN double _arcctg(double x);
EXTERN double _arctg(double x);
EXTERN double _cos(double x);
EXTERN double _sin(double x);
EXTERN double _ctg(double x);
EXTERN double _tg(double x);
EXTERN double _exp(double x);
EXTERN double _lg(double x);
EXTERN double _ln(double x);
EXTERN double _sqrt(double x);
EXTERN double _max(double x, double y);
EXTERN double _min(double x, double y);

EXTERN int _int(double x);
EXTERN int _iabs(int x);
EXTERN int _imax(int x, int y);
EXTERN int _imin(int x, int y);
EXTERN int _div(int x, int y);
EXTERN int _mod(int x, int y);

EXTERN int _maxint();
EXTERN double _maxreal();



#endif
