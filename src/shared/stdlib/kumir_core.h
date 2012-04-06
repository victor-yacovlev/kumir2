#ifndef KUMIR_CORE_H
#define KUMIR_CORE_H

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

struct __array__ {
    int dim;
    int lefts[3];
    int rights[3];
    int sizes[3];
    void * data;
    char type;
};

EXTERN void __error__(unsigned int code);


#endif
