#include "__kumir__.h"
#include <stdlib.h>
#include <wchar.h>


extern void __allocate_array__(struct __array__ * arr)
{
  int size = 1;
  for (int i=0; i<arr->dim; i++) {
    if (arr->rights[i]<arr->lefts[i]) {
        __abort__(L"Incorrect array bound", -1);
    }
    arr->sizes[i] = (arr->rights[i]-arr->lefts[i]+1);
    size *= arr->sizes[i];
  }
  size_t elemSize;
  switch (arr->type) {
    case 'i': elemSize = sizeof(int); break;
    case 'r': elemSize = sizeof(double); break;
    case 'b': elemSize = sizeof(unsigned char); break;
    case 'c': elemSize = sizeof(wchar_t); break;
    case 's': elemSize = sizeof(wchar_t*); break;
    default: elemSize = sizeof(void*);
  }
  wprintf(L"Total size: %d\n", size);
  arr->data = calloc(size, elemSize);
}

extern void* __element__(struct __array__ * arr, int i0, int i1, int i2)
{
  size_t elemSize;
  switch (arr->type) {
    case 'i': elemSize = sizeof(int); break;
    case 'r': elemSize = sizeof(double); break;
    case 'b': elemSize = sizeof(unsigned char); break;
    case 'c': elemSize = sizeof(wchar_t); break;
    case 's': elemSize = sizeof(wchar_t*); break;
    default: elemSize = sizeof(void*);
  }
  int index = 0;
  switch (arr->dim) {
    case 1:
      index = i0 - arr->lefts[0];
      break;
    case 2: index = ( i0 - arr->lefts[0] ) * arr->sizes[1]
      + i1 - arr->lefts[1];
      break;
    case 3:
      index = ( i0 - arr->lefts[0] ) * arr->sizes[1] * arr->sizes[2]
      + ( i1 - arr->lefts[1] ) * arr->sizes[2]
      + ( i2 - arr->lefts[2] );
      break;
    default:
      index = 0;
  }
  int * i = 0;
  double * r = 0;
  wchar_t * c = 0;
  unsigned char * b = 0;
  wchar_t* * s = 0;
  switch (arr->type) {
    case 'i':
      i = (int*)(arr->data);
      return &(i[index]);
    case 'r':
      r = (double*)(arr->data);
      return &(r[index]);
    case 'c':
      c = (wchar_t*)(arr->data);
      return &(c[index]);
    case 's':
      s = (wchar_t* *)(arr->data);
      return &(s[index]);
    default:
      return 0;
  }
}


extern wchar_t* __string_from_constant__(const wchar_t * str)
{
    int len = wcslen(str);
    wchar_t* result = (wchar_t*)calloc(len+1, sizeof(wchar_t));
    wcscpy(result, str);
    return result;
}

extern wchar_t* __concatenate_ss__(const wchar_t * s1, const wchar_t * s2)
{
    int len = wcslen(s1) + wcslen(s2);
    wchar_t* result = (wchar_t*)calloc(len+1, sizeof(wchar_t));
    wcscpy(result, s1);
    wcscat(result, s2);
    return result;
}

extern wchar_t* __concatenate_sc__(const wchar_t * s, wchar_t ch)
{
    int len = wcslen(s) + 1;
    wchar_t* result = (wchar_t*)calloc(len+1, sizeof(wchar_t));
    wcscpy(result, s);
    result[len-1] = ch;
    result[len] = L'\0';
    return result;
}

extern wchar_t* __concatenate_cs__(wchar_t ch, const wchar_t * s)
{
    int len = wcslen(s);
    wchar_t* result = (wchar_t*)calloc(len+2, sizeof(wchar_t));
    result[0] = ch;
    for (int i=0; i<len; i++) {
        result[i+1] = s[i];
    }
    result[len+1] = L'\0';
    return result;
}

extern wchar_t* __concatenate_cc__(wchar_t ch1, wchar_t ch2)
{
    wchar_t* result = (wchar_t*)calloc(3, sizeof(wchar_t));
    result[0] = ch1;
    result[1] = ch2;
    result[2] = L'\0';
    return result;
}

extern void __abort__(const wchar_t * message, int lineNo)
{
    if (lineNo!=-1) {
        wprintf(L"RUNTIME ERROR AT LINE %i: %s\n", lineNo, message);
    }
    else {
        wprintf(L"RUNTIME ERROR: %s\n", message);
    }
    exit(1);
}
