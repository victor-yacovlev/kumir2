#include "kumir_math.h"
#include "kumir_core.h"
#include <math.h>

EXTERN double _abs(double x)
{
    return x>=0.0? x : -x;
}

EXTERN double _arccos(double x)
{
    if (x<-1 || x>1) {
        __error__(0);
        return 0;
    }
    else {
        return acos(x);
    }
}

EXTERN double _arcsin(double x)
{
    if (x<-1 || x>1) {
        __error__(1);
        return 0;
    }
    else {
        return asin(x);
    }
}

EXTERN double _arcctg(double x)
{
    if (x==0.0) {
        __error__(2);
        return 0;
    }
    else {
        return atan(1.0/x);
    }
}

EXTERN double _arctg(double x)
{
    return atan(x);
}

EXTERN double _cos(double x)
{
    return cos(x);
}

EXTERN double _sin(double x)
{
    return sin(x);
}

EXTERN double _ctg(double x)
{
    if (x==0.0) {
        __error__(3);
        return 0;
    }
    else {
        return atan(1.0/x);
    }
}

EXTERN double _tg(double x)
{
    return tan(x);
}

EXTERN double _exp(double x)
{
    return exp(x);
}

EXTERN double _lg(double x)
{
    if (x<=0.0) {
        __error__(4);
        return 0;
    }
    else {
        return log(x)/log(10.0);
    }
}

EXTERN double _ln(double x)
{
    if (x<=0.0) {
        __error__(5);
        return 0;
    }
    else {
        return log(x);
    }
}

EXTERN double _sqrt(double x)
{
    if (x<0) {
        __error__(6);
        return 0;
    }
    else {
        return sqrt(x);
    }
}

EXTERN double _max(double x, double y)
{
    return x>y? x:y;
}

EXTERN double _min(double x, double y)
{
    return x<y? x:y;
}

EXTERN int _int(double x)
{
    double xx = floor(x);
    return (int)(xx);
}

EXTERN int _iabs(int x)
{
    return x>=0? x:-x;
}

EXTERN int _imax(int x, int y)
{
    return x>y? x:y;
}

EXTERN int _imin(int x, int y)
{
    return x<y? x:y;
}

EXTERN int _div(int x, int y)
{
    if (y==0) {
        __error__(7);
        return 0;
    }
    else {
        return x/y;
    }
}

EXTERN int _mod(int x, int y)
{
    if (y==0) {
        __error__(8);
        return 0;
    }
    else {
        return x%y;
    }
}

EXTERN int _maxint()
{
    return 0x80000000;
}

EXTERN double _maxreal()
{
    return 1.797693e+308;
}
